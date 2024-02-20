// LED dapat menampilkan status koneksi di perangkat jadi tidak usah dilihatin di LCD
// LED dan BUZZER dapat memberi peringatan ketika koneksi jaringan tersambung atau terputus, dan 
//pengiriman sukses



#define LED_POWER 14 //LED untuk verifikasi bahwa sudah terkirim
#define LED_CONNECT 12 // LED untuk menunjukan bahwa perangkat telah terkonek
#define BUZZER 13 // Alarm untuk menunjukan bahwa 1. Terkirirm dan gagal kririm 2. Inisialisasi diawal 3. Terkoneksi(connected) atau terputus (disconnected)
#define PWM1_Ch    0
#define PWM1_Res   8
#define PWM1_Freq  800

#define PWM2_Ch    2
#define PWM3_Ch   3
#define PWM2_Freq  1100
#define PWM2_Res   8

#include <Arduino.h>

bool led_ch = false;

long led_start=0;

bool connection;

uint8_t map_volume;

uint8_t map_brightness;

void LED_BUZZER_init(){
//  pinMode(LED_POWER, OUTPUT);
//  pinMode(LED_CONNECT, OUTPUT);
  
  ledcAttachPin(BUZZER, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
//  delay(100);
  ledcAttachPin(LED_POWER,PWM2_Ch);
  ledcAttachPin(LED_CONNECT,PWM3_Ch);
  ledcSetup(PWM2_Ch, PWM2_Freq, PWM2_Res);
  ledcSetup(PWM3_Ch, PWM2_Freq, PWM2_Res);
  
  connection = false;
  brightness = EEPROM.readByte(BRIGHTNESS_ADDR);
  map_brightness = map(brightness,0,100,0,255);
  
  volume = EEPROM.readByte(VOLUME_ADDR);
  map_volume = map(volume,0,100,0,255);
  //keadaan awal
//  analogWrite(BUZZER, 10);
  ledcWrite(PWM1_Ch, map_volume);
//  digitalWrite(LED_CONNECT, HIGH);
//  digitalWrite(LED_VER, HIGH);
  delay(300);                       
  ledcWrite(PWM1_Ch, 0);
//  digitalWrite(LED_CONNECT, LOW);
//  digitalWrite(LED_VER, LOW);  
  delay(50);
  ledcWrite(PWM1_Ch, map_volume);
//  digitalWrite(LED_CONNECT, HIGH);
//  digitalWrite(LED_VER, HIGH);
  delay(50);                       
//  analogWrite(BUZZER, 0);
  ledcWrite(PWM1_Ch, 0);  
//  digitalWrite(LED_CONNECT, LOW);
//  digitalWrite(LED_VER, LOW);  
  delay(50);
    ledcWrite(PWM1_Ch, map_volume);
//  digitalWrite(LED_CONNECT, HIGH);
//  digitalWrite(LED_VER, HIGH);
  delay(700);                       
  ledcWrite(PWM1_Ch, 0); 
//  digitalWrite(LED_CONNECT, LOW);
//  digitalWrite(LED_VER, LOW);  
  delay(700);
  LED_Power();
}

void LED_Power(){
//  digitalWrite(LED_POWER, HIGH);
  ledcWrite(PWM2_Ch, map_brightness) ;
//  analogWrite(LED_POWER, map_brightness);
}

void LED_WifiConnect(){
//  Serial.println("ledcon");
//  digitalWrite(LED_CONNECT, HIGH);
  ledcWrite(PWM3_Ch, map_brightness);
}
void LED_WifiDisconnect(){
//  Serial.println("ledcon");
//  digitalWrite(LED_CONNECT, LOW);
  ledcWrite(PWM3_Ch, 0) ;
}

void LED_Connect(){
//  Serial.println("ledcon");
  if((millis()- led_start) >= 500){
    if(connection){
//      digitalWrite(LED_CONNECT, HIGH);
      ledcWrite(PWM3_Ch, map_brightness);
      connection = false;
    }else{
//      digitalWrite(LED_CONNECT, LOW);
      ledcWrite(PWM3_Ch, 0);
      connection = true;
    }
    
    led_start = millis();
  }
  
}

void BUZZER_successfull(){
    ledcWrite(PWM1_Ch, 0); 
    delay(100);
    ledcWrite(PWM1_Ch, 0);
    delay(100);
    ledcWrite(PWM1_Ch, map_volume);
    delay(50);
    ledcWrite(PWM1_Ch, 0);
    delay(50);
    ledcWrite(PWM1_Ch, map_volume);
    delay(50);
    ledcWrite(PWM1_Ch, 0);
    delay(50);
//  if(digitalRead(LED_VER) == HIGH ){
//    
//  }
}

void BUZZER_beep(){
  ledcWrite(PWM1_Ch, map_volume);
  delay(20);
  ledcWrite(PWM1_Ch, 0);
  delay(20);
}

void BUZZER_connect(){
//  digitalWrite(LED_CONNECT, HIGH);
  ledcWrite(PWM1_Ch, map_volume);
  delay(100);
  ledcWrite(PWM1_Ch, 0);
  delay(100);
  ledcWrite(PWM1_Ch, map_volume);
  delay(100);
  ledcWrite(PWM1_Ch, 0);
}

void BUZZER_disconnect(){
//  digitalWrite(LED_CONNECT, LOW);
  ledcWrite(PWM1_Ch, map_volume);
  delay(100);
  ledcWrite(PWM1_Ch, 0);
  delay(100);
  ledcWrite(PWM1_Ch, map_volume);
  delay(100);
  ledcWrite(PWM1_Ch, 0);
}
