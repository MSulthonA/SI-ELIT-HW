// LCD dapat menampilkan tampilan konfig dan subkonfig didalamnya
// LCD dapat menampilkan splashscreen ketika power baru dihidupkan sudah
// LCD dapat menampilkan konfirmasi berhasil atau tidak berhasil (sudah cuma berhasil)
// LCD dapat menampilkan posisi ready atau waiting for action (sudah)

//LCD
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//LCD
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Variabel penentu Nomor menu konfig
uint8_t gcounter = 0;
uint8_t prev=1;
uint8_t max_count;

bool ip_switch = false;
uint8_t selected_ip;

//Menu
typedef enum {
  GET_STATUS,
  SET_SERVER,
  SET_VOLUME,
  SET_BRIGHTNESS,
  SET_BACK,
  ROOT,
} Select;

Select menu_select = ROOT;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void LCD_initt(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);//2000
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(40, 28);
  // Display static text
  display.println("SI ELIT");
  display.fillCircle(20, 34, 10, WHITE);
  //  LCD_init();
  display.display();
  delay(1000);//5000
}

void LCD_send(char datas[40]){
  display.stopscroll();
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(1, 0);
  display.println("- PRESENSI DITERIMA -");
  display.setCursor(31, 17);
  display.println("----...----");
  char kolom[20];
  memset(kolom, '\0', sizeof(kolom));
  uint8_t j=0;
  uint8_t k = 0;
  for(int i=0; datas[i]!='\0';i++){
    if(datas[i]== ',') {
      if(k==0){
        display.setCursor(64-(j*6/2),35);
        display.println(kolom);
      }else if(k==1){
        display.setCursor(64-(j*6/2), 26);// 26
        display.println(kolom);
      }
      j=0;k++;
      memset(kolom, '\0', sizeof(kolom));
    }else{
      kolom[j]=datas[i];
      j++;
    }
//  
  }
  display.setCursor(49, 44);
  display.println("19:31");
  display.setCursor(0, 53);
//  display.println("----...----");
  display.println("abcdefghijklmnopqrstuvwxyz");
  
  display.display();
  delay(500);
  display.startscrollleft(0x03, 0x0E);
  delay(1000);
  display.stopscroll();
  display.startscrollright(0x03, 0x0E);
  delay(2500);
  display.stopscroll();
  display.startscrollleft(0x03, 0x0E);
  delay(1000);
  display.stopscroll();
  delay(250);

  LCD_wait();
}

void LCD_process(String datas){
  if (xSemaphoreTake (xMutex, portMAX_DELAY)) {
    display.stopscroll();
    display.clearDisplay();
  
    display.setTextSize(1);
    display.setCursor(31, 17);
    display.println("----...----");
    display.setCursor(0, 0);
    display.println("- PRESENSI DIPROSES -");
    display.setCursor(10,26);
    display.println("Perangkat offline");
    display.setCursor(20,35);
    display.print("data ");
    display.print(datas); 
    
    display.setCursor(10,44);//10
    display.print("akan diproses ");
    display.print((char)1);display.print((char)1);display.print((char)1);
    display.setCursor(31, 53);
    display.println("----...----");
    
    display.display();
    xSemaphoreGive (xMutex);
  }
    delay(250);
    display.startscrollleft(0x03, 0x0E);
    delay(600);
    display.stopscroll();
    display.startscrollright(0x03, 0x0E);
    delay(1200);
    display.stopscroll();
    display.startscrollleft(0x03, 0x0E);
    delay(600);
    display.stopscroll();
    LCD_wait();
}

