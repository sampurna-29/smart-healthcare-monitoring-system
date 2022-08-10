 
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define REPORTING_PERIOD_MS     1000
PulseOximeter pox;
uint32_t tsLastReport = 0;
LiquidCrystal_I2C lcd(0x23, 16, 2);
#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


// repace your wifi username and password
const char* Host_Name     = "A";
const char* Password = "12345678";
unsigned long myChannelNumber = 1721858;
const char * apiKey = "2DIGT29RIKB8FB9Y";

// variables
float cel=0.00, ecg=0.00;
float HeartRate = 0.00, SpO2 = 0.00;
char latbuf[20],TEMP[20],ecg2[20],lanbuf[20],roomtemp[20],aqi[20];
int j=0;
int aq=0;
float a,b;
float hr=0;
float spo =0;
int i = 15;
float c = 0;
 int m=70;
float man=0;
const int lm35_pin = 1;  
 int temp_adc_val;
float temp_val;
float sensorValue;

void setup()
{ 
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Smart HealthCare");
  lcd.setCursor(0,1);
  lcd.print("Monitoring System"); 
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Put Finger");
  lcd.setCursor(0,1);
  lcd.print("on Sensor"); 
  delay(2000);
  lcd.clear();
  Serial.begin(115200);
  pinMode(10, INPUT); // Setup for leads off detection LO +
   pinMode(2,INPUT);//AQI
     pinMode(1,INPUT);//temp
  pinMode(6,INPUT);//Switchpox
  pinMode(7,INPUT);//Switchecg
  pinMode(11, INPUT); // Setup for leads off detection LO -
  Serial.print("Initializing...");
  sensors.begin();
  sensors.setWaitForConversion(false);
  // Initialize the PulseOximeter instance
  if (!pox.begin()) {
    Serial.println("MAX30100 was not found. Please check the wiring/power.");
    lcd.print("FAILED");
    for (;;);
  } else {
    Serial.println("SUCCESS");
    Serial.println("AT+RST");               // Resetting ESP8266            
    char inv ='"';
    String cmd = "AT+CWJAP";
   cmd+= "=";
   cmd+= inv;
   cmd+= Host_Name;
   cmd+= inv;
   cmd+= ",";
   cmd+= inv;
   cmd+= Password;
   cmd+= inv;
   Serial.println(cmd);
   sensorValue = analogRead(2);
   aq= sensorValue;
  }
}
void loop()
{ 
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      a = pox.getHeartRate();
      b = pox.getSpO2();
      sensors.requestTemperatures();
      cel=sensors.getTempCByIndex(0);
      sensorValue = analogRead(2);
      if (m>0)
      {    sensorValue = analogRead(2);
           man=(man+sensorValue)/2;
           m=m-1;
      }
      if(m==0)
      {aq=man;
         m=70;
         }
          if( a <=45 || b == 0)
            {  lcd.clear(); 
               lcd.setCursor(10,0);
                lcd.print("Temp:");
                lcd.setCursor(10,1);
                 lcd.print(cel);
                 lcd.print("C");
                 lcd.setCursor(0,0);
                 lcd.print("AQI:");
                 lcd.setCursor(0,1);
                 lcd.print(aq);
                 if ( aq> 200)
                  { Serial.print("AQI index is high:");
                    Serial.println(aq);
                     senddata();
                     return;
                   }
                 i=15;
                return;
            }
              if (i>0)
              {
                c = ((c+a)/2);
                i = i-1;
              lcd.clear();
               lcd.setCursor(0,0);
                 lcd.print("measuring");
                 lcd.setCursor(0,1);
                   lcd.print(i); 
              }
             if (i==0)
              {
                i=15;
                Serial.print("Heart rate: ");
                Serial.print(c);
                Serial.print(" bpm & SpO2: ");
                Serial.print(b);
                Serial.println("%");
                Serial.print("BODY Temperature = ");
                Serial.println(cel);
                Serial.print("Room Temperature = ");
                Serial.print(temp_val);
                Serial.print(" Degree Celsius\n");
                if (aq > 100)
                  { Serial.print("AQI index is high:");
                    Serial.println(aq);
                     
                   }
               else
                   { Serial.print("AQI index is good: ");
                     Serial.println(aq);
                     }       
                hr=c;
                spo=b;
                c=0; 
                }
                 
  if(hr!= 0 || spo!= 0)
   { shoow();
     pox.begin();
   }   
   temp_adc_val = analogRead(1);  /* Read Temperature */
   temp_val = (temp_adc_val * 4.88);
   temp_val = (temp_val/10); /* LM35 gives output of 10mv/°C */
  tsLastReport = millis();
  }
   if (digitalRead(6)==HIGH)
  {
    Serial.println("yes");
    senddata();
  }
  if (digitalRead(7)==HIGH)  
  {   
    lcd.clear();
    Serial.println("yes");
    ecgsenddata();
  }
  }
