# IoT 기초 Term Project - RFID와 MQTT를 활용한 스피드게이트 출입
<br>

## 1. 프로젝트 계획
### 1.1 연구 개발 배경
![image](https://user-images.githubusercontent.com/72501562/151308893-b58b22b1-a00b-42d6-8feb-e63a8bd94ae3.png)

필요성 측면에서, 기숙사 생활을 하면서 출입카드를 잊어버리고 들고나오지 못했을 때, 스피드게이트를 통과하기 위해 다시 방에 돌아가야 하는 불편함을 느꼈다. 또한, 사회적 이슈 측면에서 바라볼 때 최근 2년간 코로나19로 인해 비접촉식 체온계, 음주 감지기, 엘리베이터 등 다양한 비접촉식 제품이 나오고 있다. 이를 통해 카드를 찍지 않아도 되는 비접촉식 스피드게이트 출입 방식을 고안하였다. 

### 1.2 개념도
![image](https://user-images.githubusercontent.com/72501562/151309021-8d8316b4-72dc-4ee4-bc52-2edbf34ba5ab.png)

MQTT 브로커로는 mosquito를 사용했다. RFID 카드를 태그하거나 MQTT 메시지로 “OPEN” 이라는 단어를 보내면 서보모터가 회전하도록 구현하고자 하였다. 또한 초음파센서를 이용하여 허용되지 않은 사용자가 감지되었을 시에 부저를 통해 경보를 울리고자 하였다. 

### 1.3 블록 다이어그램
![image](https://user-images.githubusercontent.com/72501562/151309109-dde6682b-03e0-4bc1-9f80-0e54c3ed1919.png)

### 1.4 동작 시나리오 
**동작 시나리오 1 – RFID 카드 태그** 

①	RFID 카드/버튼이 태그되지 않았는데 스피드게이트에 출입하면 (초음파센서를 통해 사람이 감지되면)  부저를 울린다.

②	RFID 카드/버튼을 태그한다.

③	스피드게이트 문을 연다. (서보모터 회전)  

④	특정 시간이 지나고, 초음파 센서에 사람이 감지되지 않으면 스피드게이트 문을 다시 닫는다. (서보모터 회전)

**동작 시나리오 2 – MQTT Message 전송**

①	스피드게이트 출입 가능자만 Broker에 연결할 수 있다고 가정

②	MQTT broker에 “OPEN” Message를 전송한다. 	

③	Message가 전송되지 않았는데 스피드게이트에 출입하면 (초음파센서를 통해 사람이 감지되면)  부저를 울린다.

④	“OPEN” Message를 수신하면 스피드게이트 문을 연다. (서보모터 회전)  

⑤	특정 시간이 지나고, 초음파 센서에 사람이 감지되지 않으면 스피드게이트 문을 다시 닫는다. (서보모터 회전)

<br>

## 2. 회로 구성
![image](https://user-images.githubusercontent.com/72501562/151309739-3c828236-023a-4b94-9380-cc2f9ef96da6.png)
<br>
## 3. 문제점 및 해결방안
### 3.1 RFID RC522모듈 라이브러리의 ESP8266 보드 지원 문제
아두이노 IDE를 통해 RFID RC522 모듈을 사용할 때에는 , MFRC522라는 라이브러리를 사용한다. 아두이노 UNO 보드를 사용했을 때에는 RFID에 카드를 태그하면 서보모터가 회전하는 동작을 잘 구현했지만, ESP8266 보드에 사용했을 때에는 구현하지 못했다.
<br>
https://github.com/miguelbalboa/rfid 사이트에 들어가보면, <Compatible boards> 부분에서 "This library is compatible with the Teensy and ESP8266 if you use the board plugin of the Arduino IDE. Not all examples are available for every board. You also have to change pins. See pin layout.” 라고 언급하였다. 이번 수업에서 사용하는 보드에서는 이 MFRC522 라이브러리를 사용할수 없는 것으로 추정되며, 이에 대한 해결은 하지 못하였다. 따라서 아두이노 UNO 보드에서 동작하는 것은 확인하였고, ESP8266보드에서는 MQTT 메시지를 활용하여 서보모터를 제어하는 기능만 구현하고자 하였다. 
### 3.2 서보모터의 전압 문제
  서보모터를 회전시키는 동작을 구현하였을 때, 서보모터가 제대로 회전하지 않고 진동하듯이 동작했었다. 확인을 해보니 본 프로젝트에서 사용한 서보모터 SG90 모델은 정격전압이 4.7~5.0 VDC 이며, ESP8266의 전원 전압은 3.3V이기 때문에 서보모터가 정상적으로 동작하지 않을 수 있다고 한다. 노트북으로부터 들어오는 전원을 사용해도 되지만 이것조차 서보모터가 요구하는 정격 전류보다 낮다면 제대로 동작하지 않을 수 있다는 것을 알게 되었다. 따라서 5V 2.1A 전원 어댑터를 사용하였다. <br>
![image](https://user-images.githubusercontent.com/72501562/151309973-3e7662bb-5bd6-47ad-b099-24c4964d57e3.png)
  
<br>
  
## 4. 소스코드
### 4.1  RFID 카드로 스피드게이트 출입 (Arduino UNO)
[이 소스코드](https://github.com/ssongplay/IoT_speedgate/blob/main/RFID_SpeedGate.ino)는 RFID 카드를 태그하면 서보모터가 열리고, 허용되지 않은 사람이 감지되었을 시 부저음을 울리는 동작을 수행한다. 3.1에서 언급했듯이 아두이노 UNO 보드에 업로드하였다. (*동영상은 촬영하지 못하였다.) 

### 4.2 최종 소스코드 (ESP8266 NodeMCU)
[최종 소스코드](https://github.com/ssongplay/IoT_speedgate/blob/main/MQTT_SpeedGate.ino)에서는 RFID모듈과 MQTT 기능을 한번에 하지 못했던 점이 아쉬워서 OLED에 문자를 출력하는 기능을 추가했다. 게이트가 닫혀 있을 때에는 “CLOSED... “, 열릴 때에는 “OPEN 😊 “, 허용되지 않은 사용자가 출입할 때에는 “Watch Out!! “을 출력한다.

  
<br>
  
## 5. 동작 결과
아두이노 UNO 보드를 사용하여 RFID 스피드게이트를 구현한 것을 제외하면, 크게 세가지 동작을 구현하였다.
  
① 물체가 감지되면 부저를 울린다.
  
② MQTT mosquitto 서버를 통해 “OPEN” 을 publish 하면 스피드게이트가 열린다.
  
③ 문이 열린 후 일정시간 후에 닫히는데, 닫히는 도중에 물체가 감지되면 물체가 빠져나갈 때까지 부저를 울리며 문이 열린 상태로 있는다. 
  
[동작 영상 링크](https://youtu.be/8N3pLHOi_jQ)
  
OLED에는 다음과 같은 결과들이 출력된다. 
  ![image](https://user-images.githubusercontent.com/72501562/151311212-9d2dc507-4c36-42cd-ab2e-ecfaf308c4e6.png)

MQTT 메시지 publish를 위해 어플을 사용하거나 cmd 창을 사용할 수 있다. 
  
![image](https://user-images.githubusercontent.com/72501562/151311262-5389638d-127a-4bc3-b8d8-b5b538b532e1.png)
![image](https://user-images.githubusercontent.com/72501562/151311278-94e0cbc7-0944-46ed-ab7b-9a1423e10499.png)
  
<br>
  
## 6. 느낀 점
 한 학기 동안 수업을 듣고 IoT에 대한 기초 지식을 쌓을 수 있었고, 관심을 가지게 되는 계기가 되었다. 특기 학기 말에 Term Project를 수행하면서 혼자 많은 것들을 찾아보고 부딪혀볼 수 있었다. 서버에 대한 개념이 어려웠는데 MQTT mosquito 서버 구축 실습을 하고 프로젝트에 응용함으로써 감을 잡을 수 있었다. 또한 실습시간에 사용해본 각종 센서들의 사용법을 익혔고, 그 외에 RFID등 다양한 모듈을 사용할 수 있는 실력도 쌓게 되었다. 아쉬웠던 점은 교육용 보드의 핀이 부족하여 더 다양한 기능을 실행해보지 못했다는 점과, 시간 부족으로 인하여 좀 더 실제처럼 스피드게이트 데모를 꾸며보지 못했다는 점이다. 본 프로젝트 이후에도 꾸준히 IoT 분야에 관심을 가지고 혼자서도 다양한 기능들을 구현해보고 싶다. 
  
