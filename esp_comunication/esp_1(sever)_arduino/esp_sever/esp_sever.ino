
#include <WiFi.h>

const char* ssid = "ESP32_AP";      // Tên mạng Wi-Fi do AP phát
const char* password = "12345678";  // Mật khẩu (tối thiểu 8 ký tự)

WiFiServer server(80);  // Tạo server TCP port 80

void blink_led(){
  static unsigned long lastTime = 0;
  static int ledState = LOW;
  if (millis() - lastTime > 1000) {  // Thay đổi trạng thái LED mỗi giây
    ledState = (ledState == LOW) ? HIGH : LOW;  // Đảo trạng thái LED
    digitalWrite(2, ledState);  // Cập nhật trạng thái LED
    lastTime = millis();  // Cập nhật thời gian
  }
}
void setup() {
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH); // Tắt đèn LED trên board ESP32 (nếu có)
  Serial.begin(115200);
  
  // Thiết lập AP
  WiFi.softAP(ssid, password);
  
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());  // In địa chỉ IP của AP (ví dụ: 192.168.4.1)
  
  server.begin();  // Khởi động server
}

void loop() {
  WiFiClient client = server.available();  // Kiểm tra client kết nối
  
  if (client) {
    Serial.println("Client connected!");
    
    // Đọc dữ liệu từ client
    while (client.connected()) {
        
      if (client.available()) {
        blink_led();
        String command = client.readStringUntil('\r');  // Đọc lệnh đến khi gặp ký tự xuống dòng
        Serial.print("Nhận lệnh: ");
        Serial.println(command);
        
        // Gửi phản hồi về client
        client.print("AP đã nhận lệnh: " + command);
      }
    }
    
    client.stop();  // Ngắt kết nối
    Serial.println("Client disconnected");
  }
  else digitalWrite(2,1);
}