/**
 * @file vision.c
 * @brief Vision service implementation.
 * @author Paracosm
 *
 * @par Processing flow
 * Raw grayscale frame → OTSU threshold → binary image → downsampled image
 * → noise filtering → track edge detection → weighted midline → PID error.
 * Frame geometry comes from SmartcarHal_Camera. The internal compressed frame is
 * fixed by VISION_ZIP_IMAGE_W/H.
 */

#include "vision.h"
#include "config.h"     /* ELEM_* 元素检测参数 */
#include "hal/hal.h"

#define OTSU_GRAY_LEVELS                         256U
#define VISION_BINARY_BLACK                      0U
#define VISION_BINARY_WHITE                      255U
#define VISION_EDGE_NOT_FOUND                    UINT8_MAX
#define VISION_BINARY_FILTER_WHITE_NEIGHBORS     2U
#define VISION_BINARY_FILTER_WHITE_SUM           ((uint16_t)(VISION_BINARY_FILTER_WHITE_NEIGHBORS * VISION_BINARY_WHITE))
#define MID_WEIGHT_FIRST_ROW                     15U

typedef struct
{
    uint8_t binary[VISION_RAW_H][VISION_RAW_W];
    uint8_t binary_zip[VISION_ZIP_IMAGE_H][VISION_ZIP_IMAGE_W];
    uint8_t left_line[VISION_ZIP_IMAGE_H];
    uint8_t right_line[VISION_ZIP_IMAGE_H];
    uint8_t mid_line_per_row[VISION_ZIP_IMAGE_H];
    uint8_t mid_line;
    uint8_t image_mid;
    int16_t calculate_error;
    uint8_t lost_count;
    uint8_t image_threshold;
    uint8_t element_cooldown;
    uint32_t version;
} vision_context_t;

static vision_context_t s_vision_ctx = {
    .image_threshold = 120U,
    .image_mid = IMAGE_MID_COL,
};


// s_mid_weight_list 行权重表：中远端（行 24-35）权重最高，两端衰减
// 设计思路：极远端（行 0-23）距离太远、数据不可靠；极近端（行 48-59）受车身抖动影响大；
//           中远端（行 24-35）既有前瞻性又相对稳定，主导循迹决策
// 权重分配（每段 12 行）：行 0-11 低(1)，行 12-23 低(1)，行 24-35 高(10)，
//                         行 36-47 中(5)，行 48-59 低(1)
// 注意：行号 0 是图像最远处，59 是最近处（车辆正前方）
static const uint8_t s_mid_weight_list[VISION_ZIP_IMAGE_H] = {
    // 行 0-11: 远端低权重 (1)
    1,1,1,1,1,1,1,1,1,1,1,1,
    // 行 12-23: 远端低权重 (1)
    1,1,1,1,1,1,1,1,1,1,1,1,
    // 行 24-35: 中远端高权重 (10) — 主导循迹决策
    10,10,10,10,10,10,10,10,10,10,10,10,
    // 行 36-47: 中近端中等权重 (5)
    5,5,5,5,5,5,5,5,5,5,5,5,
    // 行 48-59: 近端低权重 (1) — 抖动敏感区
    1,1,1,1,1,1,1,1,1,1,1,1
};

/**
 * @brief 灰度图像转二值图像
 *        遍历灰度图像，按阈值 value 划分为黑白两色
 * @param value 二值化阈值（通常由 OTSU 计算得到）
 * @note 灰度 < value → VISION_BINARY_BLACK（黑，赛道）；
 *       灰度 >= value → VISION_BINARY_WHITE（白，背景）。
 */
static void set_image_grayscale_to_binary(uint8_t value) //灰度转二值化
{
    uint8_t temp = 0;
    uint8_t (*cam_img)[VISION_RAW_W] = (uint8_t(*)[VISION_RAW_W])SmartcarHal_CameraData();
    for(uint8_t i = 0 ; i < VISION_RAW_H ; i++)
    {
        for(uint8_t j = 0 ; j < VISION_RAW_W ; j++)
        {
            temp = cam_img[i][j];//获取图像灰度值暂存至temp
            if(temp < value)
            {
                s_vision_ctx.binary[i][j] = VISION_BINARY_BLACK; //黑（赛道）
            }
            else
            {
                s_vision_ctx.binary[i][j] = VISION_BINARY_WHITE;//白（背景）
            }
        }
    }
}

