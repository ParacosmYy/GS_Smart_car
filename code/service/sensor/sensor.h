#ifndef SENSOR_H_
#define SENSOR_H_

void Sensor_ProcessGyro10ms(void);
void Sensor_ProcessEncoder50ms(void);

int Sensor_GetLeftEncoderSpeed(void);
int Sensor_GetRightEncoderSpeed(void);

#endif /* SENSOR_H_ */
