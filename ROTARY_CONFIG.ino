#include <ezButton.h>  // the library to use for SW pin

// Encoder without Noise Teme
#define CLK_PIN 33 // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DATA_PIN  25 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  26 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

// Variabel volume and brightness percentage
//uint8_t volume = 100;// 0-100
//uint8_t brightness = 100; // 0-100

//int counter = 0;
int direction = DIRECTION_CW;

//#define YLED A1

ezButton button(SW_PIN);



////////////////////////////////////////////////////
void ROTARY_init() {
   pinMode(CLK_PIN,INPUT);
   pinMode(DATA_PIN,INPUT);
   button.setDebounceTime(50); //50
//   pinMode(YLED,OUTPUT);
}

////////////////////////////////////////////////////
void ROTARY_config() {
  button.loop();  // MUST call the loop() function first
static uint16_t state=0,counter=0;

    delayMicroseconds(100); // Simulate doing somehing else as well.

    state=(state<<1) | digitalRead(CLK_PIN) | 0xe000;

    if (state==0xf000){
       state=0x0000;
       if(digitalRead(DATA_PIN)){
        counter++;
        if(counter==max_count){
          counter=0;
        }
        BUZZER_beep();
        direction = DIRECTION_CW;
       }
       else{
        counter--;
        if(counter==65535){
          counter=max_count-1;
        }
        BUZZER_beep();
        direction = DIRECTION_CCW;
       }
      Serial.print(direction);
      Serial.print(" : ");
      Serial.println(counter);
      gcounter = counter;
    }

    if (button.isPressed()) {
      Serial.println("The button is pressed");
      BUZZER_beep();  
      switch(menu_select){
      case GET_STATUS:
        menu_select = ROOT;
        break;
      case SET_SERVER:
        if(gcounter==0){
          menu_select = ROOT; // Ini untuk sementara
          ip_switch = false;
        }else{
          selected_ip = gcounter;
          if(ip_switch){
            ip_switch = false;
          }else {
            ip_switch = true; 
          }
        }
        break;
      case SET_VOLUME:
        menu_select = ROOT;
        break;
      case SET_BRIGHTNESS:
        menu_select = ROOT;
        break;
      case ROOT:
        menu_select = (Select)counter; 
        break;
      }
      counter=0;
      gcounter=0;
      prev=5;
    }
}

uint8_t get_ip(uint8_t number){
  if(number == 1){
    return EEPROM.readByte(IP_OCT1_ADDR);  
  }else if(number == 2){
    return EEPROM.readByte(IP_OCT2_ADDR);  
  }else if(number == 3){
    return EEPROM.readByte(IP_OCT3_ADDR);  
  }else if(number == 4){
    return EEPROM.readByte(IP_OCT4_ADDR);  
  }
  
}

