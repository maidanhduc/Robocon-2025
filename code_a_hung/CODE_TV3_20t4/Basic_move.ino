// ========================  Di chuyển cơ bản ========================  //
void soft_start(int ac_start, int speedv){ 
  //if(!soft_state) speedv1 = 0; 
  if( soft_state){
    if(v_bot < speedv){
      if( millis() - timea > ac_start){
        v_bot ++; 
        timea = millis(); 
      }
    }
  }
}

void down(int v){
  dc1.quaynghich(v); 
  dc3.quaynghich(v); 
  dc2.quaythuan(v); 
  dc4.quaythuan(v); 
}

void up(int v){
  dc2.quaynghich(v); 
  dc4.quaynghich(v); 
  dc1.quaythuan(v); 
  dc3.quaythuan(v); 
}

void right(int va, int vb){
  dc1.quaythuan(abs(va+vb)); 
  dc4.quaynghich(abs(va-vb)); 
  dc2.quaythuan(abs(va+vb)); 
  dc3.quaynghich(abs(va-vb)); 
}

void left_1(int va, int vb){
  dc2.quaynghich( abs(va )); 
  dc3.quaythuan(abs(va+vb)); 
  dc1.quaynghich(abs(va )); 
  dc4.quaythuan(abs(va+vb)); 
}

void right_1(int va, int vb){
  dc1.quaythuan(abs(va+vb)); 
  dc4.quaynghich(abs(va)); 
  dc2.quaythuan(abs(va+vb)); 
  dc3.quaynghich(abs(va)); 
}

void left(int va, int vb){
  dc2.quaynghich( abs(va - vb)); 
  dc3.quaythuan(abs(va+vb)); 
  dc1.quaynghich(abs(va -vb)); 
  dc4.quaythuan(abs(va+vb)); 
}

//==========================================
void rota_l(int v){
  dc1.quaynghich(v); 
  dc3.quaynghich(v); 
  dc2.quaynghich(v); 
  dc4.quaynghich(v); 
}

void rota_r(int v){
  dc1.quaythuan(v); 
  dc3.quaythuan(v); 
  dc2.quaythuan(v); 
  dc4.quaythuan(v); 
}

void stop(){
  dc1.phanh();
  dc2.phanh();
  dc3.phanh();
  dc4.phanh();
}
//=========================================== Chạy lệch 
void up_rl(int v_left, int v_right){
  dc2.quaynghich(v_right); 
  dc4.quaynghich(v_right); 
  dc1.quaythuan(v_left); 
  dc3.quaythuan(v_left);
}

void down_rl(int v_left, int v_right){
  dc1.quaynghich(v_left); 
  dc3.quaynghich(v_left); 
  dc2.quaythuan(v_right); 
  dc4.quaythuan(v_right); 
}

