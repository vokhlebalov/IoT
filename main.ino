const long MINUTE = 60;
const long QUARTER_HOUR = 900;
const long HOUR = 3600;
const long DAY = 86400;

unsigned long impulseCounter = 0;
unsigned long minuteImpulseCounter = 0;
unsigned long quarterHourImpulseCounter = 0;
unsigned long hourImpulseCounter = 0;
unsigned long dayImpulseCounter = 0;

double minuteConsumption = 0.0;
double quarterHourConsumption = 0.0;
double hourConsumption = 0.0;
double dayConsumption = 0.0;
double totalConsumption = 3888.0;

const char minuteTopic[] = "mega_1/minuteConsumption";
const char quarterHourTopic[] = "mega_1/quarterHourConsumption";
const char hourTopic[] = "mega_1/hourConsumption";
const char dayTopic[] = "mega_1/dayConsumption";
const char totalTopic[] = "mega_1/totalConsumption";


// Ethernet and MQTT related objects
EthernetClient ethClient;
PubSubClient mqttClient(ethClient);

double getPowerConsumption(unsigned long n) {
  return 1000.0 * n / GEAR_RATIO;
}

void incImpulseCounter() {
  impulseCounter++;
  minuteImpulseCounter++;
  quarterHourImpulseCounter++;
  hourImpulseCounter++;
  dayImpulseCounter++;
}

void impulseOutPut(int impulseValue) {
  if (impulseValue > 0) {
    incImpulseCounter();
    Serial.println("impulse detected");
    impulseDetected = true;
  } else if (impulseValue == 0){
    if (!meterConnected /*&& !impulseDetected*/) {
      Serial.println("Power meter connected");
    }

    meterConnected = true;
    impulseDetected = false;
  } else {
    if (meterConnected && !impulseDetected) {
      Serial.println("No connection to the power meter");
      Serial.println("Checking for the power meter connection...");
    }
    impulseDetected = false;
    meterConnected = false;
  }
}

void getConsumption() {
  RtcDateTime now = Rtc.GetDateTime();
  totalSeconds = calculateTotalSeconds(startTime, now);
  if (totalSeconds % DAY == 0) {
    dayConsumption = getPowerConsumption(dayImpulseCounter);
    // Отправка потребления за день
    Serial.print("day consumption: "); Serial.print(dayConsumption); Serial.println(" W/h");
    publish(dayConsumption, dayTopic);
    dayImpulseCounter = 0;
  }
  if (totalSeconds % HOUR == 0) {
    hourConsumption = getPowerConsumption(hourImpulseCounter);
    // Отправка потребления за час
    Serial.print("hour consumption: "); Serial.print(hourConsumption); Serial.println(" W/h");
    publish(hourConsumption, hourTopic);
    hourImpulseCounter = 0;
  }
  if (totalSeconds % QUARTER_HOUR == 0) {
    quarterHourConsumption = getPowerConsumption(quarterHourImpulseCounter);
    // Отправка потребления за четвереть часа 
    Serial.print("quarter hour consumption: "); Serial.print(quarterHourConsumption); Serial.println(" W/h");
    publish(quarterHourConsumption, quarterHourTopic);
    quarterHourImpulseCounter = 0;
  }
  if (totalSeconds % MINUTE == 0) {
    minuteConsumption = getPowerConsumption(minuteImpulseCounter);
    totalConsumption = getPowerConsumption(impulseCounter);
    Serial.print("Minute consumption: "); Serial.print(minuteConsumption); Serial.println(" W/h");
    Serial.print("Total consumption: "); Serial.print(totalConsumption); Serial.println("W/h");
    publish(minuteConsumption, minuteTopic);
    publish(totalConsumption, totalTopic);
    minuteImpulseCounter = 0;
    delay(1000);
  }
}

void setup() {
  Serial.begin(9600);
  setTime();
  setMQTTConnection();
  setAnalogConnection();  
}

void loop() {
  // This is needed at the top of the loop!
  mqttClient.loop();

  int impulseValue = getImpulse();
  impulseOutPut(impulseValue);
  getConsumption();
}
