#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <SimpleKalmanFilter.h>

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(D0, D6,  D2, D1, 12345);
SimpleKalmanFilter kf = SimpleKalmanFilter(5, 5, 1);
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //Replace with receiver's MAC address.

typedef struct Smessage {
  int degreeOfServo;
} Smessage;

Smessage myData;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void senddegree(int degr){
    myData.degreeOfServo = degr;
    esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}
 
void setup() {
  Serial.begin(115200);
  Serial.println("Accelerometer Test"); Serial.println("");
  if(!accel.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while(1);
  }
  accel.setRange(ADXL345_RANGE_16_G);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

 
void loop() {
  sensors_event_t event; 
  accel.getEvent(&event);
  int drc=(int)(kf.updateEstimate((int)((((event.acceleration.y))+10)*180/20)));
  Serial.println(drc);
  senddegree(drc);
  delay(100);
}
