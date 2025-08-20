#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "ESP32_AP";      // Tên mạng AP
const char* password = "12345678";  // Mật khẩu

WiFiClient client;
unsigned long lastConnectionTime = 0;
const long connectionInterval = 2000; // Gửi lệnh mỗi 2 giây

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW); // Ban đầu tắt LED
  
  // Kết nối vào AP
  WiFi.begin(ssid, password);
  Serial.print("Connecting to AP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected!");
  Serial.print("STA IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Kiểm tra kết nối WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.reconnect();
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nReconnected!");
  }

  // Gửi lệnh định kỳ
  if (millis() - lastConnectionTime >= connectionInterval) {
    if (client.connect("192.168.4.1", 80)) {
      Serial.println("Sending command to AP");
      client.println("GET_STATUS"); // Yêu cầu trạng thái hiện tại
      
      // Nhận phản hồi
      unsigned long startTime = millis();
      while (millis() - startTime < 1000) { // Timeout 1 giây
        if (client.available()) {
          String response = client.readStringUntil('\r');
          Serial.print("AP response: ");
          Serial.println(response);
          
          // Xử lý phản hồi từ AP
          if (response.indexOf("ON") != -1) {
            digitalWrite(2, HIGH);
          } else if (response.indexOf("OFF") != -1) {
            digitalWrite(2, LOW);
          }
          break;
        }
      }
      client.stop();
    } else {
      Serial.println("Connection to AP failed!");
      digitalWrite(2, LOW); // Tắt LED khi mất kết nối
    }
    lastConnectionTime = millis();
  }
}