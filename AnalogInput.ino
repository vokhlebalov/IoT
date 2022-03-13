const int CONNECTED_VALUE = 0/*695*//*1023*/; // Аналоговое значение, при котором счетчик подключен
const int METER_PIN = A0; // Пин для счетчика
const int GEAR_RATIO = 2000; // Передаточное число счетчика
const int CONNECTION_TIME = 200; // Время проверки подключения к счетчику

int sensorValue = 0;  // variable to store the value coming from the sensor
unsigned long connectionTimeMs = 0; // Время подключения счетчика
bool meterConnected = false;
bool impulseDetected = false;

int getImpulse() {
  int impulseValue = -1;
  sensorValue = analogRead(METER_PIN);

  if (sensorValue == CONNECTED_VALUE) {
    connectionTimeMs++;
    if (connectionTimeMs > CONNECTION_TIME) {
      connectionTimeMs = CONNECTION_TIME + 1;
      impulseValue = 0;
    }
  } else if (connectionTimeMs > CONNECTION_TIME) {
    int msCounter = 0;
    delay(CONNECTION_TIME);

    for (int i = 0; i < 200; i++) {
      sensorValue = analogRead(METER_PIN);
      if (sensorValue != CONNECTED_VALUE) {
        msCounter++;
      }
      delay(1);
    }

    if (msCounter < 5) {
      impulseValue = 1;
    } else { 
      connectionTimeMs = 0;
    }   
  } 
  delay(2);

  return impulseValue;
}

void output(int impulseValue) {
  if (impulseValue > 0) {
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

void setup() {
  Serial.begin(9600);

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

void loop() {
  int impulseValue = getImpulse();
  output(impulseValue);
}