/**
 * @brief 二值图像压缩
 *        隔行隔列采样：每 2 行取 1 行，每 2 列取 1 列
 *        减少后续滤波和边线检测的计算量
 */
static void zip_image(void) //图像压缩
{
    uint8_t n = 0; // 压缩后行号
    // 外层步长 2：每两行采样一行
    for (uint8_t i = 0; i < VISION_RAW_H; i += 2)
    {
        uint8_t m = 0; // 压缩后列号
        // 内层步长 2：每两列采样一列
        for (uint8_t j = 0; j < VISION_RAW_W; j += 2)
        {
            s_vision_ctx.binary_zip[n][m] = s_vision_ctx.binary[i][j];
            m++;
        }
        n++;
    }
}


/**
 * @brief 二值图像滤波（去孤立噪点）
 *        采用 4 邻域多数表决：
 *          - 黑点四周若 >= 2 个白点 → 转为白（去黑色孤立点）
 *          - 白点四周若 <  2 个白点 → 转为黑（去白色孤立点）
 *        阈值为 2 个白色像素值之和，即比较四邻域白色像素数与 2 的关系。
 * @note 仅处理内部像素（跳过第 0 行/列与最后一行/列），避免越界
 */
static void Bin_Image_Filter (void)//过滤噪点 （黑四周有2白 -》 白 ）
{
    int16_t nr; //行
    int16_t nc; //列

    // 跳过边缘 1 像素，确保 (nr±1, nc±1) 都在图像范围内
    for (nr = 1; nr < VISION_ZIP_IMAGE_H - 1; nr++)
    {
        for (nc = 1; nc < VISION_ZIP_IMAGE_W - 1; nc = nc + 1)
        {
            uint16_t neighbor_white_sum = (uint16_t)(s_vision_ctx.binary_zip[nr - 1][nc]
                + s_vision_ctx.binary_zip[nr + 1][nc]
                + s_vision_ctx.binary_zip[nr][nc + 1]
                + s_vision_ctx.binary_zip[nr][nc - 1]);

            // 当前为黑 且 上下左右白色像素 > 2 个 → 改为白
            if ((s_vision_ctx.binary_zip[nr][nc] == VISION_BINARY_BLACK) //如果当前色块为黑色
                && (neighbor_white_sum > VISION_BINARY_FILTER_WHITE_SUM))
            {
                s_vision_ctx.binary_zip[nr][nc] = VISION_BINARY_WHITE;
            }
            // 当前为白 且 上下左右白色像素 < 2 个 → 改为黑
            else if ((s_vision_ctx.binary_zip[nr][nc] == VISION_BINARY_WHITE)//如果当前色块为白色
                && (neighbor_white_sum < VISION_BINARY_FILTER_WHITE_SUM))
            {
                s_vision_ctx.binary_zip[nr][nc] = VISION_BINARY_BLACK;
            }
        }
    }
}



/**
 * @brief OTSU 大津法自适应阈值计算
 *        遍历 0-255 所有灰度级作为候选阈值，统计前景（黑）和背景（白）
 *        两类的像素数与平均灰度，找到使类间方差最大的阈值
 * @param p_image 灰度图像数据（一维数组形式，按行存储）
 * @param width 图像宽度（列数）
 * @param height 图像高度（行数）
 * @return 最佳二值化阈值（0-255）
 * @note 数学原理：阈值 T 使 σ²_between = wB·wF·(mB-mF)² 取最大值
 *       当前帧为 188x120、8 bit 灰度，float 精度足以区分候选阈值；
 *       TC264 单精度 FPU 可避免双精度软浮点开销。
 */
