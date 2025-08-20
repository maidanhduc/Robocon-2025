#include <WiFi.h>

const char* ssid = "ESP32_AP";
const char* password = "12345678";

WiFiServer server(80);

// Các trạng thái hệ thống
enum SystemStatus {
  STATUS_IDLE,          // Chờ kết nối
  STATUS_CONNECTED,     // Có client kết nối
  STATUS_DATA_RECEIVED  // Đang nhận dữ liệu
};

SystemStatus currentStatus = STATUS_IDLE;
unsigned long lastBlinkTime = 0;
const long statusCheckInterval = 500; // Kiểm tra mỗi 500ms

void updateLed() {
  unsigned long currentMillis = millis();
  
  switch(currentStatus) {
    case STATUS_IDLE:
      // Nhấp nháy chậm (1 giây) khi chờ kết nối
      if (currentMillis - lastBlinkTime >= 1000) {
        digitalWrite(2, !digitalRead(2));
        lastBlinkTime = currentMillis;
      }
      break;
      
    case STATUS_CONNECTED:
      // Sáng liên tục khi có client kết nối
      digitalWrite(2, HIGH);
      break;
      
    case STATUS_DATA_RECEIVED:
      // Nhấp nháy nhanh (200ms) khi nhận dữ liệu
      if (currentMillis - lastBlinkTime >= 200) {
        digitalWrite(2, !digitalRead(2));
        lastBlinkTime = currentMillis;
      }
      break;
  }
}

void handleClient(WiFiClient &client) {
  currentStatus = STATUS_CONNECTED;
  
  while (client.connected()) {
    if (client.available()) {
      currentStatus = STATUS_DATA_RECEIVED;
      
      String command = client.readStringUntil('\r');
      command.trim();
      
      Serial.print("Received: ");
      Serial.println(command);
      
      // Xử lý lệnh đơn giản
      if (command.equalsIgnoreCase("LED_ON")) {
        digitalWrite(2, HIGH);
        client.print("LED is ON");
      } 
      else if (command.equalsIgnoreCase("LED_OFF")) {
        digitalWrite(2, LOW);
        client.print("LED is OFF");
      }
      else {
        client.print("Unknown command");
      }
      
      currentStatus = STATUS_CONNECTED; // Trở lại trạng thái connected sau khi xử lý
    }
    updateLed(); // Cập nhật LED liên tục
  }
  
  client.stop();
  currentStatus = STATUS_IDLE;
  Serial.println("Client disconnected");
}

void setup() {
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  server.begin();
  Serial.println("Server started");
}

void loop() {
  updateLed(); // Luôn cập nhật trạng thái LED
  
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    handleClient(client);
  }
}