#define MQ135_PIN 1      // ADC ESP32-C3
#define BUZZER_PIN 3     // Pin untuk buzzer

// threshold bebas, nanti disesuaikan dari hasil bacaan sensor
int gasThreshold = 2500;  

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);        // 0–4095
  pinMode(BUZZER_PIN, OUTPUT);     // buzzer sebagai output
  digitalWrite(BUZZER_PIN, LOW);   // awalnya mati
}

void loop() {
  int adcValue = analogRead(MQ135_PIN);
  float voltage = adcValue * (3.3 / 4095.0);

  Serial.print("ADC: ");
  Serial.print(adcValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage, 3);

  // ---- DETEKSI GAS ----
  if (adcValue > gasThreshold) {
    digitalWrite(BUZZER_PIN, HIGH);   // bunyi
  } else {
    digitalWrite(BUZZER_PIN, LOW);    // mati
  }

  delay(300);
}