static uint8_t otsu(const uint8_t *p_image, uint16_t width, uint16_t height)
{
    uint32_t pixel_count[OTSU_GRAY_LEVELS] = {0U};
    uint32_t total = (uint32_t)width * (uint32_t)height;
    uint16_t i;
    uint16_t j;

    // 统计每个灰度级的像素个数
    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixel_count[p_image[((uint32_t)i * (uint32_t)width) + (uint32_t)j]]++;
        }
    }

    // 总像素的平均灰度
    float sum = 0.0f;
    for (i = 0; i < OTSU_GRAY_LEVELS; i++)
    {
        sum += (float)i * (float)pixel_count[i];
    }

    float sum_b = 0.0f;
    uint32_t w_b = 0U;
    uint32_t w_f = 0U;
    float max_var = 0.0f;
    uint8_t threshold = 0;
    uint8_t first_max = 0;
    uint8_t last_max = 0;
    int found_max = 0;

    for (i = 0; i < OTSU_GRAY_LEVELS; i++)
    {
        w_b += pixel_count[i]; // 前景像素数
        if (w_b == 0U)
        {
            continue;
        }

        w_f = total - w_b; // 背景像素数
        if (w_f == 0U)
        {
            break;
        }

        sum_b += (float)i * (float)pixel_count[i];

        float m_b = sum_b / (float)w_b; // 前景平均灰度
        float m_f = (sum - sum_b) / (float)w_f; // 背景平均灰度
        float mean_diff = m_b - m_f;

        // 类间方差
        float var_between = (float)w_b * (float)w_f * mean_diff * mean_diff;

        if (var_between > max_var)
        {
            max_var = var_between;
            first_max = (uint8_t)i;
            last_max = (uint8_t)i;
            found_max = 1;
        }
        else if ((var_between >= max_var) && (found_max != 0))
        {
            last_max = (uint8_t)i;
        }
    }

    // 对最大方差平台取中点，避免双峰边界不稳定
    if (found_max != 0)
    {
        threshold = (uint8_t)(((uint16_t)first_max + (uint16_t)last_max) >> 1);
    }

    return threshold;

}

/**
 * @brief 左右边线检测与中线计算
 *        从图像底部向顶部逐行扫描，每行以"上一行中线"为起点向两侧搜索边线
 *        左边界：黑→白跳变（赛道左边缘），右边界：白→黑跳变（赛道右边缘）
 *        中线  = (left + right) / 2，再做时间平滑与范围限制
 * @note 结果写入 s_vision_ctx.left_line / s_vision_ctx.right_line / s_vision_ctx.mid_line_per_row（长度 60）
 *       行号约定：row=59 为图像底部（车前），row=0 为图像顶部（远处）
 */
static void find_mid_line(void)
{
    // 初始化上一行的中线位置为图像宽度的一半（假设起始在中点）
    // VISION_ZIP_IMAGE_W/2 = 47，即图像中心列
    uint8_t last_mid   = VISION_ZIP_IMAGE_W / 2;
    // 初始化上一行的左边界为图像宽度的1/4位置（约 23 列）
    uint8_t last_left  = VISION_ZIP_IMAGE_W / 4;
    // 初始化上一行的右边界为图像宽度的3/4位置（即宽度减去1/4，约 70 列）
    uint8_t last_right = VISION_ZIP_IMAGE_W - VISION_ZIP_IMAGE_W / 4;

    // 从图像底部行开始向上遍历每一行（row从VISION_ZIP_IMAGE_H-1到0）
    // 这样可以利用上一行的信息作为参考，提高连续性
    for (int8_t row = VISION_ZIP_IMAGE_H - 1; row >= 0; --row)
    {
        // 将当前行的搜索起点设为上一行的中线位置
        uint8_t center = last_mid;
        // 确保center在有效范围内，避免越界（至少1，至少宽度-2）
        if (center < 1)                     center = 1;
        if (center > VISION_ZIP_IMAGE_W - 2)     center = VISION_ZIP_IMAGE_W - 2;

        // 初始化左边界为哨兵值，表示"未找到"。
        uint8_t left = VISION_EDGE_NOT_FOUND;
        // 从center向左搜索左边界：寻找从白到黑的过渡点
        // 这表示线条的左边缘（假设线条是黑色的，背景是白色的）
        for (int col = center; col > 0; --col)
        {
            // 检查当前像素为黑，左侧像素为白（左边界条件）
            if ((s_vision_ctx.binary_zip[row][col] == VISION_BINARY_BLACK) &&
                (s_vision_ctx.binary_zip[row][col - 1] == VISION_BINARY_WHITE))
            {
                left = (uint8_t)col;  // 记录左边界位置
                break;  // 找到后立即停止搜索
            }
        }
        // 如果没找到左边界，使用上一行的左边界值
        if (left == VISION_EDGE_NOT_FOUND)
        {
            left = last_left;
        }

        // 初始化右边界为哨兵值，表示"未找到"。
        uint8_t right = VISION_EDGE_NOT_FOUND;
        // 从center向右搜索右边界：寻找从黑到白的过渡点
        // 这表示线条的右边缘
        for (int col = center; col < VISION_ZIP_IMAGE_W - 1; ++col)
        {
            // 检查当前像素为黑，右侧像素为白（右边界条件）
            if ((s_vision_ctx.binary_zip[row][col] == VISION_BINARY_BLACK) &&
                (s_vision_ctx.binary_zip[row][col + 1] == VISION_BINARY_WHITE))
            {
                right = (uint8_t)col;  // 记录右边界位置
                break;  // 找到后立即停止搜索
            }
        }
        // 如果没找到右边界，使用上一行的右边界值
        if (right == VISION_EDGE_NOT_FOUND)
        {
            right = last_right;
        }

        // 如果右边界小于等于左边界，说明边界检测有问题，使用上一行的边界
        if (right <= left)
        {
            left  = last_left;
            right = last_right;
        }

        // 计算当前行的中线：左边界和右边界的平均值（右移1位相当于除以2）
        uint8_t mid = (left + right) >> 1;
        // 平滑处理：中线 = (当前计算的中线 * 2 + 上一行的中线) / 3
        // 这可以减少抖动，使中线变化更平滑
        mid = (uint8_t)((mid * 2 + last_mid) / 3);

        // 定义中线的有效范围：最小为宽度的1/10，最大为宽度-1/10
        // 避免中线过于靠近边缘
        const uint8_t min_mid = VISION_ZIP_IMAGE_W / 10;
        const uint8_t max_mid = VISION_ZIP_IMAGE_W - min_mid;
        if (mid < min_mid)   mid = min_mid;
        if (mid > max_mid)   mid = max_mid;

        // 将当前行的左边界、右边界和中线存储到全局列表中
        s_vision_ctx.left_line[row]  = left;
        s_vision_ctx.right_line[row] = right;
        s_vision_ctx.mid_line_per_row[row]   = mid;

        // 更新last变量，为下一行（上一行）做准备
        last_left  = left;
        last_right = right;
        last_mid   = mid;
    }
}

