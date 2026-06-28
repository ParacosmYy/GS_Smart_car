/*
 * image.c
 *
 *  Created on: 2025年10月20日
 *      Author: Paracosm
 */

#include "image.h"

#define GrayScale 256//定义256个灰度级
#define COL 120
#define ROW 188

#define zip_MT9V03X_H 60
#define zip_MT9V03X_W 94

#define FILTER_LEN 20
#define WHITE_COL 93

////////////////////大津法二值化//////////////////////
uint8_t image_threshold = 120;  //图像阈值

uint8_t  mt9v03x_image_bandw[MT9V03X_H][MT9V03X_W] = {{0}}; //将灰度值数组转为二值化数组
uint8_t  mt9v03x_image_bandw_zip[60][94] = {{0}}; //将二值化数组压缩成一半


uint8_t left_line_list[zip_MT9V03X_H] = {0} ;//记录左三个边界点
uint8_t right_line_list[zip_MT9V03X_H] = {0};//记录右三个边界点
uint8_t mid_line_list[zip_MT9V03X_H] = {0}; //记录中点

uint8_t mid_line = 0; //最后的加权平均中值
uint8_t image_mid = 47;//图像中点
int16_t calculate_error = 0; //加权平均中线 与 图像中线差值


int8_t left_line_lost_flag = 0; //0 表示未丢线 ；1 表示丢线
int8_t right_line_lost_flag = 0;
uint8_t lost_count = 0; // 丢线计数


// mid_weight_list 数组用于循迹或图像处理时的加权计算
// 根据你的要求，从59行到0行权重配比为：低、中、高、低、低
// 低权重设为1，中权重设为5，高权重设为10
// 每段12行：0-11低，12-23低，24-35高，36-47中，48-59低

uint8_t mid_weight_list[60] = {
    // 0-11: 低 (1)
    1,1,1,1,1,1,1,1,1,1,1,1,
    // 12-23: 低 (1)
    1,1,1,1,1,1,1,1,1,1,1,1,
    // 24-35: 高 (10)
    10,10,10,10,10,10,10,10,10,10,10,10,
    // 36-47: 中 (5)
    5,5,5,5,5,5,5,5,5,5,5,5,
    // 48-59: 低 (1)
    1,1,1,1,1,1,1,1,1,1,1,1
};
void set_image_grayscale_to_binary(uint8 value) //灰度转二值化
{
    uint8_t temp = 0;
    for(uint8_t i = 0 ; i < MT9V03X_H ; i++)
    {
        for(uint8_t j = 0 ; j < MT9V03X_W ; j++)
        {
            temp = mt9v03x_image[i][j];//获取图像灰度值暂存至temp
            if(temp < value)
            {
                mt9v03x_image_bandw[i][j] = 0 ; //黑
            }
            else
            {
                mt9v03x_image_bandw[i][j] = 255 ;//白
            }
        }
    }
}

void zip_image(void) //图像压缩
{
    uint8_t n = 0; // 行
    for (uint8_t i = 0; i < MT9V03X_H; i += 2)
    {
        uint8_t m = 0; // 列
        for (uint8_t j = 0; j < MT9V03X_W; j += 2)
        {
            mt9v03x_image_bandw_zip[n][m] = mt9v03x_image_bandw[i][j];
            m++;
        }
        n++;
    }
}


void Bin_Image_Filter (void)//过滤噪点 （黑四周有2白 -》 白 ）
{
    sint16 nr; //行
    sint16 nc; //列

    for (nr = 1; nr < zip_MT9V03X_H - 1; nr++)
    {
        for (nc = 1; nc < zip_MT9V03X_W - 1; nc = nc + 1)
        {
            if ((mt9v03x_image_bandw_zip[nr][nc] == 0) //如果当前色块为黑色
                && (mt9v03x_image_bandw_zip[nr - 1][nc] + mt9v03x_image_bandw_zip[nr + 1][nc] + mt9v03x_image_bandw_zip[nr][nc + 1] + mt9v03x_image_bandw_zip[nr][nc - 1] > 2 * 255))
            {
                mt9v03x_image_bandw_zip[nr][nc] = 255;
            }
            else if ((mt9v03x_image_bandw_zip[nr][nc] == 255)//如果当前色块为白色
                && (mt9v03x_image_bandw_zip[nr - 1][nc] + mt9v03x_image_bandw_zip[nr + 1][nc] + mt9v03x_image_bandw_zip[nr][nc + 1] + mt9v03x_image_bandw_zip[nr][nc - 1] < 2 * 255))
            {
                mt9v03x_image_bandw_zip[nr][nc] = 0;
            }
        }
    }
}



uint8_t otsu(uint8_t *image, uint16 width, uint16 height)
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

void find_mid_line(void)
{
    // 初始化上一行的中线位置为图像宽度的一半（假设起始在中点）
    uint8_t last_mid   = zip_MT9V03X_W / 2;
    // 初始化上一行的左边界为图像宽度的1/4位置
    uint8_t last_left  = zip_MT9V03X_W / 4;
    // 初始化上一行的右边界为图像宽度的3/4位置（即宽度减去1/4）
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

        // 初始化左边界为0xFF（表示未找到）
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

        // 初始化右边界为0xFF（表示未找到）
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

void find_mid_line_weight(void)//图像中值 与 补线中值差 然后 计算平均偏差数
{
    uint16_t weight_sum = 0; //权重和
    int16 mid_line_error = 0;//相邻两次中值点 差
    uint8_t lost_line_flag = 0;//丢线标志位
    uint16_t weight_mid_line_sum = 0;//权重*中值差 求和

    lost_count = 0; // 初始化丢线计数

    // 修改循环：跳过前15行，只计算从第15行到最后一行（i从15到zip_MT9V03X_H-2）
    for(uint8_t i = 15 ; i < zip_MT9V03X_H - 1 ; i++)
    {
        mid_line_error = my_abs((int)(mid_line_list[i] - mid_line_list[i+1]));
        if(lost_line_flag == 1 )
        {
            lost_count++; // 增加丢线计数
            continue ; // 丢线 不计算偏差
        }
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

    // 添加除零检查
    if(weight_sum == 0)
    {
        // 调试：打印或设置默认值
        // printf("Warning: weight_sum is 0, possible lost line issue\n");
        mid_line = image_mid; // 设置为默认中点
    }
    else
    {
        mid_line = weight_mid_line_sum / weight_sum;
    }

    calculate_error = (int16_t)mid_line - image_mid;
}

static void vision_process(void)
{
    image_threshold = otsu(mt9v03x_image[0], 188, 120);
    set_image_grayscale_to_binary(image_threshold);
    zip_image();
    Bin_Image_Filter();
    find_mid_line();
    find_mid_line_weight();
    draw_all_lines_test();
}

static void control_update(void)
{
    if(lost_count > 10)
    {
        return;
    }
    servo_pid_output = servo_pid_contorl(&servo_pid, 0, calculate_error);
    left_motor_pid_output = motor_pid_control(&left_motor_pid, 0, left_encoder_speed);
    right_motor_pid_output = motor_pid_control(&right_motor_pid, 0, right_encoder_speed);
}

static void actuator_apply(void)
{
    if(lost_count > 10)
    {
        pwm_set_duty(ATOM1_CH1_P33_9, 678);
        return;
    }
    motor_setspeed_left((int32)left_motor_pid_output);
    motor_setspeed_right((int32)right_motor_pid_output);
}

void task_calculte(void)
{
    vision_process();
    control_update();
    actuator_apply();
    mt9v03x_finish_flag = 0;
}