void senddata()
{ dtostrf(HeartRate, 4,1, latbuf);
    dtostrf(SpO2, 4, 1, lanbuf);
    dtostrf(cel, 4, 1, TEMP);
    dtostrf(temp_val, 4, 1, roomtemp);
    dtostrf(aq, 2, 0, aqi);
    Serial.print("Heart rate: ");
    Serial.println(HeartRate);
    Serial.print(" bpm & SpO2: ");
    Serial.print(SpO2);
    Serial.println("%");
    Serial.print("BODY Temperature = ");
    Serial.println(TEMP);
    Serial.print("Room Temperature = ");
    Serial.print(roomtemp);
    Serial.print(" Degree Celsius\n");
    Serial.print("Air Quality Index:");
    Serial.println(aqi);
  String cmd = "AT+CIPSTART=\"TCP\",\"";          // Establishing TCP connection
  cmd += "184.106.153.149";                       // api.thingspeak.com
  cmd += "\",80";                                 // port 80
  Serial.println(cmd);
  if(Serial.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  String getStr = "GET /update?api_key=";         // prepare GET string
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(latbuf);                       
  getStr +="&field2=";
  getStr += String(lanbuf);
  getStr +="&field3=";
  getStr += String(TEMP); 
  getStr +="&field6=";
  getStr += String(roomtemp);
  getStr +="&field7=";
  getStr += String(aqi);                     
  getStr += "\r\n\r\n\r\n\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());                // Total Length of data
  Serial.println(cmd);

  if(Serial.find(">"))
  {
    //ser.print(getStr);
    Serial.print(getStr);
  }
  else
  {
    //ser.println("AT+CIPCLOSE");                  // closing connection
    Serial.println("AT+CIPCLOSE");
  }
   pox.begin();
}
void ecgsenddata()
{ for(j=0;j<60;j++)
   {
     if((digitalRead(10) == 1)||(digitalRead(11) == 1)){
      Serial.println('!');
    //lcd.print("!");
     }
  else{
      // send the value of analog input 0:
      ecg=analogRead(A0);
      Serial.println(ecg);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ECG Mode");
      lcd.setCursor(0,1);
      lcd.print(ecg);
      delay(100);
    }
    dtostrf(ecg, 4, 2, ecg2);
    Serial.print(ecg2);
     String cmd = "AT+CIPSTART=\"TCP\",\"";          // Establishing TCP connection
  cmd += "184.106.153.149";                       // api.thingspeak.com
  cmd += "\",80";                                 // port 80
  Serial.println(cmd);
  if(Serial.find("Error"))
  {
    Serial.println("AT+CIPSTART error");
    return;
  }
  String getStr = "GET /update?api_key=";         // prepare GET string
  getStr += apiKey;
  getStr +="&field4=";
  getStr += String(ecg2);                       
  getStr += "\r\n";
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());                // Total Length of data
  Serial.println(cmd);

  if(Serial.find(">"))
  {
    //ser.print(getStr);
    Serial.print(getStr);
  }
  else
  {
    //ser.println("AT+CIPCLOSE");                  // closing connection
    Serial.println("AT+CIPCLOSE");
  }
  delay(1000);
}
pox.begin();
}
void shoow()
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HR:");
    lcd.print(hr);
    lcd.setCursor(0,1);
    lcd.print("O2:");
    lcd.print(spo);
    lcd.print("%");
    sensors.requestTemperatures();
    cel=sensors.getTempCByIndex(0);
    lcd.setCursor(10,0);
    lcd.print("Temp:");
    lcd.setCursor(10,1);
    lcd.print(cel);
    lcd.print("C");
    delay(2000);
    HeartRate=hr;
    SpO2=spo;
    hr=0;spo=0; 
  pox.begin();
      
}
