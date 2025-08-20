#ifndef SET_BALL_AND_AREA_H  // Nếu chưa được định nghĩa
#define SET_BALL_AND_AREA_H   // Định nghĩa SET_BALL_AND_AREA_H 
// file này mục để để cài đặt nhanh các thông số pwm_default và speed_default tùy theo khu vực , bóng 
// example speed_default_12 mode bóng 1 khu vực 2
// bóng 1 //
  #if 1
#define DEFAULT_PWM_11 95
#define DEFAULT_PWM_12 90
#define DEFAULT_PWM_13 85
#define DEFAULT_PWM_14 80
#define DEFAULT_SPEED_11 12000
#define DEFAULT_SPEED_12 10000
#define DEFAULT_SPEED_13 8000
#define DEFAULT_SPEED_14 60000
// bóng 2 // 
#define DEFAULT_PWM_21 1
#define DEFAULT_PWM_22 2
#define DEFAULT_PWM_23 3
#define DEFAULT_PWM_24 4
#define DEFAULT_SPEED_21 5
#define DEFAULT_SPEED_22 6
#define DEFAULT_SPEED_23 7
#define DEFAULT_SPEED_24 8
// bóng 3 // 
#define DEFAULT_PWM_31 9
#define DEFAULT_PWM_32 10
#define DEFAULT_PWM_33 11
#define DEFAULT_PWM_34 12
#define DEFAULT_SPEED_31 13
#define DEFAULT_SPEED_32 14
#define DEFAULT_SPEED_33 15
#define DEFAULT_SPEED_34 16
// bóng 4 //
#define DEFAULT_PWM_41 17
#define DEFAULT_PWM_42 18
#define DEFAULT_PWM_43 19
#define DEFAULT_PWM_44 20
#define DEFAULT_SPEED_41 21
#define DEFAULT_SPEED_42 22
#define DEFAULT_SPEED_43 23
#define DEFAULT_SPEED_44 24

  #endif
#endif