/**
 * @brief 加权中线计算与丢线检测
 *        对 s_vision_ctx.mid_line_per_row[] 按行权重 s_mid_weight_list[] 做加权平均，得到单值 s_vision_ctx.mid_line
 *        逐行比较相邻行中线差，若 >= LOST_MIDLINE_GAP 视为异常（丢线/弯道突变），停止累加并计入 s_vision_ctx.lost_count
 *        最终偏差 s_vision_ctx.calculate_error = s_vision_ctx.mid_line - s_vision_ctx.image_mid（PID 控制器输入）
 * @note 跳过前 MID_WEIGHT_FIRST_ROW 行（远处行噪点较多），实际处理到倒数第 2 行。
 *       s_vision_ctx.image_mid = 47 为参考中线，s_vision_ctx.calculate_error > 0 表示中线偏右
 */
static void find_mid_line_weight(void)//图像中值 与 补线中值差 然后 计算平均偏差数
{
    uint16_t weight_sum = 0; //权重和
    int16_t mid_line_error = 0;//相邻两次中值点 差
    uint8_t lost_line_flag = 0U;//丢线标志位
    uint16_t weight_mid_line_sum = 0;//权重*中值差 求和

    s_vision_ctx.lost_count = 0; // 初始化丢线计数

    // 跳过图像最远处的噪声行，处理到倒数第 2 行
    for(uint8_t i = MID_WEIGHT_FIRST_ROW ; i < VISION_ZIP_IMAGE_H - 1 ; i++)
    {
        // 相邻行中线差绝对值
        int _diff = (int)(s_vision_ctx.mid_line_per_row[i] - s_vision_ctx.mid_line_per_row[i+1]);
        if (_diff < 0)
        {
            mid_line_error = (int16_t)(-_diff);
        }
        else
        {
            mid_line_error = (int16_t)_diff;
        }
        if(lost_line_flag != 0U)
        {
            s_vision_ctx.lost_count++; // 增加丢线计数
            continue ; // 丢线 不计算偏差
        }
        // 相邻行中线差 >= LOST_MIDLINE_GAP：判定丢线（赛道突变或检测失败）
        else if(mid_line_error >= LOST_MIDLINE_GAP)
        {
            lost_line_flag = 1U; //中线跳变过大 不计算偏差
            s_vision_ctx.lost_count++; // 增加丢线计数
        }
        else
        {
            weight_mid_line_sum += (uint16_t)s_vision_ctx.mid_line_per_row[i] * s_mid_weight_list[i];  // 修正：加权中线，而不是偏差
            weight_sum += s_mid_weight_list[i];
            lost_line_flag = 0U;
        }
    }

    // 除零保护：所有行都丢线时，权重和为 0
    if(weight_sum == 0)
    {
        // 调试：打印或设置默认值
        // printf("Warning: weight_sum is 0, possible lost line issue\n");
        s_vision_ctx.mid_line = s_vision_ctx.image_mid; // 回退到图像中点（偏差为 0，直行）
    }
    else
    {
        s_vision_ctx.mid_line = weight_mid_line_sum / weight_sum;
    }

    // 计算最终偏差：> 0 中线偏右（车偏左），< 0 中线偏左（车偏右）
    s_vision_ctx.calculate_error = (int16_t)s_vision_ctx.mid_line - s_vision_ctx.image_mid;
}

