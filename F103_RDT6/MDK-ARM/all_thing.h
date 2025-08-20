#ifndef ALL_THING_H
#define ALL_THING_H

#include "main.h"
#include "stdio.h"
#include "stdlib.h" 
#include "stdbool.h"
#include "robot.h"

/*===============nhay================*/
#define GPIO_day GPIOC
#define GPIO_day_pin GPIO_PIN_7

#define GPIO_kep GPIOC
#define GPIO_kep_pin GPIO_PIN_8

//SENSOR for JUMP
#define GPIO_sensor_port GPIOB
#define GPIO_sensor_pin GPIO_PIN_14

/*============gia tri tay cam========*/
#define button_up 			  ((0x01 	 & 	button) != 0)  // 0
#define button_down			  ((0x02	 &	button) != 0)  // 1
#define button_right		  ((0x04	 &	button) != 0)  // 2
#define button_left			  ((0x08	 &	button) != 0)  // 3
#define button_R1			  ((0x20	 &	button) != 0)  // 4
#define button_L1			  ((0x10	 &	button) != 0)  // 5
#define button_L2			  ((0x40	 &	button) != 0)  // 6
#define	button_R2			  ((0x80	 &	button) != 0)  // 7
#define button_square		  ((0x400	 &	button) != 0)  // 8
#define button_triangle       ((0x800	 &	button) != 0)  // 9
#define button_circle	      ((0x200	 &	button) != 0)  // 10
#define button_cross	      ((0x100	 &	button) != 0)  // 11
#define button_share	      ((0x2000 &	button) != 0) // 12
#define button_options        ((0x4000 &	button) != 0)  // 13
#define button_L3			  ((0x1000 &	button) != 0)  // 14
#define button_R3			  ((0x8000 &	button) != 0) // 15
#define RAD_TO_DEG  57.295779513082320876798154814105 // 180 / pi
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define abs(a) ((a) >= 0 ? (a) : -(a))

/*=======cho uart========*/
extern bool read_stt;
extern uint8_t rx_data[10];
extern uint16_t button;
extern int16_t RX;
extern int16_t LX;
extern int16_t RY;
extern int16_t LY;	
extern uint16_t speed_robot;
extern int32_t debug_var;
// for compas
extern bool angle_stt;
extern uint8_t angle_buff[2];
extern int16_t angle;
/*=========for move=============*/

extern long X;
extern long last_X;
extern long Y;
extern long last_Y;

extern bool okok;

/*=============cac ham tung tung tung sahur ==========*/

void Soft_start(uint16_t target_speed, uint8_t time);
void Soft_end(uint16_t target_speed, uint8_t time);
float constrain(float value, float min_val, float max_val);
int pid_cal(int set_point, int input,int constrain_p,  double pP, double pI, double pD);
void rundc1(int v, int vec);void rundc2(int v, int vec);void rundc3(int v, int vec);void rundc4(int v, int vec);
void move(int angle_rotare, int v_rotare, int angle_tt, int v_tt);
void rolate_right(uint16_t speed);
void rolate_left(uint16_t speed);
void test();
void hand_ctrl();
void test_nhay();
#endif