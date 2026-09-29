/******************************************************************************
arduino_mpu6500_clk.h - MPU-6500 Digital Motion Processor Arduino Library 
******************************************************************************/
#ifndef _ARDUINO_MPU6500_CLK_H_
#define _ARDUINO_MPU6500_CLK_H_

int arduino_get_clock_ms(unsigned long *count);
int arduino_delay_ms(unsigned long num_ms);

#endif // _ARDUINO_MPU6500_CLK_H_