/**
 * @brief 视觉处理总入口（每帧调用一次，通常在摄像头中断或主循环中触发）
 *
 * 处理流水线（顺序执行）：
 *   1. OTSU 计算自适应阈值（基于原始灰度图）
 *   2. 按阈值二值化（写入 s_vision_ctx.binary）
 *   3. 图像压缩（写入 s_vision_ctx.binary_zip）
 *   4. 二值图像滤波（去孤立噪点）
 *   5. 边线检测与中线计算（写入 left/right/s_vision_ctx.mid_line_per_row）
 *   6. 加权中线 + 丢线检测（输出 s_vision_ctx.calculate_error、s_vision_ctx.lost_count）
 *
 * @note 调用后控制器读取 s_vision_ctx.calculate_error（PID 输入）和 s_vision_ctx.lost_count（停止判断）
 */
void Vision_Process(void)
{
    smartcar_hal_camera_desc_t frame_desc = {0};

    // 步骤 1：OTSU 自适应阈值，图像尺寸由帧描述提供
    SmartcarHal_CameraGetDesc(&frame_desc);
    if ((frame_desc.width == 0U) || (frame_desc.height == 0U))
    {
        frame_desc.width = VISION_RAW_W;
        frame_desc.height = VISION_RAW_H;
    }

    s_vision_ctx.image_threshold = otsu(SmartcarHal_CameraData(), frame_desc.width, frame_desc.height);
    // 步骤 2：按阈值二值化
    set_image_grayscale_to_binary(s_vision_ctx.image_threshold);
    // 步骤 3：隔行隔列压缩到内部视觉工作尺寸
    zip_image();
    // 步骤 4：4 邻域滤波去噪
    Bin_Image_Filter();
    // 步骤 5：从底向顶逐行搜索左右边线，计算每行中线
    find_mid_line();
    // 步骤 6：行加权平均得到最终中线 s_vision_ctx.mid_line 与偏差 s_vision_ctx.calculate_error
    find_mid_line_weight();
    /* Bump after all frame outputs are refreshed; snapshots copy this stamp. */
    s_vision_ctx.version++;
}

/**
 * @brief 查询摄像头帧是否就绪。
 *
 * 处理步骤：
 *  1. 通过平台端口读取摄像头 ready 状态。
 *  2. 向 App 层隐藏 SmartcarHal_CameraReady 硬件契约。
 *
 * @return 1: 有新帧  0: 无新帧。
 *
 */
uint8_t Vision_IsFrameReady(void)
{
    uint8_t is_ready = 0U;

    if (SmartcarHal_CameraReady())
    {
        is_ready = 1U;
    }

    return is_ready;
}

/**
 * @brief 标记当前摄像头帧已消费。
 *
 * 处理步骤：
 *  1. 调用平台端口清除摄像头帧 ready 标志。
 *  2. 将帧生命周期收敛在视觉服务内。
 *
 * @return void。
 *
 */
void Vision_MarkFrameConsumed(void)
{
    SmartcarHal_CameraClear();
}

/**
 * @brief 读取控制层所需的完整帧快照。
 *
 * 处理步骤：
 *  1. 将视觉算法当前帧输出复制到调用方结构体。
 *  2. 调用方只依赖快照，不直接访问视觉内部全局状态。
 *
 * @param[out] p_snapshot : 控制层视觉快照。
 *
 * @return void : 无返回值。
 *
 */