void LCD_reqmsg(char datas[40]){
  if (xSemaphoreTake (xMutex, portMAX_DELAY)) {
  display.stopscroll();
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(31, 17);
  display.println("----...----");
  char kolom[20];
  memset(kolom, '\0', sizeof(kolom));
  uint8_t j=0;
  uint8_t k = 0;
  bool isSuccess = false;
  for(int i=0; datas[i]!='\0';i++){
    if(datas[i]== ',') {
      if(k==0){ //command : res_att
      }
      if(k==1){ //status : 0 / 1
        if(strcmp(kolom,"1")==0) isSuccess = true;
      }
      uint8_t cursor_center = 64-(j*6/2);
      if(cursor_center < 0){
        cursor_center = 0;
      }
      if(isSuccess){
        if(k==2){ // UID
          display.setCursor(1, 0);
          display.println("- PRESENSI BERHASIL -");
          display.setCursor(cursor_center, 35);// 26
          display.println(kolom);
        }else if(k==3){ // nama
          display.setCursor(cursor_center, 26);// 26
          display.println(kolom);
        }else if(k==4){ // jam
          display.setCursor(cursor_center, 44);// 26
          display.println(kolom);
        }
      }else{
        if(k == 2){
          display.setCursor(7, 0);
          display.println("- PRESENSI GAGAL -");
          display.setCursor(52,26);
          display.println("Maaf,");
          display.setCursor(cursor_center,35);
          display.println(kolom); 
          display.setCursor(55,44);//10
          display.print((char)1);display.print((char)1);display.print((char)1);
        }
      }
//      Serial.println(kolom);
      j=0;k++;
      memset(kolom, '\0', sizeof(kolom));
    }else{
      kolom[j]=datas[i];
      j++;
    }
  }
//  
//  char * token=strtok(datas, ",");
//  int count = 1;
//  bool isSuccess=false;
//  while(token != NULL){
//    count++;
////    Serial.println(token);
//    token = strtok(NULL, ",");
//    if(count == 2){
//      Serial.print("status : ");
//      Serial.println(strcmp(token,"1"));
//      if(strcmp(token,"1")==0){//0
//        isSuccess=true;
//      }
//    }
//    if(isSuccess){
//      if(count == 3){ // NIM
//        display.setCursor(64-(sizeof(token)*6/2),35);
//        display.println(token);
//      }
//      if(count == 4){ // Nama
//        display.setCursor(64-(sizeof(token)*6/2),26);
//        display.println(token);        
//      }
//      if(count == 5){ // Jam
//        display.setCursor(64-(sizeof(token)*6/2),44);
//        display.println(token);        
//      }
//    }else{
//      if(count == 3){
//        display.setCursor(20,35);
//        display.println("Maaf,");
//        display.setCursor(64-(sizeof(token)*6/2),26);
//        display.println(token); 
//        display.setCursor(10,44);
//        display.println("Silahkan coba lagi");
//      }
//    }
//
//  }
  
//  if(datas=="res_att"){
//  display.stopscroll();
//  display.clearDisplay();
//  
//  display.setTextSize(1);
//  display.setCursor(1, 0);
//  display.println("-PERINGATAN PRESENSI-");
//  display.setCursor(31, 17);
//  display.println("----...----");
//  char kolom[20];
//  memset(kolom, '\0', sizeof(kolom));
//  uint8_t j=0;
//  uint8_t k = 0;
//  for(int i=0; datas[i]!='\0';i++){
//    if(datas[i]== ',') {
//      if(k==0){
//        display.setCursor(64-(j*6/2),35);
//        display.println(kolom);
//      }else if(k==1){
//        display.setCursor(64-(j*6/2), 26);// 26
//        display.println(kolom);
//      }
//      j=0;k++;
//      memset(kolom, '\0', sizeof(kolom));
//    }else{
//      kolom[j]=datas[i];
//      j++;
//    }
////  
//  }
//  display.setCursor(49, 44);
//  display.println("19:31");
  display.setCursor(31, 53);
  display.println("----...----");
//  display.println("abcdefghijklmnopqrstuvwxyz");
  
  display.display();
  
  xSemaphoreGive (xMutex);
  }
  delay(250);
  display.startscrollleft(0x03, 0x0E);
  delay(600);
  display.stopscroll();
  display.startscrollright(0x03, 0x0E);
  delay(1200);
  display.stopscroll();
  display.startscrollleft(0x03, 0x0E);
  delay(600);
  display.stopscroll();

  LCD_wait();
  
//  }
}

//
void LCD_wait(){ 
  // Invert and restore display, pausing in-between
  if (xSemaphoreTake (xMutex, portMAX_DELAY)) {
  display.stopscroll();
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 28);
  display.println("Silahkan");
  display.setTextSize(1);
  display.setCursor(17, 48);
  display.println("Sematkan Cocard");
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(24, 1);
  display.println("PPM BKI");
  display.display();

  display.startscrollright(0x02, 0x0D); // dimulai dari 0-15 
  xSemaphoreGive (xMutex);
  }
}

// bagian yang akan dipanggil terus menerus dalam LCD ketika mode konfigurasi
void MENU_CONFIG(){
  switch(menu_select){
    case GET_STATUS:
      if(prev!=gcounter){
        LCD_CONFIG_MODE1();
        prev = gcounter;
      }
      break;
    case SET_SERVER:
      if(prev!=gcounter){
        max_count = 5;
        if(ip_switch){
          Serial.print("IP_SWITCH : ");
          Serial.println(ip_switch);
          LCD_CONFIG_MODE2(selected_ip ,get_eeprom(selected_ip+2));
        }else{
          LCD_CONFIG_MODE2(gcounter,0);
//          selected_ip = gcounter;
        }
        prev = gcounter;
      }
      break;
    case SET_VOLUME:
      if(prev!=gcounter){
        LCD_CONFIG_MODE3(get_eeprom(0));
        prev = gcounter; 
      }
      break;
    case SET_BRIGHTNESS:
      if(prev!=gcounter){
        LCD_CONFIG_MODE4(get_eeprom(1));
        LED_Power();
        prev = gcounter;  
      }
      break;
    case SET_BACK:
      MODE = false;
      menu_select = ROOT;
      LCD_wait();
      break;
    case ROOT:
      if(prev!=gcounter){
        max_count = 5;
        LCD_CONFIG_MODE(gcounter);
        prev = gcounter;  
      }
      break;
  }
}

void reset_LCD_Config(){
  menu_select = ROOT;
}


void LCD_reconnect(){
  if (xSemaphoreTake (xMutex, portMAX_DELAY)) {
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 28);
  display.println("Connect..");

  display.display();
  xSemaphoreGive (xMutex);
  }
}

