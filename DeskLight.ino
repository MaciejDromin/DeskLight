#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#define trig 4
#define echo 5
#define led 0
#define num_leds 38
boolean ledsEnabled;
const char* ssid = "your wifi name";
const char* password = "password";
int R = 255;
int G = 255;
int B = 255;
int nOfTimes = 0;

Adafruit_NeoPixel pixels(num_leds, led, NEO_GRB + NEO_KHZ800);
ESP8266WebServer server(80); 

void setup() {
  Serial.begin(115200);
  pinMode (trig, OUTPUT);
  pinMode (echo, INPUT);
  pinMode (led, OUTPUT);
  ledsEnabled = false;
  pixels.begin();
  pixels.show();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());
  server.on("/color", handleColor);
  server.begin();      
}

void loop() {
  server.handleClient();
  float pomiarRet = pomiar();
  if(pomiarRet>20){
    if(!ledsEnabled){
      nOfTimes = nOfTimes>=3? 3 : (nOfTimes + 1);
    } if(ledsEnabled){
      nOfTimes = nOfTimes<=0? 0 : (nOfTimes - 1);
    }
  } else if(pomiarRet<=20){
    if(!ledsEnabled){
      nOfTimes = nOfTimes<=0? 0 : (nOfTimes - 1);
    } if(ledsEnabled){
      nOfTimes = nOfTimes>=3? 3 : (nOfTimes + 1);
    }
  }
  if(nOfTimes==3){
    turnOnOff(pomiarRet);
    nOfTimes = 0;
  }
}
  
float pomiar(){
  unsigned long czas;
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  czas = pulseIn(echo,HIGH);
  return czas*340 / 20000;
}

void turnOnOff(float pomiar){
  if(pomiar>20.0f && !ledsEnabled){
    initLeds();
    ledsEnabled = true;
  } else if(pomiar<=20.0f && ledsEnabled) {
    disableLeds();  
    ledsEnabled = false;
  }
}

void initLeds(){
  for(int i = 0;i<38;i++){
    if(i!=0){
      pixels.setPixelColor(i-1, pixels.Color(0,0,0));
    }
    pixels.setPixelColor(i, pixels.Color(R,G,B));
    pixels.show();
    delay(20);
  }
  for(int i = 37;i>=0;i--){
    if(i!=59){
      pixels.setPixelColor(i+1, pixels.Color(0,0,0));
    }
    pixels.setPixelColor(i, pixels.Color(R,G,B));
    pixels.show();
    delay(20);
  }
  for(int i = 0;i<38;i++){
    pixels.setPixelColor(i, pixels.Color(R,G,B));
    pixels.show();
    delay(5);
  }
}

void disableLeds(){
  for(int i = 38;i>=0;i--){
    pixels.setPixelColor(i, pixels.Color(0,0,0));
    pixels.show();
    delay(5);
  }
}

void handleColor(){
  int colorValue = server.arg("R").toInt();
  int colors[3];
  boolean allArgsValid = true;
  if (colorValue>=0&&colorValue<=255){
    colors[1] = colorValue;
  } else {allArgsValid = false;}
  colorValue = server.arg("G").toInt();
  if (colorValue>=0&&colorValue<=255){
    colors[2] = colorValue;
  } else {allArgsValid = false;}
  colorValue = server.arg("B").toInt();
  if (colorValue>=0&&colorValue<=255){
    colors[3] = colorValue;
  } else {allArgsValid = false;}
  if (allArgsValid){
    changeColor(colors);
  }
}

void changeColor(int colors[3]){
  int holdR = R;
  int holdG = G;
  int holdB = B;
  R = colors[1];
  G = colors[2];
  B = colors[3];
  if (ledsEnabled){
    while(pixels.Color(R,G,B)!=pixels.Color(holdR,holdG,holdB)){
      if(holdR<R&&holdR!=R){
        holdR++;
      } else if(holdR>R&&holdR!=R){
        holdR--;
      }
      if(holdG<G&&holdG!=G){
        holdG++;
      } else if(holdG>G&&holdG!=G){
        holdG--;
      }
      if(holdB<B&&holdB!=B){
        holdB++;
      } else if(holdB>B&&holdB!=B){
        holdB--;
      }
      pixels.fill(pixels.Color(holdR,holdG,holdB),0,pixels.numPixels());
      pixels.show();
      Serial.println(holdR);
      Serial.println(holdG);
      Serial.println(holdB);
      delay(10);
    }
  }
}
