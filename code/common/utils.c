/*
 * utils.c
 *
 *  Created on: 2025年10月21日
 *      Author: Paracosm
 *
 *  通用工具函数实现文件
 *  存放项目内部使用的辅助函数，例如绝对值计算等。
 */
#include "utils.h"

/**
 * @brief 求 16 位有符号整数的绝对值
 * @param value 输入值（可为正、负或零）
 * @return value 的绝对值（非负）
 *
 * 使用自定义版本而非标准库 abs()，原因：
 *  - 精确控制输入类型为 int16_t，避免类型转换开销
 *  - 在 TC264 上手写实现比库调用更快
 *  - 已修复历史 bug：value == 0 时返回 0，避免返回带符号的负零
 */
int16_t my_abs(int16_t value)
{
    if(value > 0)
    {
        return value;
    }
    else if(value < 0)
    {
        return (int16_t)(-value);
    }
    else
    {
        return 0;
    }

}
