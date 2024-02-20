#define SS_PIN 17  // 21 //D2 4
#define RST_PIN 16 // 22 //D1 5

#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void RFID_init(){
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
}

char* check_credentials(char uid[]){//
  uint8_t statuss = 0;
  static char datas[40];
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return '\0';
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return '\0';
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
//  Serial.println(uid);
  uint8_t j = 0;
  //datas hasilnya adalah data credentials saat itu
  for(int i=0; uid[i] != '\0';i++){
    if(uid[i]=='\n' || uid[i]=='\0'){
      if(statuss==1){
        static char temp[sizeof(datas)];
        Serial.println(datas);
        memcpy(temp, datas, sizeof(datas));
        memset(datas, '\0', sizeof(datas));
        Serial.println(temp);
        return temp;
      }
      memset(datas, '\0', sizeof(datas)); // ketika sudah sampai pojok, data dihapus
      j=0;
    }else{
      datas[j]= uid[i];
      if(j==10){
        if (content.substring(1) == datas) // "31 21 D0 26"
        {
          Serial.println("Access Granted ");
          Serial.println(content.substring(1));
          Serial.println("Welcome Santri ");
          statuss = 1;
        }
      }
      j++;
    }
  }
  if(statuss==0){
    Serial.println(" Access Denied ");
    delay(1000);
    return '\0';
  }
  else{
    static char temp[sizeof(datas)];
    memcpy(temp, datas, sizeof(datas));
    memset(datas, '\0', sizeof(datas));
    return temp;
  }
}






// =======================================CHECK UID=============================================


String check_uid(){//
  uint8_t statuss = 0;
  static char datas[10];
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return "\0";
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return "\0";
  }
  //Show UID on serial monitor
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
//  Serial.println(uid);
  //datas hasilnya adalah data credentials saat itu
  return content;
}

/*
// Ini Kode kedua referensi testing(work)
char* check_credentials(char uid[]){//
  bool statuss = false;
//  uint32_t start = millis();
  static char datas[40];
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return '\0';
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return '\0';
  }
  //Show UID on serial monitor
//  Serial.println();
//  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
//  Serial.print("==");
//  Serial.println(uid);
//  delay(5000);
  uint8_t j = 0;
  //datas hasilnya adalah data credentials saat itu
  for(int i=0; uid[i] != '\0';i++){
//    if(uid[i]==','){
////      kolom++;j=0;
//    }else 
    if(uid[i]=='\n'){
//      Serial.print(datas);
//      Serial.print("  ");
      if(statuss){
        return datas;
      }
      memset(datas, '\0', sizeof(datas)); // ketika sudah sampai pojok, data dihapus
      j=0;
    }else{
      datas[j]= uid[i];
      if(j==10){
        if (content.substring(1) == datas) //uid[0] "31 21 D0 26"//change UID of the card that you want to give access
        {
          Serial.println(" Access Granted ");
          Serial.println(content.substring(1));
          Serial.print(" Welcome Santri ");
//          delay(1000);
      //    Serial.println(uid[0]);
      //    Serial.println(" Semangat ");
          statuss = true;
      //    uint32_t end = millis()-start;
      //    Serial.print(" Time Consume : ");
      //    Serial.println(end);
        }
      }
      j++;
    }
//    else if(now=='\n'){ 
////      check_credentials(datas);
//      memset(datas[0], '\0', sizeof(datas[0]));
//      memset(datas[1], '\0', sizeof(datas[1]));
//      memset(datas[2], '\0', sizeof(datas[2]));
//      i=0;j=0;
//    }else{
//      datas[i][j] = now;
//      j++;
//    }
  }
      Serial.println(" Access Denied ");
      delay(1000);
  return '\0';
}
*/
