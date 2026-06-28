/*
 * vision.c
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 *
 * @brief Vision service implementation.
 *
 * Processing flow:
 * Raw grayscale frame → OTSU threshold → binary image → downsampled image
 * → noise filtering → track edge detection → weighted midline → PID error.
 * The raw MT9V03X frame is 188 x 120 and the internal compressed frame is
 * 94 x 60.
 */

#include "vision.h"
#include "config.h"     /* ELEM_* 元素检测参数 */

#define GrayScale 256    // OTSU 使用的灰度级数量（0-255）
#define COL 120          // 原始图像行数（保留备用，实际用 MT9V03X_H）
#define ROW 188          // 原始图像列数（保留备用，实际用 MT9V03X_W）

#define zip_MT9V03X_H 60 // 压缩后图像高度（原始 120 的一半）
#define zip_MT9V03X_W 94 // 压缩后图像宽度（原始 188 的一半）

#define FILTER_LEN 20    // 滤波相关参数（保留备用）
#define WHITE_COL 93     // 压缩图像最右列索引（94 列，索引 0-93）

////////////////////大津法二值化//////////////////////
uint8_t image_threshold = 120;  // 二值化阈值（初值，运行时被 OTSU 覆盖）

// 二值化后的图像缓冲（0=黑，255=白），原始尺寸 188×120
uint8_t  mt9v03x_image_bandw[PAL_CAM_H][PAL_CAM_W] = {{0}};
// 二值化后压缩到一半的图像（60×94），后续算法均基于此
uint8_t  mt9v03x_image_bandw_zip[60][94] = {{0}};


// 边线与中线数据，按行存储（索引 0=图像顶部，59=图像底部）
uint8_t left_line_list[zip_MT9V03X_H] = {0} ; // 每行的左边界列坐标
uint8_t right_line_list[zip_MT9V03X_H] = {0}; // 每行的右边界列坐标
uint8_t mid_line_list[zip_MT9V03X_H] = {0};   // 每行的中线列坐标（左右边界均值）

uint8_t mid_line = 0;       // 最终加权平均的中线位置（单值，所有行汇总）
uint8_t image_mid = 47;     // 图像中心列（94/2 - 1 = 46，这里取 47 作为参考中线）
int16_t calculate_error = 0;// 中线偏差 = mid_line - image_mid，作为 PID 控制器输入


int8_t left_line_lost_flag = 0;  // 左边线丢线标志（0=未丢线，1=丢线）
int8_t right_line_lost_flag = 0; // 右边线丢线标志
uint8_t lost_count = 0;          // 连续丢线计数，超过阈值时停止控制输出


