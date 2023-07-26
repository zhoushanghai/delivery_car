#ifndef __GW_GRAY_SERIAL_H__
#define __GW_GRAY_SERIAL_H__

#include <stdint.h>
#include "sys.h"
#include <stdlib.h>
// #include "usart.h"

#define GW_GRAY_SERIAL_GPIO_GROUP GPIOB
#define GW_GRAY_SERIAL_GPIO_CLK GPIO_Pin_6
#define GW_GRAY_SERIAL_GPIO_DAT GPIO_Pin_7

#define GET_NTH_BIT(sensor_value, nth_bit) (((sensor_value) >> ((nth_bit)-1)) & 0x01)

uint8_t gw_gray_serial_read(GPIO_TypeDef *GPIOx, uint16_t gpio_clk, uint16_t gpio_dat);
void spl_gw_gray_serial_init(void);
void get_gray_data(void);

#endif //__GW_GRAY_SERIAL_H__
