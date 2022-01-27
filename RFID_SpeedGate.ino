// RFID를 활용한 스피드게이트 출입 //
// 2019146018 송연주

// 아두이노 UNO 보드에서 동작 

#include <Servo.h>    // servo 라이브러리
#include <SPI.h>      // RFID를 위한 SPI 라이브러리
#include <MFRC522.h>  // RFID 라이브러리

#define SS_PIN  10   // SPi 통신을 위한 SS(chip select) 핀 설정
#define RST_PIN 9    // 리셋 핀 설정

MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key;
Servo myservo;  //myservo 객체 할당

// ID 선언
#define ID_0 0x73
#define ID_1 0xB8
#define ID_2 0x2F
#define ID_3 0x17

byte nuidPICC[4];

byte trigPin=3, echoPin=4, buzzer=5, servoPin=6;

void setup(){
  pinMode(trigPin, OUTPUT);   // TRIG 핀 번호 할당
  pinMode(echoPin, INPUT);    // ECHO 핀 번호 할당
  pinMode(buzzer, OUTPUT);    // BUZZER 핀 번호 할당
  myservo.attach(servoPin);
  myservo.write(0); // 초기 서보모터 값은 0도
  Serial.begin(9600);

  SPI.begin();    // SPI bus 초기화
  rfid.PCD_Init();  // RFID 초기화
}

void loop(){

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
    myservo.write(0);
    tone(buzzer, 2093, 500);
    
  }
  else{
    
    /////////////////// RFID ///////////////////
    if( ! rfid.PICC_IsNewCardPresent()) // 새 카드 접촉이 있을때만 다음단계로 넘어감
      return;
    if( ! rfid.PICC_ReadCardSerial()) // 카드 읽힘이 제대로 되면 다음으로 넘어감
      return;

    
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);  // 카드의 타입을 읽어옴
    Serial.println(rfid.PICC_GetTypeName(piccType));
  
    // 감지한 ID의 방식이 MIFARE가 아니라면 더이상 진행하지 않고 빠져나감 
    if(piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
       piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
       piccType != MFRC522::PICC_TYPE_MIFARE_4K){
        Serial.println(F("Your tag is not of type MIFARE Classic."));
        return;
       }
     
    
     // 초기 등록한 ID와 같다면
     if(rfid.uid.uidByte[0] != ID_0 ||
        rfid.uid.uidByte[1] != ID_1 ||
        rfid.uid.uidByte[2] != ID_2 ||
        rfid.uid.uidByte[3] != ID_3) {
          Serial.println(F("등록된 카드입니다."));
          Serial.print(F("카드 ID: "));
          printHex(rfid.uid.uidByte, rfid.uid.size);
          Serial.println();
          
          myservo.write(90);    // 서보모터 각도 90도
          
          tone(buzzer, 262, 500);
          delay(500);
          tone(buzzer, 330, 500);
          delay(500);
          tone(buzzer, 392, 500);
          delay(500);
          tone(buzzer, 523, 500);
          delay(500);
          
          delay(1000);
 
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
          }
          else{
            myservo.write(90);
            digitalWrite(buzzer, 1);
            delay(2000);
            myservo.write(0);
            digitalWrite(buzzer,0);
          }
          
        }
        else Serial.println(F("등록되지 않은 카드입니다."));
  
        rfid.PICC_HaltA();  // 종료
        rfid.PCD_StopCrypto1();   // 다시 시작
  }

}

// 카드 ID를 16진수로 바꿔주는 함수
void printHex(byte *buffer, byte bufferSize){
  for(byte i=0; i<bufferSize; i++){
    Serial.print(buffer[i] < 0x10 ? "0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
