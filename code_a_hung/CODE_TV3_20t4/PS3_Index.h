#define button_l2 0x80000000 & bt_in
#define button_l1 0x40000000 & bt_in
#define button_up 0x20000000 & bt_in
#define button_left 0x10000000 & bt_in
#define button_down 0x8000000 & bt_in
#define button_right 0x4000000 & bt_in
#define button_select 0x2000000 & bt_in
#define button_start 0x1000000 & bt_in
#define button_square 0x800000 & bt_in
#define button_x  0x400000 & bt_in
#define button_circle 0x200000 & bt_in
#define button_triangle 0x100000 & bt_in
#define button_r1  0x80000 & bt_in
#define button_r2  0x40000 & bt_in
#define button_l3 0x20000 & bt_in
#define button_r3 0x10000 & bt_in
#define lu 0x8000 & bt_in  // joy_stick, lu là jeft_up, tương tự như vậy ty=ừ dòng 17 đến 24 là joy_stick
#define ld 0x4000 & bt_in
#define ll 0x2000 & bt_in
#define lr 0x1000 & bt_in
#define ru 0x800 & bt_in
#define rd 0x400 & bt_in
#define rl 0x200 & bt_in
#define rr 0x100 & bt_in
#define NO 0x00 & bt_in

uint32_t bt_in = 0;