// mid_weight_list 行权重表：中远端（行 24-35）权重最高，两端衰减
// 设计思路：极远端（行 0-23）距离太远、数据不可靠；极近端（行 48-59）受车身抖动影响大；
//           中远端（行 24-35）既有前瞻性又相对稳定，主导循迹决策
// 权重分配（每段 12 行）：行 0-11 低(1)，行 12-23 低(1)，行 24-35 高(10)，
//                         行 36-47 中(5)，行 48-59 低(1)
// 注意：行号 0 是图像最远处，59 是最近处（车辆正前方）
uint8_t mid_weight_list[60] = {
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
 *        遍历原始 188×120 灰度图像，按阈值 value 划分为黑白两色
 * @param value 二值化阈值（通常由 OTSU 计算得到）
 * @note 灰度 < value → 0（黑，赛道）；灰度 >= value → 255（白，背景）
 */
void set_image_grayscale_to_binary(uint8_t value) //灰度转二值化
{
    uint8_t temp = 0;
    uint8_t (*cam_img)[PAL_CAM_W] = (uint8_t(*)[PAL_CAM_W])pal_cam_data();
    for(uint8_t i = 0 ; i < PAL_CAM_H ; i++)
    {
        for(uint8_t j = 0 ; j < PAL_CAM_W ; j++)
        {
            temp = cam_img[i][j];//获取图像灰度值暂存至temp
            if(temp < value)
            {
                mt9v03x_image_bandw[i][j] = 0 ; //黑（赛道）
            }
            else
            {
                mt9v03x_image_bandw[i][j] = 255 ;//白（背景）
            }
        }
    }
}

/**
 * @brief 二值图像压缩（188×120 → 94×60）
 *        隔行隔列采样：每 2 行取 1 行，每 2 列取 1 列
 *        减少后续滤波和边线检测的计算量
 */
void zip_image(void) //图像压缩
{
    uint8_t n = 0; // 压缩后行号
    // 外层步长 2：每两行采样一行
    for (uint8_t i = 0; i < PAL_CAM_H; i += 2)
    {
        uint8_t m = 0; // 压缩后列号
        // 内层步长 2：每两列采样一列
        for (uint8_t j = 0; j < PAL_CAM_W; j += 2)
        {
            mt9v03x_image_bandw_zip[n][m] = mt9v03x_image_bandw[i][j];
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
 *        阈值 2*255 = 510，即比较四邻域白色像素数与 2 的关系
 * @note 仅处理内部像素（跳过第 0 行/列与最后一行/列），避免越界
 */
void Bin_Image_Filter (void)//过滤噪点 （黑四周有2白 -》 白 ）
{
    int16_t nr; //行
    int16_t nc; //列

    // 跳过边缘 1 像素，确保 (nr±1, nc±1) 都在图像范围内
    for (nr = 1; nr < zip_MT9V03X_H - 1; nr++)
    {
        for (nc = 1; nc < zip_MT9V03X_W - 1; nc = nc + 1)
        {
            // 当前为黑 且 上下左右白色像素 > 2 个 → 改为白
            if ((mt9v03x_image_bandw_zip[nr][nc] == 0) //如果当前色块为黑色
                && (mt9v03x_image_bandw_zip[nr - 1][nc] + mt9v03x_image_bandw_zip[nr + 1][nc] + mt9v03x_image_bandw_zip[nr][nc + 1] + mt9v03x_image_bandw_zip[nr][nc - 1] > 2 * 255))
            {
                mt9v03x_image_bandw_zip[nr][nc] = 255;
            }
            // 当前为白 且 上下左右白色像素 < 2 个 → 改为黑
            else if ((mt9v03x_image_bandw_zip[nr][nc] == 255)//如果当前色块为白色
                && (mt9v03x_image_bandw_zip[nr - 1][nc] + mt9v03x_image_bandw_zip[nr + 1][nc] + mt9v03x_image_bandw_zip[nr][nc + 1] + mt9v03x_image_bandw_zip[nr][nc - 1] < 2 * 255))
            {
                mt9v03x_image_bandw_zip[nr][nc] = 0;
            }
        }
    }
}



/**
 * @brief OTSU 大津法自适应阈值计算
 *        遍历 0-255 所有灰度级作为候选阈值，统计前景（黑）和背景（白）
 *        两类的像素数与平均灰度，找到使类间方差最大的阈值
 * @param image 灰度图像数据（一维数组形式，按行存储）
 * @param width 图像宽度（列数）
 * @param height 图像高度（行数）
 * @return 最佳二值化阈值（0-255）
 * @note 数学原理：阈值 T 使 σ²_between = wB·wF·(mB-mF)² 取最大值
 */
uint8_t otsu(uint8_t *image, uint16_t width, uint16_t height)
{
    int pixelCount[GrayScale] = {0};
    int total = width * height;
    int i, j;

    // 统计每个灰度级的像素个数
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            pixelCount[image[i * width + j]]++;

    // 总像素的平均灰度
    double sum = 0;
    for (i = 0; i < GrayScale; i++)
        sum += i * pixelCount[i];

    double sumB = 0;
    int wB = 0;
    int wF = 0;
    double maxVar = 0;
    uint8_t threshold = 0;

    for (i = 0; i < GrayScale; i++)
    {
        wB += pixelCount[i]; // 前景像素数
        if (wB == 0) continue;

        wF = total - wB; // 背景像素数
        if (wF == 0) break;

        sumB += i * pixelCount[i];

        double mB = sumB / wB; // 前景平均灰度
        double mF = (sum - sumB) / wF; // 背景平均灰度

        // 类间方差
        double varBetween = (double)wB * (double)wF * (mB - mF) * (mB - mF);

        if (varBetween > maxVar)
        {
            maxVar = varBetween;
            threshold = i;
        }
    }
    return threshold;

}

/**
 * @brief 左右边线检测与中线计算
 *        从图像底部向顶部逐行扫描，每行以"上一行中线"为起点向两侧搜索边线
 *        左边界：黑→白跳变（赛道左边缘），右边界：白→黑跳变（赛道右边缘）
 *        中线  = (left + right) / 2，再做时间平滑与范围限制
 * @note 结果写入 left_line_list / right_line_list / mid_line_list（长度 60）
 *       行号约定：row=59 为图像底部（车前），row=0 为图像顶部（远处）
 */
void find_mid_line(void)
{
    // 初始化上一行的中线位置为图像宽度的一半（假设起始在中点）
    // zip_MT9V03X_W/2 = 47，即图像中心列
    uint8_t last_mid   = zip_MT9V03X_W / 2;
    // 初始化上一行的左边界为图像宽度的1/4位置（约 23 列）
    uint8_t last_left  = zip_MT9V03X_W / 4;
    // 初始化上一行的右边界为图像宽度的3/4位置（即宽度减去1/4，约 70 列）
    uint8_t last_right = zip_MT9V03X_W - zip_MT9V03X_W / 4;

    // 从图像底部行开始向上遍历每一行（row从zip_MT9V03X_H-1到0）
    // 这样可以利用上一行的信息作为参考，提高连续性
    for (int8_t row = zip_MT9V03X_H - 1; row >= 0; --row)
    {
        // 将当前行的搜索起点设为上一行的中线位置
        uint8_t center = last_mid;
        // 确保center在有效范围内，避免越界（至少1，至少宽度-2）
        if (center < 1)                     center = 1;
        if (center > zip_MT9V03X_W - 2)     center = zip_MT9V03X_W - 2;

        // 初始化左边界为0xFF（哨兵值，表示"未找到"。0xFF=255 远超有效列号 0-93）
        uint8_t left = 0xFF;
        // 从center向左搜索左边界：寻找从黑(0)到白(255)的过渡点
        // 这表示线条的左边缘（假设线条是黑色的，背景是白色的）
        for (int col = center; col > 0; --col)
        {
            // 检查当前像素为黑，下一个像素为白（左边界条件）
            if ((mt9v03x_image_bandw_zip[row][col] == 0) &&
                (mt9v03x_image_bandw_zip[row][col + 1] == 255))
            {
                left = (uint8_t)col;  // 记录左边界位置
                break;  // 找到后立即停止搜索
            }
        }
        // 如果没找到左边界，使用上一行的左边界值
        if (left == 0xFF)   left = last_left;

        // 初始化右边界为0xFF（哨兵值，表示"未找到"）
        uint8_t right = 0xFF;
        // 从center向右搜索右边界：寻找从白(255)到黑(0)的过渡点
        // 这表示线条的右边缘
        for (int col = center; col < zip_MT9V03X_W - 1; ++col)
        {
            // 检查当前像素为白，下一个像素为黑（右边界条件）
            if ((mt9v03x_image_bandw_zip[row][col] == 255) &&
                (mt9v03x_image_bandw_zip[row][col + 1] == 0))
            {
                right = (uint8_t)col;  // 记录右边界位置
                break;  // 找到后立即停止搜索
            }
        }
        // 如果没找到右边界，使用上一行的右边界值
        if (right == 0xFF)  right = last_right;

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
        const uint8_t min_mid = zip_MT9V03X_W / 10;
        const uint8_t max_mid = zip_MT9V03X_W - min_mid;
        if (mid < min_mid)   mid = min_mid;
        if (mid > max_mid)   mid = max_mid;

        // 将当前行的左边界、右边界和中线存储到全局列表中
        left_line_list[row]  = left;
        right_line_list[row] = right;
        mid_line_list[row]   = mid;

        // 更新last变量，为下一行（上一行）做准备
        last_left  = left;
        last_right = right;
        last_mid   = mid;
    }
}

/**
 * @brief 加权中线计算与丢线检测
 *        对 mid_line_list[] 按行权重 mid_weight_list[] 做加权平均，得到单值 mid_line
 *        逐行比较相邻行中线差，若 >= 40 视为异常（丢线/弯道突变），停止累加并计入 lost_count
 *        最终偏差 calculate_error = mid_line - image_mid（PID 控制器输入）
 * @note 跳过前 15 行（远处行噪点较多，不参与计算），实际处理行 15 到 58
 *       image_mid = 47 为参考中线，calculate_error > 0 表示中线偏右
 */
void find_mid_line_weight(void)//图像中值 与 补线中值差 然后 计算平均偏差数
{
    uint16_t weight_sum = 0; //权重和
    int16_t mid_line_error = 0;//相邻两次中值点 差
    uint8_t lost_line_flag = 0;//丢线标志位
    uint16_t weight_mid_line_sum = 0;//权重*中值差 求和

    lost_count = 0; // 初始化丢线计数

    // 跳过前 15 行（图像最远处，数据噪声大），从第 15 行处理到倒数第 2 行
    for(uint8_t i = 15 ; i < zip_MT9V03X_H - 1 ; i++)
    {
        // 相邻行中线差绝对值（替代原逐飞 my_abs）
        int _diff = (int)(mid_line_list[i] - mid_line_list[i+1]);
        if (_diff < 0)
        {
            mid_line_error = (int16_t)(-_diff);
        }
        else
        {
            mid_line_error = (int16_t)_diff;
        }
        if(lost_line_flag == 1 )
        {
            lost_count++; // 增加丢线计数
            continue ; // 丢线 不计算偏差
        }
        // 相邻行中线差 >= 40：判定丢线（赛道突变或检测失败）
        else if(lost_line_flag == 0 && mid_line_error >= 40)
        {
            lost_line_flag = 1; //中线宽超40误差 不计算偏差
            lost_count++; // 增加丢线计数
        }
        else
        {
            weight_mid_line_sum += (uint16_t)mid_line_list[i] * mid_weight_list[i];  // 修正：加权中线，而不是偏差
            weight_sum += mid_weight_list[i];
            lost_line_flag = 0;
        }
    }

    // 除零保护：所有行都丢线时，权重和为 0
    if(weight_sum == 0)
    {
        // 调试：打印或设置默认值
        // printf("Warning: weight_sum is 0, possible lost line issue\n");
        mid_line = image_mid; // 回退到图像中点（偏差为 0，直行）
    }
    else
    {
        mid_line = weight_mid_line_sum / weight_sum;
    }

    // 计算最终偏差：> 0 中线偏右（车偏左），< 0 中线偏左（车偏右）
    calculate_error = (int16_t)mid_line - image_mid;
}

/**
 * @brief 视觉处理总入口（每帧调用一次，通常在摄像头中断或主循环中触发）
 *
 * 处理流水线（顺序执行）：
 *   1. OTSU 计算自适应阈值（基于原始 188×120 灰度图）
 *   2. 按阈值二值化（写入 mt9v03x_image_bandw）
 *   3. 图像压缩（188×120 → 94×60，写入 mt9v03x_image_bandw_zip）
 *   4. 二值图像滤波（去孤立噪点）
 *   5. 边线检测与中线计算（写入 left/right/mid_line_list）
 *   6. 加权中线 + 丢线检测（输出 calculate_error、lost_count）
 *
 * @note 调用后控制器读取 calculate_error（PID 输入）和 lost_count（停止判断）
 */
void Vision_Process(void)
{
    // 步骤 1：OTSU 自适应阈值，传入 PAL 摄像头原始灰度图（PAL_CAM_W × PAL_CAM_H）
    image_threshold = otsu(pal_cam_data(), PAL_CAM_W, PAL_CAM_H);
    // 步骤 2：按阈值二值化
    set_image_grayscale_to_binary(image_threshold);
    // 步骤 3：隔行隔列压缩到 94×60
    zip_image();
    // 步骤 4：4 邻域滤波去噪
    Bin_Image_Filter();
    // 步骤 5：从底向顶逐行搜索左右边线，计算每行中线
    find_mid_line();
    // 步骤 6：行加权平均得到最终中线 mid_line 与偏差 calculate_error
    find_mid_line_weight();
}

/**
 * @brief 查询摄像头是否已有新帧。
 *
 * 处理步骤：
 *  1. 通过 PAL 查询摄像头帧完成标志。
 *  2. 将 bool 结果收敛成模块对外的 uint8_t。
 *
 * @return uint8_t : 1 表示有新帧，0 表示无新帧。
 *
 * */
uint8_t Vision_IsFrameReady(void)
{
    uint8_t is_ready = 0U;

    if (pal_cam_ready())
    {
        is_ready = 1U;
    }

    return is_ready;
}

/**
 * @brief 清除摄像头帧就绪标志。
 *
 * 处理步骤：
 *  1. 委托 PAL 清除摄像头帧完成标志。
 *
 * @return void : 无返回值。
 *
 * */
void Vision_ClearFrameReady(void)
{
    pal_cam_clear();
}

/* ===== 特殊元素检测 ===== */

/**
 * @brief 检测冷却计数器（帧数），每帧递减
 *        检测到元素后置为 ELEM_COOLDOWN_FRAMES，期间不再触发
 */
static uint8_t s_element_cooldown = 0;

/**
 * @brief 检测特殊赛道元素（圆环 / 十字路口）
 *
 * 启发式策略（基于压缩图像边线数据 left_line_list / right_line_list）：
 *
 *   十字路口：检测窗口内双侧边线同时贴边的行数 >= ELEM_CROSSROAD_ROWS
 *            （左右两边线都到达图像边缘，说明赛道横向完全打开）
 *
 *   圆环：检测窗口内单侧边线贴边的行数 >= ELEM_RING_ROWS
 *         （一侧边线大量丢失，但另一侧正常，是圆环入口的典型特征）
 *
 * 判定优先级：先十字路口（更严格），再圆环（更宽松）
 *
 * @return 0=无特殊元素, 1=圆环, 2=十字路口
 */
uint8_t Vision_DetectElement(void)
{
    uint8_t i;
    uint8_t left_lost  = 0;
    uint8_t right_lost = 0;
    uint8_t both_lost  = 0;

    /* 冷却期内不检测，防止单元素持续触发蜂鸣器 */
    if (s_element_cooldown > 0)
    {
        s_element_cooldown--;
        return 0;
    }

    /* 扫描检测窗口行，统计各方向丢线行数 */
    for (i = ELEM_ROW_START; i < ELEM_ROW_END; i++)
    {
        uint8_t left_at_edge  = (left_line_list[i]  <= ELEM_EDGE_LEFT);
        uint8_t right_at_edge = (right_line_list[i] >= ELEM_EDGE_RIGHT);

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
        s_element_cooldown = ELEM_COOLDOWN_FRAMES;
        return 2;  /* 十字路口 */
    }

    if (left_lost >= ELEM_RING_ROWS || right_lost >= ELEM_RING_ROWS)
    {
        s_element_cooldown = ELEM_COOLDOWN_FRAMES;
        return 1;  /* 圆环 */
    }

    return 0;
}
