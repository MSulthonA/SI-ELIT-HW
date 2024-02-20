
#define BATTERY_PIN 32
 
// Floats for resistor values in divider (in ohms)
float R1 = 30000.0;
float R2 = 7500.0; 
 
// Float for Reference Voltage
float ref_voltage = 4.8;



void Battery_init(){
  pinMode(BATTERY_PIN, INPUT);
}

float get_Battery(){
    // Floats for ADC voltage & Input voltage
    float in_voltage = 0.0;
    float adc_voltage = 0.0;

    int adc_battery;
  
    for(int i = 0; i<10;i++){
    adc_battery = analogRead(BATTERY_PIN);
    // Determine voltage at ADC input
    adc_voltage  = (adc_battery * ref_voltage) / 4096.0;
  
    // Calculate voltage at divider input
    in_voltage = in_voltage + adc_voltage*(R1+R2)/R2 - 0.62;
    Serial.println(in_voltage);  
  }
  Serial.print("Battery : ");
  in_voltage /= 10.0;
  Serial.println(in_voltage);
  return in_voltage;
}

//void get_Battery(){
//  Battery_Info();
//  
//}
