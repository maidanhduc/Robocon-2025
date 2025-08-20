#ifndef SET_BALL_AND_AREA_H  // Nếu chưa được định nghĩa
#define SET_BALL_AND_AREA_H   // Định nghĩa SET_BALL_AND_AREA_H 
// file này mục để để cài đặt nhanh các thông số pwm_default và speed_default tùy theo khu vực , bóng 
// example speed_default_12 mode bóng 1 khu vực 2
// bóng 1 // bỏ qua bóng 1 
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
#define DEFAULT_PWM_21 110
#define DEFAULT_PWM_22 120
#define DEFAULT_PWM_23 200
#define DEFAULT_PWM_24 250
#define DEFAULT_SPEED_21 5000
#define DEFAULT_SPEED_22 7000
#define DEFAULT_SPEED_23 8000
#define DEFAULT_SPEED_24 11000
// bóng 3 // 
#define DEFAULT_PWM_31 115
#define DEFAULT_PWM_32 130
#define DEFAULT_PWM_33 150
#define DEFAULT_PWM_34 245
#define DEFAULT_SPEED_31 6000
#define DEFAULT_SPEED_32 8000
#define DEFAULT_SPEED_33 9000
#define DEFAULT_SPEED_34 10000
// bóng 4 //
#define DEFAULT_PWM_41 95
#define DEFAULT_PWM_42 160
#define DEFAULT_PWM_43 200
#define DEFAULT_PWM_44 255
#define DEFAULT_SPEED_41 8000
#define DEFAULT_SPEED_42 9000
#define DEFAULT_SPEED_43 10000
#define DEFAULT_SPEED_44 12000
// bóng 5 // 
  #endif
#endif