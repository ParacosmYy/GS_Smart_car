#ifndef CODE_CONFIG_CONFIG_H_
#define CODE_CONFIG_CONFIG_H_

/**
 * @file config.h
 * @brief Central tunable parameter table for GS_Smart_car.
 *
 * Parameters that normally change during track tuning are kept here so that
 * control, vision and actuator behavior can be adjusted without editing
 * module implementation files.
 */

/*===========================================================================*/
/*  Parameter Overview                                                       */
/*===========================================================================*/

/* ===== Servo Parameters ===== */
/* 控制转向舵机，PWM 信号周期与占空比基准 */
#define SERVO_PWM_HZ            50      /* 舵机 PWM 频率，单位 Hz，标准舵机统一为 50Hz，勿改                */
#define SERVO_CENTER_DUTY       678     /* 舵机机械中位对应的占空比，装车后用标定尺校准，左右对称           */
#define SERVO_INIT_DUTY         692     /* 上电时初始占空比，略偏离中位以补偿机械偏置                      */
#define SERVO_RANGE             63      /* 舵机左右最大偏转量（占空比差值），超出此值视为打死方向          */

/* ===== Motor Parameters ===== */
/* 驱动后轮直流电机，PWM 频率与速度限幅 */
#define MOTOR_PWM_HZ            20000   /* 电机 PWM 频率，单位 Hz，20kHz 高于可听上限，避免啸叫           */
#define MOTOR_CLAMP_LEFT        15      /* 左电机速度限幅（绝对值），起步/弯道防打滑，调大则更激进        */
#define MOTOR_CLAMP_RIGHT       100     /* 右电机速度限幅（绝对值），通常大于左值以补偿左右轮差异         */

/* ===== PID Gains ===== */
/* 分两组：舵机位置式、电机增量式。先调舵机保证循迹，再调电机保证速度 */
/* 舵机：转向响应。Kp 决定响应强度，Kd 抑制振荡，Ki 一般为 0          */
#define SERVO_PID_KP            3.0f    /* 舵机比例增益，偏大响应快但易抖；偏小循迹迟钝                   */
#define SERVO_PID_KI            0.0f    /* 舵机积分增益，循迹场景通常置 0，避免积分饱和                   */
#define SERVO_PID_KD            1.2f    /* 舵机微分增益，增大可压抖动，过大则对噪声敏感                   */

/* 电机：转速闭环。一般用 P 控制即可，I/D 留作扩展                    */
#define MOTOR_PID_KP            0.3f    /* 电机比例增益，决定目标速度与实际速度的耦合强度                 */
#define MOTOR_PID_KI            0.0f    /* 电机积分增益，消除稳态误差；存在静差时小幅加入                 */
#define MOTOR_PID_KD            0.0f    /* 电机微分增益，一般保持 0，避免编码器噪声放大                   */

/* ===== Vision Parameters ===== */
/* 决定图像中线的提取与丢线判定 */
#define IMAGE_MID_COL           47      /* 图像参考中线列号（94 列宽下居中为 47），影响误差基准           */
#define LOST_LINE_THRESHOLD     10      /* 丢线计数阈值，连续丢线超过此值则暂停转向控制                   */
#define LOST_MIDLINE_GAP        40      /* 相邻行中线跳变阈值，跳变过大视为丢线，过滤毛刺                 */

/* ===== Periodic Interrupt ===== */
#define PIT_PERIOD_MS           10      /* CCU60 通道周期，单位 ms；传感器积分周期默认由此换算              */

/* ===== Buzzer Parameters ===== */
/* GPIO 有源蜂鸣器（P11_11），通过开关时序产生不同提示音           */
/* 1 帧 ≈ 33ms（30fps），帧数 × 33 ≈ 实际毫秒数                  */
#define BUZZER_BEEP_SHORT_F    3       /* 短声持续帧数（~100ms），用于圆环提示       */
#define BUZZER_BEEP_LONG_F     12      /* 长声持续帧数（~400ms），用于十字路口提示   */
#define BUZZER_GAP_F           2       /* 声间间隔帧数（~66ms），区分连续短声        */

/* ===== Element Detection ===== */
/* 基于边线数据的启发式检测，行索引范围对应图像中下部              */
/* 行号说明：0=图像顶部（远端），59=图像底部（近端/车前）         */
#define ELEM_ROW_START         20      /* 检测窗口起始行（跳过远端噪点）             */
#define ELEM_ROW_END           50      /* 检测窗口结束行（跳过近端车体遮挡）         */
#define ELEM_EDGE_LEFT         2       /* 左边线贴边阈值（列号 ≤ 此值视为丢线）      */
#define ELEM_EDGE_RIGHT        91      /* 右边线贴边阈值（列号 ≥ 此值视为丢线）      */
#define ELEM_CROSSROAD_ROWS    8       /* 双侧丢线行数 ≥ 此值 → 判定十字路口         */
#define ELEM_RING_ROWS         15      /* 单侧丢线行数 ≥ 此值 → 判定圆环             */
#define ELEM_COOLDOWN_FRAMES   30      /* 检测冷却帧数（~1s），防止单元素重复触发     */

#endif /* CODE_CONFIG_CONFIG_H_ */
