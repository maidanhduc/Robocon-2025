double PI_cons = 3.141592654;
volatile uint32_t xung=0;
uint32_t dist_en;
void encoder_(){
  xung++;
  dist_en = xung*PI_cons*0.1;
  
}
