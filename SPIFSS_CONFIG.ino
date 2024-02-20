#include "SPIFFS.h"
#include <string.h>

void SPIFFS_init(){
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
}

//-----------Pegon Bacaan---------------
char* readPB(){ // Eksekusi 4s
  uint32_t start = millis();
  // Buka File Pegon Bacaan
  File file_PB = SPIFFS.open("/pegon_bacaan.txt");
  if(!file_PB){
    Serial.println("Failed to open file for reading");
    return "Failed to open file for reading";
  }

  static char datas[300];
  uint32_t kolom = 0;
  uint8_t baris = 0;
  // while ini akan melakukan looping per character
  while(file_PB.available()){ 
    //membaca ID data ke 1-11 
    char now = file_PB.read();
    datas[kolom] = now;
    kolom++;
  }
  return datas;
  file_PB.close();
}

//-----------Lambatan---------------
char* readLB(){ // Eksekusi 4s
  uint32_t start = millis();
  // Buka File Lambatan
  File file_LB = SPIFFS.open("/lambatan.txt");
  if(!file_LB){
    Serial.println("Failed to open file for reading");
    return "Failed to open file for reading";
  }

  static char datas[300];
  uint32_t kolom = 0;
  uint8_t baris = 0;
  // while ini akan melakukan looping per character
  while(file_LB.available()){ 
    //membaca ID data ke 1-11 
    char now = file_LB.read();
    datas[kolom] = now;
    kolom++;
  }
  return datas;
  file_LB.close();
}

//-----------Cepatan---------------
char* readCP(){ // Eksekusi 4s
  uint32_t start = millis();
  // Buka File Lambatan
  File file_CP = SPIFFS.open("/cepatan.txt");
  if(!file_CP){
    Serial.println("Failed to open file for reading");
    return "Failed to open file for reading";
  }

  static char datas[300];
  uint32_t kolom = 0;
  uint8_t baris = 0;
  // while ini akan melakukan looping per character
  while(file_CP.available()){ 
    //membaca ID data ke 1-11 
    char now = file_CP.read();
    datas[kolom] = now;
    kolom++;
  }
  return datas;
  file_CP.close();
}

void addPresence(const char * message){
    Serial.printf("Appending to file: %s\r\n", "/offbuffer.txt");

    File file = SPIFFS.open("/offbuffer.txt", FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

char* readPresence(){
  Serial.printf("Reading file: %s\r\n", "/offbuffer.txt");
  
  File file = SPIFFS.open("/offbuffer.txt");
  if(!file || file.isDirectory()){
      Serial.println("- failed to open file for reading");
      return "Failed to open file for reading";
  }
  
  Serial.println("- read from file:");
  static char datas[300];
  uint32_t huruf = 0;
  // while ini akan melakukan looping per character
  while(file.available()){ 
    //membaca ID data ke 1-11 
    char now = file.read();
    datas[huruf] = now;
    huruf++;
  }
  return datas;
  file.close();
}

void writePresence(const char * message){
  Serial.printf("Writing file: %s\r\n", "/offbuffer.txt");
  File file = SPIFFS.open("/offbuffer.txt", FILE_WRITE);
  if(!file){
      Serial.println("- failed to open file for writing");
      return;
  }
  
  if(file.print(message)){
      Serial.println("- file written");
  } else {
      Serial.println("- write failed");
  }
  file.close();
}

void removePresence(){
  Serial.printf("Deleting file: %s\r\n", "/offbuffer.txt");
  if(SPIFFS.remove("/offbuffer.txt")){
      Serial.println("- file deleted");
  } else {
      Serial.println("- delete failed");
  }
}




//-----------Lambatan---------------
//char[][] readLB(){ // Eksekusi 4s
//  // Buka File Lambatan
//  File file_LB = SPIFFS.open("/lambatan.txt");
//  if(!file_LB){
//    Serial.println("Failed to open file for reading");
//    return;
//  }
//
//  char datas[10][40];
//  // while ini akan melakukan looping per character
//  uint8_t kolom = 0;
//  uint8_t baris = 0;
//  while(file_LB.available()){ 
//    //membaca ID data ke 1-11 
//    char now = file_LB.read();
////    if(now==','){
////      kolom++;i=0;
////    }else 
//    if(now=='\n'){
////      Serial.print(datas[baris][0]);
////      Serial.print(datas[baris][1]);
////      Serial.print(datas[baris][2]);
////      Serial.print(datas[0]);
////      Serial.print(datas[1]);
////      Serial.println(datas[2]);
////      memset(datas[0], '\0', sizeof(datas[0]));
////      memset(datas[1], '\0', sizeof(datas[1]));
////      memset(datas[2], '\0', sizeof(datas[2]));
//      baris++;kolom=0;
//    }else{
//      datas[baris][kolom]= now;
//      kolom++;
//    }
//    
//  }
////  Serial.println("");
//  return datas;
//  file_LB.close();
//}

//-----------Cepatan---------------
//void readCP(){ // Eksekusi 4s
//  // Buka File Cepatan
//  File file_CP = SPIFFS.open("/cepatan.txt");
//  if(!file_CP){
//    Serial.println("Failed to open file for reading");
//    return;
//  }
//
//  char datas[3][20];
//  // while ini akan melakukan looping per character
//  uint8_t i = 0;
//  uint8_t j = 0;
//  while(file_CP.available()){ 
//    char now = file_CP.read();
//    if(now==','){
//      i++;j=0;
//    }else if(now=='\n'){
////      Serial.print(datas[0]);
////      Serial.print(datas[1]);
////      Serial.println(datas[2]); 
////      check_credentials(datas);
//      memset(datas[0], '\0', sizeof(datas[0]));
//      memset(datas[1], '\0', sizeof(datas[1]));
//      memset(datas[2], '\0', sizeof(datas[2]));
//      i=0;j=0;
//    }else{
//      datas[i][j] = now;
//      j++;
//    }
//  }
////  Serial.println("");
//  file_CP.close();
//}