uint8_t get_eeprom(uint8_t RCHOOSE){ // 0 : volume dan 1 : brightness , 3-6 : ip_octave
 
  if(RCHOOSE==1){// jika brightness yang dipilih
//    uint8_t brightness; // 0-100
    brightness = EEPROM.readByte(BRIGHTNESS_ADDR);
    if(direction){
      brightness = constrain(brightness + -5, 0, 100);
      EEPROM.writeByte(BRIGHTNESS_ADDR, brightness);
      EEPROM.commit();
      return brightness;    // jika DIRECTION_CW
    }
    brightness = constrain(brightness + 5, 0, 100);
    EEPROM.writeByte(BRIGHTNESS_ADDR, brightness);
    EEPROM.commit();
    return brightness; // jika DIRECTION_CCW
  }else if(RCHOOSE==0){
  // jika volume yang dipilih
  //  uint8_t volume;// 0-100
    volume = EEPROM.readByte(VOLUME_ADDR);
    
    if(direction){
      volume = constrain(volume + -5, 0, 100);
      map_volume = map(volume,0,100,0,255);
      EEPROM.writeByte(VOLUME_ADDR, volume);
      EEPROM.commit();
      return volume;    // jika DIRECTION_CW
    }
    volume = constrain(volume + 5, 0, 100);
    map_volume = map(volume,0,100,0,255);
    EEPROM.writeByte(VOLUME_ADDR, volume);
    EEPROM.commit();
    return volume; // jika DIRECTION_CCW
  }else if(RCHOOSE==3){
    ip_oct1 = EEPROM.readByte(IP_OCT1_ADDR);
    if(direction){
      ip_oct1--;
      EEPROM.writeByte(IP_OCT1_ADDR, ip_oct1);
      EEPROM.commit();
      return ip_oct1;
    }
    ip_oct1++;
    EEPROM.writeByte(IP_OCT1_ADDR, ip_oct1);
    EEPROM.commit();
    return ip_oct1;
  }else if(RCHOOSE==4){
    ip_oct2 = EEPROM.readByte(IP_OCT2_ADDR);
    if(direction){
      ip_oct2--;
      EEPROM.writeByte(IP_OCT2_ADDR, ip_oct2);
      EEPROM.commit();
      return ip_oct2;
    }
    ip_oct2++;
    EEPROM.writeByte(IP_OCT2_ADDR, ip_oct2);
    EEPROM.commit();
    return ip_oct2;
  }else if(RCHOOSE==5){
    ip_oct3 = EEPROM.readByte(IP_OCT3_ADDR);
    if(direction){
      ip_oct3--;
      EEPROM.writeByte(IP_OCT3_ADDR, ip_oct3);
      EEPROM.commit();
      return ip_oct3;
    }
    ip_oct3++;
    EEPROM.writeByte(IP_OCT3_ADDR, ip_oct3);
    EEPROM.commit();
    return ip_oct3;
  }else if(RCHOOSE==6){
    ip_oct4 = EEPROM.readByte(IP_OCT4_ADDR);
    if(direction){
      ip_oct4--;
      EEPROM.writeByte(IP_OCT4_ADDR, ip_oct4);
      EEPROM.commit();
      return ip_oct4;
    }
    ip_oct4++;
    EEPROM.writeByte(IP_OCT4_ADDR, ip_oct4);
    EEPROM.commit();
    return ip_oct4;
  }

}


//#include <ezButton.h>  // the library to use for SW pin
//
//#define CLK_PIN 25 // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
//#define DT_PIN  26 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
//#define SW_PIN  27 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin
//
//#define DIRECTION_CW  0   // clockwise direction
//#define DIRECTION_CCW 1  // counter-clockwise direction
//
//int counter = 0;
//int direction = DIRECTION_CW;
//int CLK_state;
//int prev_CLK_state;
//
//ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;
//
//void ROTARY_init() {
//  // configure encoder pins as inputs
//  pinMode(CLK_PIN, INPUT);
//  pinMode(DT_PIN, INPUT);
//  button.setDebounceTime(50);  // set debounce time to 50 milliseconds
//
//  // read the initial state of the rotary encoder's CLK pin
//  prev_CLK_state = digitalRead(CLK_PIN);
//}
//
//void ROTARY_config() {
//  button.loop();  // MUST call the loop() function first
//
//  // read the current state of the rotary encoder's CLK pin
//  CLK_state = digitalRead(CLK_PIN);
//
//  // If the state of CLK is changed, then pulse occurred
//  // React to only the rising edge (from LOW to HIGH) to avoid double count
//  if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
//    // if the DT state is HIGH
//    // the encoder is rotating in counter-clockwise direction => decrease the counter
//    if (digitalRead(DT_PIN) == HIGH) {
//      counter--;
//      if(counter==-1){
//        counter=4;
//      }
//      BUZZER_beep();
//      direction = DIRECTION_CCW;
//    } else {
//      // the encoder is rotating in clockwise direction => increase the counter
//      counter++;
//      if(counter==5){
//        counter=0;
//      }
//      BUZZER_beep();
//      direction = DIRECTION_CW;
//    }
//
//    Serial.print("Rotary Encoder:: direction: ");
//    if (direction == DIRECTION_CW)
//      Serial.print("Clockwise");
//    else
//      Serial.print("Counter-clockwise");
//
//    Serial.print(" - count: ");
//    Serial.println(counter);
//  }
//
//  // save last CLK state
//  prev_CLK_state = CLK_state;
//
//  if (button.isPressed()) {
//    Serial.println("The button is pressed");
//    menu_select = (Select)counter;
//    
////    
////    if(MODE){
//////      MODE=false;
//////      LCD_wait();
////      Serial.println("-----OPERATIONAL MODE-------");
////    }else{
//////      MODE=true;
//////      LCD_CONFIG_MODE();
////      Serial.println("-----CONFIG MODE-------");
////    }
//    
//  }
//}
