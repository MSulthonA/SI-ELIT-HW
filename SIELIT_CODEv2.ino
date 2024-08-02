/*** Ini adalah Bagian kode utama dari SIELIT
Disini dijelaskan pemanggilan fungsi2 yang ada di masing-masing file tiap komponen
ada beberapa file komponen yang dipanggil mulai dari BATERRY_CONFIG.ino, EEPROM_CONFIG, 
LCD_CONFIG, LED_BUZZER, RFID-CONFIG, ROTARY_CONFIG, SPIFSS_CONFIG, dan WIFI_MQTT_CONFIG.

Kode ini menggunakan prinsip RTOS menggunakan 2 core dari ESP32:
Core 0 : taskkontrol
Core 1 : taskwifi
Kode ini menerapkan 2 mode:
- Mode Konfigurasi
- Mode Kontrol
Kode ini menghandling 2 kondisi : 
- Offline ( tidak terhubung atau hanya terhubung wifi )
- Online (terhubung ke server & wifi)
***/

// Setting Perangkat 
// Ganti nama dan topik tiap perangkat bila ingin memulai pengkodean perangkat tertentu
const char* namaPerangkat = "SIELIT_B";
const char* Topic = "PegonBacaanP";   // Untuk terhubung ke MQTT broker
const char* credential[14] = {"SAS", "123456789",                      // Pastikan bila mengubah ini, jumlah array sama dengan inisialisasi
                              "PPMBKI ASTRA 1", "bki12345678",
                              "PPMBKI ASTRA 2", "calonmenantuidaman",
                              "PPMBKI ASTRI 3", "",
                              "PPMBKI ASTRA 3", "1sampai9",
                              "PPMBKI ASTRI 1", "istrisolekha",
                              "PPMBKI ASTRI 2", "istrisolekha"
                            };
int len = *(&credential + 1) - credential; // Sebagai pangjang array credential


//End Of Settingan Perangkat

// Button merah untuk menggati mode
#define MENU_BUTTON 34 

// inisialisasi task
TaskHandle_t taskkontrol;
TaskHandle_t taskwifi;

// MODE ditriggrer menggunakan interrupt
bool MODE = false; // true : mode config, false : mode operation
bool LAST_MODE = MODE; // menyimpan riwayat mode
bool isConnect = false; // Apakah online atau offline
uint8_t KELAS = 2; // 0:PB 1:LB 2:CP 3:SR // Tidak dipakai

//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0;

uint32_t start_mil = 0; 
uint32_t start_req = 0; 

// Semaprore, fitur untuk menghindari Racing data pada LCD
SemaphoreHandle_t xMutex = NULL;
// Untuk Passing data antar task kontrol dan task wifi
QueueHandle_t queue;

char* reply;

