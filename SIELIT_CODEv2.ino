#define MENU_BUTTON 34

TaskHandle_t taskkontrol;
TaskHandle_t taskwifi;

// MODE ditriggrer menggunakan interrupt
bool MODE = true; // true : mode config, false : mode operation
bool LAST_MODE = MODE;
bool isConnect = false;
uint8_t KELAS = 2; // 0:PB 1:LB 2:CP 3:SR

//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;  
unsigned long last_button_time = 0;

SemaphoreHandle_t xMutex = NULL;

QueueHandle_t queue;


char* reply;

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

void setup() {
  Serial.begin(115200);
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
  
  //Tambahan
  SPIFFS_init();
  EEPROM_init();
  //LCD
  LCD_initt();
  LCD_wait();
  RFID_init();
  ROTARY_init();
  Battery_init();
  get_Battery();
  LED_BUZZER_init();
  pinMode(MENU_BUTTON, INPUT);
  attachInterrupt(MENU_BUTTON, ISR_changeMenu, RISING);
  uint32_t start_publish = millis();
  
  
  for(;;){
    changeMenu();
    switch(MODE){
    case true: // MODE 
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
        // setiap 20 detik dicek
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
      }else{ // jika offline
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
      char token[40];
      if(xQueueReceive(queue, &messageTemp, xTicksToWait)== pdPASS){
        Serial.print("Pesan :");
        Serial.println(messageTemp);
        strcpy(token,messageTemp.c_str());
        LCD_reqmsg(token);
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


void task_wifi( void * pvParameters ){
  Serial.print( "task wifi running on: core " );
  Serial.println(xPortGetCoreID() );

  //Tambahan
  MQTT_init();
  
  for(;;){
    MQTT_config();
    vTaskDelay(100);
  }
}

void loop(){
//Serial.print( " loop() is running on: Core " );
//Serial.println( xPortGetCoreID() );
//delay(1000);
}

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
