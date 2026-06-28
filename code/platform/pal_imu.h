#ifndef PAL_IMU_H_
#define PAL_IMU_H_

/**
 * @file pal_imu.h
 * @brief PAL IMU 接口。
 */

void  pal_gyro_init(void);
void  pal_gyro_read(void);
float pal_gyro_z(void);

#endif /* PAL_IMU_H_ */
