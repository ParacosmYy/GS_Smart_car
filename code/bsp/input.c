/*
 * input.c
 */
#include "input.h"
#include "platform/interface/mcu_io_if.h"
#include "system/board/smartcar_board_resources.h"
static const uint16_t s_keys[4] = {SMARTCAR_GPIO_KEY1,SMARTCAR_GPIO_KEY2,SMARTCAR_GPIO_KEY3,SMARTCAR_GPIO_KEY4};
static const uint16_t s_dips[4] = {SMARTCAR_GPIO_DIP1,SMARTCAR_GPIO_DIP2,SMARTCAR_GPIO_DIP3,SMARTCAR_GPIO_DIP4};
state_machine_enum state = IMAGE_VALUE;
static input_t s_input = {0};
static void scan_key(input_t *in, uint8_t idx){
    uint8_t p=in->key_level[idx],c=McuIo_GpioRead(s_keys[idx]);
    in->key_last_level[idx]=p;in->key_level[idx]=c;in->key_pressed[idx]=(c&&!p)?1:0;
}
static void scan_dip(input_t *in, uint8_t idx){in->dip_switch[idx]=(McuIo_GpioRead(s_dips[idx])==0)?1:0;}
void Input_Init(void){for(uint8_t i=0;i<4;i++){McuIo_GpioInit(s_keys[i],MCUIO_GPIO_INPUT);McuIo_GpioInit(s_dips[i],MCUIO_GPIO_INPUT);s_input.key_level[i]=1;s_input.key_last_level[i]=1;}}
void Input_Scan(void){for(uint8_t i=0;i<4;i++){scan_key(&s_input,i);scan_dip(&s_input,i);}}
uint8_t Input_GetPressedKey(void){for(uint8_t i=0;i<4;i++){if(s_input.key_level[i]==0)return(uint8_t)(i+1);}return 0;}
