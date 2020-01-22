//************************\\//**************************\\
//**Smarthouse IoT Blynk**\\//**Created by Maxim Kotov**\\
//************************\\//**************************\\
////////////////////////////

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <Adafruit_ADS1015.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <SPI.h>                       // конфигурация блинка // Blynk configuration 

#include <FastLED.h>                   // конфигурация матрицы // LED matrix configuration   
#include <FastLED_GFX.h>
#include <FastLEDMatrix.h>
#define NUM_LEDS 12                    // количество светодиодов в матрице // number of LEDs 
CRGB leds[NUM_LEDS];                   // определяем матрицу (FastLED библиотека) // defining the matrix (fastLED library)
#define LED_PIN             27         // пин к которому подключена матрица // matrix pin
#define COLOR_ORDER         GRB        // порядок цветов матрицы // color order 

#define  PHOTORESISTOR      34
#define  LIGHT              16

#define  wind   17                     // пин вентилятора // cooler pin 
#define  water  39                     // пин воды
#define  CO2    38                     // пин углексислого газа

char ssid[] = "Redmi";                            // Логин Wi-Fi  // Wi-Fi login
char pass[] = "uuubbb555";                    // Пароль от Wi-Fi // Wi-Fi password
char auth[] = "X7vNBlQVt1tDh84hmU2_g13a37yUNGwZ"; // Токен // Authorization token
IPAddress blynk_ip(139, 59, 206, 133);            // конфигурация блинка // Blynk configuration

#include <Seeed_BME280.h>  // BME280                         
BME280 bme280;       //

#define UPDATE_TIMER 3333
BlynkTimer timer_update;      // настройка таймера для обновления данных с сервера BLynk // Blynk update timer configuration
#define UPDATE_button 250
#define UPDATE_sound 25


//////////////////////////////////////////НАСТРОЙКИ/CONFIGURATION/////////////////////////////////////////////////////////////////
void setup()
{
  pinMode(  PHOTORESISTOR, INPUT );
  pinMode(  LIGHT, OUTPUT );
  digitalWrite( LIGHT, LOW);
  pinMode(  CO2, INPUT );
  pinMode(  water, INPUT );
  pinMode(  wind, OUTPUT );       // настройка пинов насоса и вентилятора на выход // pump and cooler pins configured on output mode
  digitalWrite(wind, LOW);
  

  Serial.begin(115200);

  delay(512);
  Blynk.begin(auth, ssid, pass, blynk_ip, 8442);         // подключение к серверу Blynk // connection to Blynk server

  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);    // конфигурация матрицы // matrix configuration
  for (int i = 0; i < 12; i++)
      leds[i] = CRGB(125, 200, 50);
  FastLED.setBrightness(20);
  FastLED.show();

  delay(1000);

  bool bme_status = bme280.init();
  if (!bme_status)
    Serial.println("Could not find a valid BME280 sensor, check wiring!");  // проверка  датчика температуры, влажности и давления // checking the temp hum bar sensor

  timer_update.setInterval(UPDATE_TIMER, readSendData);  // включаем таймер обновления данных  // turn on the update timer
}

//////////////////////////////////////////////////////ЧТЕНИЕ И ЗАПИСЬ ДАННЫХ ДАТЧИКОВ/SENSOR DATA SEND/READ////////////////////////////////////////////////
void readSendData() {
  
  float w = analogRead(water);
  Blynk.virtualWrite(V1, w); delay(2);        // Отправка данных на сервер Blynk  вода    // Water data send
  
  float t = bme280.getTemperature();
  float h = bme280.getHumidity();
  float p = bme280.getPressure() * 0.00750063755419211;
  Blynk.virtualWrite(V14, t); delay(2);        // Отправка данных на сервер Blynk  Температура // Temperature data send
  Blynk.virtualWrite(V15, h); delay(2);        // Отправка данных на сервер Blynk  Влажность   // Humidity data send
  Blynk.virtualWrite(V16, p); delay(2);        // Отправка данных на сервер Blynk  Давление    // Pressure data send

  float co2 = analogRead(CO2);
  Blynk.virtualWrite(V13, co2); delay(2);
}

/////////////////////////////////////////////////ГЛАВНЫЙ ЦИКЛ/MAIN LOOP///////////////////////////////////////////////////////////////////////////////
void loop()
{
  Blynk.run();                                          // запуск Blynk  // turn Blynk on
  timer_update.run();
  
  int light = analogRead(PHOTORESISTOR);
  if ( light < 1000)
    digitalWrite(LIGHT, HIGH);
  else 
    digitalWrite(LIGHT, LOW);
}

//////////////////////////////////////////ВЕНТИЛЯТОР/COOLER/////////////////////////////////////////////////////////////
BLYNK_WRITE(V6)
{
  int buttonstate2 = param.asInt ();
  if (buttonstate2 == 1) {
    digitalWrite(wind, HIGH);         // включить, если нажата кнопка "Вентилятор" // turn on the cooler if button = 1
  }
  else    {
    digitalWrite(wind, LOW);
  }
}
////////////////////////////////////////////////ЦВЕТ/COLOUR/////////////////////////////////////////////////////////////////////////////////
BLYNK_WRITE(V2)
{
  int r = param[0].asInt();                            // установка  R  set up
  int g = param[1].asInt();                            //            G
  int b = param[2].asInt();                            // параметров B  parameters
  fill_solid( leds, NUM_LEDS, CRGB(r, g, b));          // заполнить всю матрицу выбранным цветом
  FastLED.show();
}

//////////////////////////////////////////////ЯРКОСТЬ/BRIGHTNESS///////////////////////////////////////////////////////////////////////////////
BLYNK_WRITE(V4)
{
  int power = param.asInt();
  FastLED.setBrightness(power);                     // устанавливаем яркость по значению слайдера (0-255) /setting the brightness (0-255)
  FastLED.show();                                   // отобразить полученные данные // show the result
}
