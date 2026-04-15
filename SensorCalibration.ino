void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("----SENSOR ESQUERDA-----");
  Serial.println(analogRead(A0));
  Serial.println("----SENSOR DIREITA-----");
  Serial.println(analogRead(A1));
  delay(1000);
}
