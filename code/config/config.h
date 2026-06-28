#ifndef CODE_CONFIG_CONFIG_H_
#define CODE_CONFIG_CONFIG_H_

/*===========================================================================*/
/*  GS_Smart_car 集中式可调参数                                              */
/*  所有可能需要调整的常量都集中在此文件。                                   */
/*  调参时只需修改本文件，无需改动其他源码。                                 */
/*===========================================================================*/

/* ===== 舵机参数（SERVO）===== */
/* 控制转向舵机，PWM 信号周期与占空比基准 */
#define SERVO_PWM_HZ            50      /* 舵机 PWM 频率，单位 Hz，标准舵机统一为 50Hz，勿改                */
#define SERVO_CENTER_DUTY       678     /* 舵机机械中位对应的占空比，装车后用标定尺校准，左右对称           */
#define SERVO_INIT_DUTY         692     /* 上电时初始占空比，略偏离中位以补偿机械偏置                      */
#define SERVO_RANGE             63      /* 舵机左右最大偏转量（占空比差值），超出此值视为打死方向          */

/* ===== 电机参数（MOTOR）===== */
/* 驱动后轮直流电机，PWM 频率与速度限幅 */
#define MOTOR_PWM_HZ            20000   /* 电机 PWM 频率，单位 Hz，20kHz 高于可听上限，避免啸叫           */
#define MOTOR_CLAMP_LEFT        15      /* 左电机速度限幅（绝对值），起步/弯道防打滑，调大则更激进        */
#define MOTOR_CLAMP_RIGHT       100     /* 右电机速度限幅（绝对值），通常大于左值以补偿左右轮差异         */

/* ===== PID 增益（PID GAINS）===== */
/* 分两组：舵机位置式、电机增量式。先调舵机保证循迹，再调电机保证速度 */
/* 舵机：转向响应。Kp 决定响应强度，Kd 抑制振荡，Ki 一般为 0          */
#define SERVO_PID_KP            3.0f    /* 舵机比例增益，偏大响应快但易抖；偏小循迹迟钝                   */
#define SERVO_PID_KI            0.0f    /* 舵机积分增益，循迹场景通常置 0，避免积分饱和                   */
#define SERVO_PID_KD            1.2f    /* 舵机微分增益，增大可压抖动，过大则对噪声敏感                   */

/* 电机：转速闭环。一般用 P 控制即可，I/D 留作扩展                    */
#define MOTOR_PID_KP            0.3f    /* 电机比例增益，决定目标速度与实际速度的耦合强度                 */
#define MOTOR_PID_KI            0.0f    /* 电机积分增益，消除稳态误差；存在静差时小幅加入                 */
#define MOTOR_PID_KD            0.0f    /* 电机微分增益，一般保持 0，避免编码器噪声放大                   */

/* ===== 视觉参数（VISION）===== */
/* 决定图像中线的提取与丢线判定 */
#define IMAGE_MID_COL           47      /* 图像参考中线列号（94 列宽下居中为 47），影响误差基准           */
#define LOST_LINE_THRESHOLD     10      /* 丢线计数阈值，连续丢线超过此值则暂停转向控制                   */
#define LOST_MIDLINE_GAP        40      /* 相邻行中线跳变阈值，跳变过大视为丢线，过滤毛刺                 */

/* ===== 周期中断（PERIODIC INTERRUPT）===== */
#define PIT_PERIOD_MS           10      /* CCU60 通道周期，单位 ms；默认 10ms，需与 data.c 中 dt 保持一致  */

#endif /* CODE_CONFIG_CONFIG_H_ */
