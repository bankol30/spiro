const int current;
float cur;
void setup() {
Serial.begin(9600);

}

void loop() {
  current=analogRead(0);
  delay(100);
  Serial.println(current);
  // put your main code here, to run repeatedly:
}
