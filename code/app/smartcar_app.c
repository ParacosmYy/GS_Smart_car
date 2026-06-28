#include "smartcar_app.h"
#include "zf_common_headfile.h"
#include "control.h"

void SmartcarApp_Init(void)
{
    init_all();
    pit_init_all();
}

void SmartcarApp_RunOnce(void)
{
    if(mt9v03x_finish_flag == 1)
    {
        Vision_Process();
        Control_Update();
        Actuator_Apply();
        mt9v03x_finish_flag = 0;
    }

    tft180_show_gray_image(0, 0, mt9v03x_image_bandw_zip[0], 94, 60, MT9V03X_W / 2, MT9V03X_H / 2, 0);

    tft180_show_string(0, 80, "left:");
    tft180_show_int(50, 80, left_encoder_speed, 4);

    tft180_show_string(0, 60, "right:");
    tft180_show_int(50, 60, right_encoder_speed, 4);

    tft180_show_string(0, 100, "l_spd:");
    tft180_show_string(0, 120, "r_spd:");

    tft180_show_int(50, 100, (int32)left_motor_pid_output, 6);
    tft180_show_int(50, 120, (int32)right_motor_pid_output, 6);

    tft180_show_string(0, 140, "err:");
    tft180_show_int(50, 140, calculate_error, 4);
}
