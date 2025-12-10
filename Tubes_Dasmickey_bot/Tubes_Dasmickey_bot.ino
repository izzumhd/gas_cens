#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_sleep.h>

#define MQ135_PIN 1
#define BUZZER_PIN 3

// ---------- Isi sesuai WiFi kamu ----------
const char* ssid = "Udaradingin";
const char* password = "woiwoiwoi";

// ---------- Telegram Bot ----------
String BOT_TOKEN = "8583077078:AAG3JtRBUt7Zy4AKJqqHurIHQg7mJGCzkBM";
String CHAT_ID = "7239861208";

int thresholdMedium = 2000;
int thresholdHigh = 3000;

WiFiClientSecure client;

void sendTelegram(String pesan) {
  if (!client.connect("api.telegram.org", 443)) return;

  String url = "/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + pesan;

  client.println("GET " + url + " HTTP/1.1");
  client.println("Host: api.telegram.org");
  client.println("Connection: close");
  client.println();
}

void klakson(int dur, int jeda) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(dur);
    digitalWrite(BUZZER_PIN, LOW);
    delay(jeda);
  }
  delay(200);
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(dur * 2);
    digitalWrite(BUZZER_PIN, LOW);
    delay(jeda);
  }
  delay(200);
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(dur);
    digitalWrite(BUZZER_PIN, LOW);
    delay(jeda);
  }
  delay(1000);
}

void enterLightSleep() {
  Serial.println("→ Light sleep aktif...");

  // WiFi sleep (hemat banyak energi)
  WiFi.setSleep(true);

  // Masuk mode sleep sekitar 200ms (cukup untuk hemat, ADC tetap hidup)
  esp_sleep_enable_timer_wakeup(200000);
  esp_light_sleep_start();

  // Setelah bangun, WiFi masih OFF, ADC masih ON
  WiFi.setSleep(false);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);

  WiFi.begin(ssid, pass);
  client.setInsecure();

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nWiFi Connected!");
}

void loop() {
  int adcValue = analogRead(MQ135_PIN);

  Serial.print("Gas ADC: ");
  Serial.println(adcValue);

  // ===== NORMAL =====
  if (adcValue < thresholdMedium) {
    digitalWrite(BUZZER_PIN, LOW);
    enterLightSleep();  // hemat daya
    return;
  }

  // ===== MEDIUM =====
  if (adcValue >= thresholdMedium && adcValue < thresholdHigh) {
    sendTelegram("⚠️ GAS MENENGAH TERDETEKSI\nADC: " + String(adcValue));
    klakson(200, 600)
      delay(3000);
    return;
  }

  // ===== HIGH =====
  if (adcValue >= thresholdHigh) {
    sendTelegram("🚨 GAS TINGGI! BAHAYA!\nADC: " + String(adcValue));
    klakson(200, 300);  // 1 kali pattern
    delay(3000);
    return;
  }
}
