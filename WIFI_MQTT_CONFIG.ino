#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiMulti.h>
//#include <ESP8266WiFi.h>

//const char* ssid = "oo";
//const char* password = "12345678";

//const char* ssid = "A7";
//const char* password = "12345678";

const char* Topic = "PegonBacaanP";

//const char* mqtt_server = "192.168.214.235";

WiFiClient espClient;
PubSubClient client(espClient);
WiFiMulti wifiMulti;

void MQTT_init() 
{
  Serial.println();

  wifiMulti.addAP("PPMBKI ASTRA 1", "bki12345678");
  wifiMulti.addAP("PPMBKI ASTRA 2", "calonmenantuidaman");
  wifiMulti.addAP("PPMBKI ASTRI 3", "");
//  wifiMulti.addAP("PPMBKI ASTRA 3", "1sampai9");
  wifiMulti.addAP("PPMBKI ASTRI 1", "istrisolekha");
//  wifiMulti.addAP("PPMBKI ASTRI 2", "istrisolekha");
  wifiMulti.addAP("SAS", "123456789");
  
  // Menyambungkan ke WiFi
//  WiFi.mode(WIFI_STA); //esp8266
//  WiFi.begin(ssid, password);



//  uint32_t start_time = millis();
  wifiMulti.run();
  if(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
//    if((millis()-start_time) >= 15000) ESP.restart();
  }
//  Serial.println();
//  Serial.println("WiFi connected");
//  Serial.println("IP address : ");
//  Serial.println(WiFi.localIP());
//  Serial.println(WiFi.SSID());
//  LED_WifiConnect();
//
  call_MQTT();
}

void call_MQTT(){
  String mqtt_server;
  mqtt_server = "" + (String)get_ip(1)+'.'+ (String)get_ip(2)+'.'+ (String)get_ip(3)+'.'+ (String)get_ip(4);
//  mqtt_server = "192.168.214.235";
  Serial.println(mqtt_server);
//  char* mqtt = mqtt_server.c_str();
  int length = mqtt_server.length();
  char* mqtt = new char[length+1];
  strcpy(mqtt, mqtt_server.c_str());
  Serial.println(mqtt);
    // Menge-set Server MQTT dan portnya dan fungsi callback 
  client.setServer(mqtt, 1883);
  client.setCallback(callback);
  delay(200);
}

void MQTT_config() 
{
  client.loop();
  if(!client.connected()){
//    int element = 1;
//    xQueueSend(queue, &element, portMAX_DELAY);
//    Serial.println("Ngirim 1");
//  LCD_Reconnect();
    isConnect = false;
    MQTT_reconnect();
  }
  LED_Connect();
} 

void MQTT_publish(String datas){
  // seperti loop_forever pada python untuk terus menerima pesan sebagai client dari publisher
//  client.loop();
  // sebagai PUBLISHER mengirimkan pesan ke subscriber(pesan dikirim dalam bentuk byte/char Array)
  String strMSG = "req_att,";
  strMSG.concat(datas);
  int arrlen = strMSG.length() + 1;
  char chrMSG[100];
  strMSG.toCharArray(chrMSG, sizeof(chrMSG));
//  delay(2000);
  client.publish(Topic, chrMSG);

  //jika gagal saja
//  if(gagal){
//  char failMSG[10];
//  datas.concat('\n');
//  datas.toCharArray(failMSG, sizeof(failMSG));
//  addPresence(failMSG);

//  client.publish("device1", chrMSG);
//  delay(250);
}

// CALLBACK : secara otomatis ketika menerima pesan dari publisher maka akan mengerjakan fungsi berikut
// topic digunakan untuk menentukan siapa subscriber yang akan dikirimi
// message berupa pesan yang diterima oleh ESP32 dalam format byte array 
// length berupa jumlah atau panjang dari bytearray
void callback(char* topic, byte* message, unsigned int length){
  String messageTemp;
  char token[40];
  memset(token, '\0', sizeof(token));
  for(int i=0; i < length; i++){
    messageTemp += (char)message[i];
//    if(i<40)token[i] = (char)message[i];
  }
  Serial.println(String(topic));
  
//  if(String(topic) == "IoT_Command"){
  BaseType_t qStatus;
  if(String(topic) == Topic){
    
//    if((messageTemp) == )
    Serial.print("Pesan : ");
    Serial.println(messageTemp);
    strcpy(token,messageTemp.c_str());
    // do something
    if(strcmp(strtok((char*)message,","),"res_att")==0){
      BUZZER_successfull();
      qStatus = xQueueSend(queue, &messageTemp, 0);//portMAX_DELAY
      vTaskDelay(2250);
//      Serial.println("Nerimo token");
//      delay(50);
//      Serial.println((char*)message);
//      Serial.println(token);
//      memset(message, '\0', sizeof(message));
    }
  }
//  if(String(topic) == "WillMSG") {
//    Serial.print("WillMSG : ");
//    Serial.print((char*)message);
//    call_MQTT();
//  }
    
}

//RECONNECT : apabila koneksi terputus maka secara terus menerus akan menyambungkan kembali sampai tersambung
void MQTT_reconnect(){
  while (!client.connected()){
    // Connect WIFI
    if(WiFi.status()!=WL_CONNECTED){
      uint32_t start_time = millis();
      LED_WifiDisconnect();
//      WiFi.begin(ssid, password);
      while(WiFi.status()!=WL_CONNECTED){
        wifiMulti.run();
        delay(500);
        Serial.print("connect wifi...");
        if((millis()-start_time) >= 20000) ESP.restart();
      }
    }
    LED_WifiConnect();
    
    //Connect Server
    Serial.print("Attempting MQTT connection....");
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.println(WiFi.macAddress());

//    String client_id= "ESP32Client-";
//    client_id += String(random(0xffff), HEX);
      
    if(client.connect("ESP8266Client2",NULL,NULL,Topic, 1, 1,"disconnect")){ // client_id.c_str() //ESP8266Client
      Serial.println("connected");
      BUZZER_beep();
//      LCD_wait();
      client.subscribe(Topic);
      client.publish(Topic, "ping_reply");
      isConnect = true;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

String getSsid(){
  return WiFi.SSID();
}
