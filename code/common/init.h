/*
 * init.h
 *
 *  Created on: 2025年10月19日
 *      Author: Paracosm
 *
 *  系统初始化接口声明
 */

#ifndef CODE_INIT_H_
#define CODE_INIT_H_

/**
 * @brief 初始化全部外设与 PID 控制器
 *
 * 按固定顺序初始化：串口 → 编码器 → 电机/舵机 →
 * 显示/摄像头 → 按键/蜂鸣器 → 陀螺仪/无线串口 → PID 控制器。
 * 应在主任务启动前调用一次。
 */
void init_all(void);

/**
 * @brief 初始化周期中断定时器（CCU60 通道 0/1）
 *
 * 周期由 config.h 中的 PIT_PERIOD_MS 决定（默认 10ms）。
 * 应在 init_all() 之后、主循环之前调用。
 */
void pit_init_all(void);


#endif /* CODE_INIT_H_ */