uint8_t LCD_CONFIG_MODE(uint8_t SELECTED){
  if (xSemaphoreTake (xMutex, portMAX_DELAY)) {
  display.stopscroll();
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40, 0);
  display.println("- Menu -");
  display.setCursor(5, 17);
  display.println("[1] Status");
  display.setCursor(5, 26);
  display.println("[2] IP SERVER");
  display.setCursor(5, 35);
  display.println("[3] Volume");
  display.setCursor(5, 44);
  display.println("[4] Brightness");
  display.setCursor(5, 53);
  display.println("[5] Back");
  switch(SELECTED){
    case GET_STATUS:
      display.setTextColor(BLACK, WHITE);
      display.setCursor(5, 17);
      display.println("[1] Status");
      break;
    case SET_SERVER:
      display.setTextColor(BLACK, WHITE);
      display.setCursor(5, 26);
      display.println("[2] IP Server");
      break;
    case SET_VOLUME:
      display.setTextColor(BLACK, WHITE);
      display.setCursor(5, 35);
      display.println("[3] Volume");
      break;
    case SET_BRIGHTNESS:
      display.setTextColor(BLACK, WHITE);
      display.setCursor(5, 44);
      display.println("[4] Brightness");
      break;
    case SET_BACK:
      display.setTextColor(BLACK, WHITE);
      display.setCursor(5, 53);
      display.println("[5] Back");
      break;
  }
  
  display.display();
  xSemaphoreGive (xMutex);
  }
}


void detail_config(){
    
}

void LCD_CONFIG_MODE1(){
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(37, 0);
  display.println("- Status -");
  display.setCursor(2, 17);
  display.print("Wifi : ");
  display.println(getSsid());
  display.setCursor(2, 26);
  display.println("ID_Device : 01");
  display.setCursor(2,35);
  display.println(get_Battery());
  display.setTextColor(BLACK, WHITE);
  display.setCursor(2, 53);
  display.println("[5]Back");
  display.display();
}

void LCD_CONFIG_MODE2(uint8_t SELECTED, uint8_t ip_oct){
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(42, 0);
  display.println("- IP SERVER -");
  // for loop untuk menampilkan wifi yang ada
  Serial.print("SELECTED : ");
  Serial.println(SELECTED);
  Serial.println("IP_OCT : ");
  Serial.println(ip_oct);
  
  display.setCursor(10, 26);
  display.print(get_ip(1));
  display.setCursor(30, 26);
  display.print('.');
  display.setCursor(39, 26);
  display.print(get_ip(2));
  display.setCursor(59, 26);
  display.print('.');
  display.setCursor(68, 26);
  display.print(get_ip(3));
  display.setCursor(88, 26);
  display.print('.');
  display.setCursor(95, 26);
  display.println(get_ip(4));
  
  display.setCursor(5, 53);
  display.println("[5] Back");
    

  switch(SELECTED){
    case 0: 
      display.setTextColor(BLACK, WHITE);
      //  display.setTextSize(1);
      display.setCursor(5, 53);
      display.println("[5] Back");
      break;
    case 1: 
      display.setTextColor(BLACK, WHITE);
      display.setCursor(10, 26);
      display.print(ip_oct);
      break;
    case 2: 
      display.setTextColor(BLACK, WHITE);
      display.setCursor(39, 26);
      display.print(ip_oct);
      break;
    case 3: 
      display.setTextColor(BLACK, WHITE);
      display.setCursor(68, 26);
      display.print(ip_oct);
      break;
    case 4: 
      display.setTextColor(BLACK, WHITE);
      display.setCursor(95, 26);
      display.print(ip_oct);
      break;
    
  }
  
  display.display();
}

uint8_t LCD_CONFIG_MODE3(uint8_t volume){
  
  display.clearDisplay();
  
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(37, 0);
  display.println("- Volume -");
  // tampilan tambah dan kurang volume dengan rectangle
  display.drawRoundRect(10, 17, 102, 9, 2, WHITE); // (kolom, baris, panjang, lebar, ketebalan, warna)
  display.fillRect(11, 18, volume, 7, WHITE); // (kolom, baris, panjang, lebar, warna) 
  display.setCursor(50, 28);
  display.print(volume);
  display.println(" %");
  display.setTextColor(BLACK, WHITE);
  display.setCursor(5, 53);
  display.println("[5] Back");
  display.display();
}


uint8_t LCD_CONFIG_MODE4(uint8_t brightness){
  
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(26, 0);
  display.println("- Brightness -");
  // tampilan tambah dan kurang brigtness dengan rectangle
  display.drawRoundRect(10, 17, 102, 9, 2, WHITE); // (kolom, baris, panjang, lebar, ketebalan, warna)
  display.fillRect(11, 18, brightness, 7, WHITE); // (kolom, baris, panjang, lebar, warna) 
  display.setCursor(50, 28);
  display.print(brightness);
  display.println(" %");
  display.setTextColor(BLACK, WHITE);
  display.setCursor(5, 53);
  display.println("[5] Back");
  display.display();
}
