#define BLYNK_TEMPLATE_ID "TMPL61MPbaMrj"
#define BLYNK_TEMPLATE_NAME "DATN26"
#define BLYNK_AUTH_TOKEN "Z_vpD3EqnPWzRfW9N8UrKGp1Cck5N7OB"

#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_SHT31.h>

#define LORA_SS   5     
#define LORA_RST  -1    
#define LORA_DIO0 13   

float temp,hum,temp2,hum2,NCBND1 = 35.0, NCBND2 = 26.0, NCBDA1 = 60.0, NCBDA2 = 40.0 ;
int AS, ASpercent, DAD, DADpercent,ASpercent2,DADpercent2,DDCDL=0;
volatile bool KT = false, received = false, rxDoneFlag=false;

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Adafruit_SHT31 sht31 = Adafruit_SHT31();

void onReceive(int packetSize) 
{
  if (packetSize) 
  {
    rxDoneFlag = true;
  }
}
int readTBADC(int pin) 
{
  long sum = 0;
  for (int i = 0; i < 10; i++) 
  {
    sum += analogRead(pin);
    delay(2); 
  }
  return sum / 10; 
}
BLYNK_WRITE(V8)
{
  NCBND1 = param.asFloat();
}
BLYNK_WRITE(V9)
{
  NCBDA1 = param.asFloat();
}
BLYNK_WRITE(V10)
{
  NCBND2 = param.asFloat();
}
BLYNK_WRITE(V11)
{
  NCBDA2 = param.asFloat();
}
void setup() 
{
  delay(1000);
  pinMode(36,INPUT);
  pinMode(39,INPUT);
  pinMode(26,OUTPUT);
  pinMode(LORA_DIO0, INPUT);
  
  digitalWrite(26,0);
  Serial.begin(115200);
  Wire.begin(21, 22, 100000);

  sht31.begin(0x44);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.setCursor(0, 18);
  display.print("Dang ket noi wifi tu dong");

  WiFiManager wm;
  wm.setConnectTimeout(30);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Ket noi wifi tu dong that bai");
  display.setCursor(0, 18);
  display.print("Vui long ket noi wifi thu cong tai dia chi 192.168.4.1");

  if (!wm.autoConnect("ESP32_WiFi_Config", "26262626")) 
  {
      delay(3000);
      ESP.restart();
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Da ket noi wifi");
  display.setCursor(0, 18);
  display.print("Dang gui tin hieu khoi dong den node 2");
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(); 
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V9);
  Blynk.syncVirtual(V10);
  Blynk.syncVirtual(V11);
  
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  LoRa.begin(433E6); 
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSpreadingFactor(12);       
  LoRa.setCodingRate4(8);         
  LoRa.enableCrc(); 
  LoRa.setSyncWord(0x12);
  LoRa.setTxPower(17);  
  LoRa.setOCP(100);
  while (!KT) 
  {
    LoRa.beginPacket();
    LoRa.print("hello");
    LoRa.endPacket();
    LoRa.receive(); 

    long startWait = millis();
    while (millis() - startWait < 3000) 
    {
      int DDC = LoRa.parsePacket();
      if (DDC) 
      {
        String message = "";
        while (LoRa.available()) {
          message += (char)LoRa.read();
      }
      if (message.indexOf("hi") >= 0) 
      {    
        KT = true;
        break;
      }
      }
      delay(10);
    }
  }
  LoRa.onReceive(onReceive); 
  LoRa.receive(); 
}
void loop() 
{
  Blynk.run();
  temp = sht31.readTemperature();
  hum  = sht31.readHumidity();
  AS = readTBADC(36);
  ASpercent = map(AS, 4095, 0, 0, 100); 
  DAD = readTBADC(39);
  DADpercent = map(DAD, 3300, 450, 0, 100); 

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Cd anh sang:");
  display.print(ASpercent, 1);
  display.print(" %");

  display.setCursor(0, 9);
  display.print("Do am dat: ");
  display.print(DADpercent, 1);
  display.print(" %");

  display.setCursor(0, 18);
  display.print("Nhiet do: ");
  display.print(temp, 1);
  display.print(" C");

  display.setCursor(0, 27);
  display.print("Do am khong khi:");
  display.print(hum, 1);
  display.print(" %");
  display.display();
  delay(500);

  LoRa.beginPacket();
  LoRa.print("ts");
  LoRa.endPacket();
  LoRa.receive();
  long startWait = millis();
  while (millis() - startWait < 1000) {}
  if (rxDoneFlag) 
  { 
    rxDoneFlag = false;

    String message2 = "";
    while (LoRa.available()) {
      message2 += (char)LoRa.read();
    }
    int firstComma = message2.indexOf(',');
    int secondComma = message2.indexOf(',', firstComma + 1);
    int thirdComma = message2.indexOf(',', secondComma + 1);

    ASpercent2 = message2.substring(0, firstComma).toInt();
    DADpercent2 = message2.substring(firstComma + 1, secondComma).toInt();
    temp2 = message2.substring(secondComma + 1, thirdComma).toFloat();
    hum2 = message2.substring(thirdComma + 1).toFloat();
    delay(1000);

    received=true;
  }
  Blynk.virtualWrite(V0, ASpercent);
  Blynk.virtualWrite(V1, DADpercent);
  Blynk.virtualWrite(V2, temp);
  Blynk.virtualWrite(V3, hum);
  Blynk.virtualWrite(V4, ASpercent2);
  Blynk.virtualWrite(V5, DADpercent2);
  Blynk.virtualWrite(V6, temp2);
  Blynk.virtualWrite(V7, hum2);
  if (temp2 >= NCBND2)
    {
      Blynk.logEvent("nd2");
    }
  if (hum2 >= NCBDA2)
    {
      Blynk.logEvent("da2");
    }
  
  if (temp >= NCBND1)
  {
    Blynk.logEvent("nd1");
  }
  if (hum >= NCBDA1)
  {
    Blynk.logEvent("da1");
  }
  if (received)
  {
    LoRa.beginPacket();
    LoRa.print("sleep");
    LoRa.endPacket();
    delay(100);

    LoRa.sleep();
    digitalWrite(26,1);
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    esp_sleep_enable_timer_wakeup(5ULL*60 * 1000000);
    esp_light_sleep_start();

    display.ssd1306_command(SSD1306_DISPLAYON);
    digitalWrite(26,0);
    delay(1000);
    LoRa.receive();
    received=false;  
  } 
}
