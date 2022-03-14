void setup() {
  Serial.begin(9600);
  setMQTTConnection();
  setAnalogConnection();
}

void loop() {
  int impulseValue = getImpulse();
  output(impulseValue);
}
