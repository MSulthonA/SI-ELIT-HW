// EEPROM dapat menyimpan data santri dalam UIDnya dan memanggilnya untuk 
//tiap indeks yang berbeda(dibuatin array atau object)
// EEPROM dapat menyimpan volume, kecerahan, dalam byte dan status perangkat berupa 
//id, wifi yang sudah tersambung, dan status baterai

#include "EEPROM.h"

uint8_t volume;
uint8_t brightness; // 0-100
uint8_t ip_oct1;
uint8_t ip_oct2;
uint8_t ip_oct3;
uint8_t ip_oct4;
uint8_t wifi;

// Alamat dari tiap-tiap konstanta yang disimpan di EEPROM
#define VOLUME_ADDR 0 //0-7
#define BRIGHTNESS_ADDR 8 //8-15
#define IP_OCT1_ADDR 16 //
#define IP_OCT2_ADDR 24 //
#define IP_OCT3_ADDR 32 //
#define IP_OCT4_ADDR 40 //
#define WIFI_ADDR 48 //

void EEPROM_init(){
  if (!EEPROM.begin(1000)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }
}