void Vision_GetControlSnapshot(vision_control_snapshot_t *p_snapshot)
{
    if (p_snapshot == 0)
    {
        return;
    }

    p_snapshot->calculate_error = s_vision_ctx.calculate_error;
    p_snapshot->lost_count = s_vision_ctx.lost_count;
    p_snapshot->mid_line = s_vision_ctx.mid_line;
    p_snapshot->image_mid = s_vision_ctx.image_mid;
    p_snapshot->image_threshold = s_vision_ctx.image_threshold;
    p_snapshot->version = s_vision_ctx.version;
}

/**
 * @brief 读取调试显示所需的视觉快照。
 *
 * 处理步骤：
 *  1. 返回只读图像缓冲和边线数组指针。
 *  2. 同步携带尺寸和当前误差，避免调试层依赖视觉内部变量名。
 *
 * @param[out] p_snapshot : 调试显示视觉快照。
 *
 * @return void : 无返回值。
 *
 */
void Vision_GetDebugSnapshot(vision_debug_snapshot_t *p_snapshot)
{
    if (p_snapshot == 0)
    {
        return;
    }

    p_snapshot->p_binary_zip = s_vision_ctx.binary_zip;
    p_snapshot->p_left_line = s_vision_ctx.left_line;
    p_snapshot->p_right_line = s_vision_ctx.right_line;
    p_snapshot->p_mid_line = s_vision_ctx.mid_line_per_row;
    p_snapshot->image_width = VISION_ZIP_IMAGE_W;
    p_snapshot->image_height = VISION_ZIP_IMAGE_H;
    p_snapshot->line_count = VISION_ZIP_IMAGE_H;
    p_snapshot->calculate_error = s_vision_ctx.calculate_error;
    p_snapshot->version = s_vision_ctx.version;
}

/* ===== 特殊元素检测 ===== */

/**
 * @brief 检测特殊赛道元素（圆环 / 十字路口）
 *
 * 启发式策略（基于压缩图像边线数据 s_vision_ctx.left_line / s_vision_ctx.right_line）：
 *
 *   十字路口：检测窗口内双侧边线同时贴边的行数 >= ELEM_CROSSROAD_ROWS
 *            （左右两边线都到达图像边缘，说明赛道横向完全打开）
 *
 *   圆环：检测窗口内单侧边线贴边的行数 >= ELEM_RING_ROWS
 *         （一侧边线大量丢失，但另一侧正常，是圆环入口的典型特征）
 *
 * 判定优先级：先十字路口（更严格），再圆环（更宽松）
 *
 * @return 视觉赛道元素类型。
 */
vision_track_element_t Vision_DetectElement(void)
{
    uint8_t i;
    uint8_t left_lost  = 0;
    uint8_t right_lost = 0;
    uint8_t both_lost  = 0;

    /* 冷却期内不检测，防止单元素持续触发蜂鸣器 */
    if (s_vision_ctx.element_cooldown > 0)
    {
        s_vision_ctx.element_cooldown--;
        return VISION_TRACK_ELEMENT_NONE;
    }

    /* 扫描检测窗口行，统计各方向丢线行数 */
    for (i = ELEM_ROW_START; i < ELEM_ROW_END; i++)
    {
        uint8_t left_at_edge  = (s_vision_ctx.left_line[i]  <= ELEM_EDGE_LEFT);
        uint8_t right_at_edge = (s_vision_ctx.right_line[i] >= ELEM_EDGE_RIGHT);

        if (left_at_edge)
        {
            left_lost++;
        }
        if (right_at_edge)
        {
            right_lost++;
        }
        if (left_at_edge && right_at_edge)
        {
            both_lost++;
        }
    }

    /* 判定优先级：先十字（双侧丢线），再圆环（单侧丢线） */
    if (both_lost >= ELEM_CROSSROAD_ROWS)
    {
        s_vision_ctx.element_cooldown = ELEM_COOLDOWN_FRAMES;
        return VISION_TRACK_ELEMENT_CROSSROAD;
    }

    if (left_lost >= ELEM_RING_ROWS || right_lost >= ELEM_RING_ROWS)
    {
        s_vision_ctx.element_cooldown = ELEM_COOLDOWN_FRAMES;
        return VISION_TRACK_ELEMENT_RING;
    }

    return VISION_TRACK_ELEMENT_NONE;
}
