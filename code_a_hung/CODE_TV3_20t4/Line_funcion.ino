void line(){
  Serial1.print("l");
  static uint8_t st1 = 0; 
  static uint8_t st2 = 0; 
  if(Serial1.available()){
    st1 = Serial1.read(); 
    // st2 = Serial1.read();  
  }
  line_index = map(st1, 0, 28, -14, 14);
  if(st1 == 100){
    line_index = 0; 
  }

  line_index = constrain(line_index, -14, 14); 
  // line_index  = st1*255 +st2; 
  //Serial.println(st);
}

// ================================ Can thang line ==============================//
double pline = 0.12, iline = 1, dline = 0.005; 
int error_line = 0, last_error_line = 0; 
int output_line = 0; 
void line_pid(int set_point, int set_angle){
  error_line = set_point - line_index; 
  output_line = pline*error_line + iline*(last_error_line + error_line) + dline*(-last_error_line + error_line ); 
  last_error_line = error_line; 
  if(output_line < 0){
    left(abs(output_line), 0); 
  }else {
    right(output_line, 0); 
  }
}

//================================ run to silo and balance =============================// 
bool runl1 = true, runl2 = false , runl3 = false, runl4 = false ; 
uint32_t delay_runl1 = 0, delay_run_l2 = 0, delay_run_l3 = 0; 
void run_to_line(){
  if(runl1){
    flow_silo(150,0,80, 50, 10); 
    if(cb_run){
      runl1 = false; 
      runl2 = true; 
    }
  }
    if(runl2){
      flow_silo(150,0, 30, 50, 10);
      if(silo){
        runl1 = false; 
        runl2 = false; 
        runl3 = true; 
      }
    }
    if(runl3){

      line_pid(0, 0); 
      if(abs(line_index) < 9){
        runl1 = false; 
        runl2 = false; 
        runl3 = false; 
        runl4 = true; 
      }
    }
    if(runl4){
      nha_bong(); 
      stop(); 
    }
  Serial.print(runl1) ;Serial.print(runl2); Serial.print(runl3) ;Serial.println(runl4);
}

// Ham Reset run_to_line
void p_run_to_line(){
  runl1 = true; 
  runl2 = false; 
  runl3 = false; 
  runl4 = false; 
}

