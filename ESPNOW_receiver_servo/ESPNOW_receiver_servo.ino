#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Servo.h>


typedef struct Smessage {
    int degreeOfServo;
} Smessage;

Smessage myData;
Servo servo;

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Servo: ");
  Serial.println(myData.degreeOfServo);
  Serial.println();
  servo.write(myData.degreeOfServo);
}
 
void setup() {
  Serial.begin(115200);
  servo.attach(D4,544,2400); //Servo motor signal pin connected D4 pin.
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
}
