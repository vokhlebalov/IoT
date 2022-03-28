#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>

ThreeWire myWire(7,6,8); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

RtcDateTime startTime;
uint64_t totalSeconds = 0;

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

const int CONNECTED_VALUE = 0; // Аналоговое значение, при котором счетчик подключен
const int METER_PIN = A0; // Пин для счетчика
const int GEAR_RATIO = 2000; // Передаточное число счетчика
const int IMPULSE_RATIO = 50; // Допустимое количество аналоговых значений, не равных значению подключения, при котором прерываение считается импульсом
const int CONNECTION_TIME = 200; // Время проверки подключения к счетчику
const int T = 3600; // секунд в часе

int sensorValue = 0;  // variable to store the value coming from the sensor
unsigned long connectionTimeMs = 0; // Время подключения счетчика
bool meterConnected = false;
bool impulseDetected = false;

int getImpulse() {
  sensorValue = analogRead(METER_PIN);
  int impulseValue = -1;

  if (sensorValue == CONNECTED_VALUE) {
    connectionTimeMs++;
    if (connectionTimeMs > CONNECTION_TIME) {
      connectionTimeMs = CONNECTION_TIME + 1;
      impulseValue = 0;
    }
  } else if (connectionTimeMs > CONNECTION_TIME) {
    int msCounter = 0;
    delay(300);

    for (int i = 0; i < CONNECTION_TIME; i++) {
      sensorValue = analogRead(METER_PIN);
      if (sensorValue != CONNECTED_VALUE) {
        msCounter++;
      }
    }

    if (msCounter < IMPULSE_RATIO) {
      impulseValue = 1;
      connectionTimeMs = CONNECTION_TIME + 1;
    } else { 
      connectionTimeMs = 0;
    }   
  } 

  return impulseValue;
}

void setAnalogConnection() {
  Serial.println("Checking for the power meter connection...");

  // Стартовый прогон для проверки подключения
  for (int i = 0; i < 500; i++) {
    getImpulse();
  }

  if (getImpulse() < 0) {
    Serial.println("No connection to the meter");
    Serial.println("Checking for the power meter connection...");
  } else {
    Serial.println("Power meter connected");
    meterConnected = true;
  }
}
