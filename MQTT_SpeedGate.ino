// MQTT를 활용한 스피드게이트 출입 //
// 2019146018 송연주

// 1. 초음파, 서보모터, 부저 구현 
// 2. MQTT 추가
// 3. OLED 추가

#include <Wire.h>
#include "OLED.h"
OLED display(4, 5);
char strdata[100];

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ssmaek2";
const char* password = "11111111";

#define mqtt_server "ihlab.iptime.org"  // MQTT server 주소
#define mqtt_port 1883                  // port 번호
#define mqtt_topic "speedgate"               // topic (자유롭게 작성}
#define mqtt_user "IoT"                 // username
#define mqtt_password "IoT_Test@"       // password

#include <Servo.h>    // servo 라이브러리

Servo myservo;  //myservo 객체 할당


byte trigPin=13, echoPin=12, buzzer=16, servoPin=15;

WiFiClient espClient;
PubSubClient client(espClient);
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.subscribe(mqtt_topic);
    } 
    else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(){
  pinMode(trigPin, OUTPUT);   // TRIG 핀 번호 할당
  pinMode(echoPin, INPUT);    // ECHO 핀 번호 할당
  pinMode(buzzer, OUTPUT);    // BUZZER 핀 번호 할당
  myservo.attach(servoPin);
  myservo.write(0); // 초기 서보모터 값은 0도
  Serial.begin(9600);
  display.begin();

  randomSeed(micros());
   
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
  }
     
  //wifi connected
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

}

void loop(){

  if(!client.connected()){
    reconnect();
  }
  else;
    
  client.loop();

  //////////////// 초음파 센서 ////////////////
  digitalWrite(trigPin, 0), digitalWrite(echoPin, 0);
  delayMicroseconds(2);   //2us 유지
  digitalWrite(trigPin, 1);
  delayMicroseconds(10);  //10us 유지
  unsigned long dur=pulseIn(echoPin, 1);    //ECHO핀에 입력이 들어왔을 때 시간을 dur에 저장
  float dist = dur/29.0/2.0;
  Serial.print("distance = ");
  Serial.print(dist); Serial.println(" cm");
  delay(200);   // 0.2초 간격 
  ////////////////////////////////////////////


  if(dist < 20){
    display.print("Watch Out!!", 1, 4);
    myservo.write(0);
    delay(200);
    digitalWrite(buzzer, 1);
    delay(1000);
    digitalWrite(buzzer, 0);
    display.print("CLOSE ...  ", 1, 4);
  }
  
}

void open_gate(){
    display.print("OPEN :-)    ", 1,4);
    
    myservo.write(180);    // 서보모터 각도 180도
    
    tone(buzzer, 262, 500);
    delay(300);
    tone(buzzer, 330, 500);
    delay(300);
    tone(buzzer, 392, 500);
    delay(300);
    tone(buzzer, 523, 500);
    delay(300);
    
    delay(1500);
    
    //////////////// 초음파 센서 ////////////////
    digitalWrite(trigPin, 0), digitalWrite(echoPin, 0);
    delayMicroseconds(2);   //2us 유지
    digitalWrite(trigPin, 1);
    delayMicroseconds(10);  //10us 유지
    unsigned long dur=pulseIn(echoPin, 1);    //ECHO핀에 입력이 들어왔을 때 시간을 dur에 저장
    float dist = dur/29.0/2.0;
    Serial.print("distance = ");
    Serial.print(dist); Serial.println(" cm");
    delay(200);   // 0.2초 간격 
    ////////////////////////////////////////////
    
    if(dist>20){
      delay(1000);
      myservo.write(0);
      display.print("CLOSE ...  ", 1, 4);
    }
    else{
      display.print("Watch Out!! ", 1, 4);
      myservo.write(180);
      digitalWrite(buzzer, 1);
      delay(1500);
      myservo.write(0);
      digitalWrite(buzzer,0);
      display.print("CLOSE ...  ", 1, 4);
    }
    
}

void callback(char* topic, byte* payload, unsigned int length){

  String Msg = "";
  unsigned int count =0;
   
  // message received
  while (count<length) 
    Msg += (char)payload[count++];

  if(Msg =="OPEN"){
    open_gate();
  }
  else;
}