// Keperluan testing
void getTest(){
  Serial.print("Heap : ");
  Serial.println(ESP.getHeapSize());
  Serial.print("FreeHeap : ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("CPU cycle : ");
  Serial.println(ESP.getCycleCount());
}

// Handling interupt dari button
void IRAM_ATTR ISR_changeMenu(){
    button_time = millis();
    if(button_time-last_button_time > 250){ //250
      if(MODE){
        MODE = false;
        reset_LCD_Config();
      }else{
        MODE = true;
      }
      last_button_time = button_time;
    }
}

// tempat inisialisasi tiap task dan beberapa tambahan
void setup() {
  start_mil = millis();
  Serial.begin(115200);
  getTest();
 
//  LCD_initt();
//  LCD_wait();

  xMutex = xSemaphoreCreateMutex();
  queue = xQueueCreate( 5, sizeof(String));
  
  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(task_kontrol, // Task function.
    "TaskKontrol", // name of task.
    10000, // Stack size of task
    NULL, // parameter of the task
    1, // priority of the task
    &taskkontrol, // Task handle to keep track of created task
    1); // pin task to core 1
  delay(1000);
  
  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(task_wifi, //Task function.
    "TaskWiFi", //name of task.
    10000, //Stack size of task
    NULL, //parameter of the task
    1, //priority of the task
    &taskwifi, //Task handle to keep track of created task
    0); //pin task to core 0
  Serial.print("FreeHeap : ");
  Serial.println(ESP.getFreeHeap());
  delay(1000);
  
}

//task_1code: blinks an LED every 1000 ms
void task_kontrol( void * pvParameters ){
  Serial.print( "task kontrol running on: core " );
  Serial.println( xPortGetCoreID() );
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
  BaseType_t qStatus;
  String messageTemp;
  char* offbuffer;
//  uint32_t start = millis();
  
  // Void setupnya RTOS: inisialisasi beberapan komponene
  SPIFFS_init();
  LCD_initt();
  LCD_wait();
  RFID_init();
  ROTARY_init();
  Battery_init();
  get_Battery();
  LED_BUZZER_init();
  pinMode(MENU_BUTTON, INPUT);
  attachInterrupt(MENU_BUTTON, ISR_changeMenu, RISING); // Init Button menu
  uint32_t start_publish = millis();
  
  //Void loop nya RTOS
  for(;;){
    changeMenu();
    switch(MODE){
    case true: // MODE Konfigurasi
      ROTARY_config();
      MENU_CONFIG();
      break;
    case false: // MODE OPERASI
      String datas = check_uid();
      if(isConnect){ // jika online
        if(datas!='\0'){
           BUZZER_successfull();
           MQTT_publish(datas);
//           vTaskDelay(1000);
        }
        // setiap 20 detik dicek kalau ada data di SPIFSS maka akan pusblish MQTT
        if((millis()-start_publish) >= 20000){
            //dibaca dulu
            offbuffer = readPresence();
            Serial.println(offbuffer);
            if(offbuffer!="Failed to open file for reading"){
              // --------------- memecah mqtt
                String section;
                for(int i=0; offbuffer[i]!='\0';i++){
                  if(offbuffer[i]== '\n') {
                    //publish data dari variabel kolom harus diubah dalam bentuk string
                    Serial.println("Memulai publish: ");
                    Serial.println(section);
                    MQTT_publish(section);
                    section = "";
                  }else{
                    section += offbuffer[i];
                  }
                }
              BUZZER_successfull();
              memset(offbuffer,'\0', sizeof(offbuffer));
              removePresence();
            }
            start_publish = millis();
        }
      }else{ // jika offline : menammbahkan data pembacaan RFID
        if(datas!='\0'){
            //ngirim ke spiffs
            char failMSG[10];
            datas.concat('\n');
            datas.toCharArray(failMSG, sizeof(failMSG));
            addPresence(failMSG);
            BUZZER_successfull();
            LCD_process(datas);
        }
      }
      // Menerima data dari task wifi untuk ditampilkan di LCD
      char token[45];
      if(xQueueReceive(queue, &messageTemp, xTicksToWait)== pdPASS){
        Serial.print("Pesan :");
        Serial.println(messageTemp);
        strcpy(token,messageTemp.c_str());
        LCD_reqmsg(token);
        getTest();
      }
//      // Belajar
//      offbuffer = readPresence();
//      Serial.println(offbuffer);
//      
//      if(millis()-start > 20000){
//       removePresence();
//       writePresence("");
//       memset(offbuffer,'\0', sizeof(offbuffer));
//       start = millis();
//      }
      break;
    }
//    vTaskDelay(2);
  }
}

// Task WIFI untuk mengurus pengiriman data menggunakan protokol MQTT
void task_wifi( void * pvParameters ){
  Serial.print( "task wifi running on: core " );
  Serial.println(xPortGetCoreID() );

  //Void setupnya RTOS
  EEPROM_init(); // EEPROM dipakai dikedua task tapi diinit ke sini karena Wifi butuh duluan
  MQTT_init();

  //Void loopnya RTOS
  for(;;){
    MQTT_config();
    vTaskDelay(100);
  }
}

// Nggak kepake
void loop(){
//Serial.print( " loop() is running on: Core " );
//Serial.println( xPortGetCoreID() );
//delay(1000);
}

//Fungsi handling LCD ketika ganti menu
void changeMenu(){
  if(MODE != LAST_MODE){
    if(MODE){
      LCD_CONFIG_MODE(0);
      BUZZER_beep();  
      Serial.println(MODE);
      Serial.println("----MODE KONFIGURASI----");
    }else{
      LCD_wait();
      BUZZER_beep();  
      Serial.println(MODE);
      Serial.println("----MODE OPERASI----"); 
    }
  }
  LAST_MODE = MODE;
}
