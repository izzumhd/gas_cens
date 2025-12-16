#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <esp_sleep.h>

#define MQ135_PIN 1
#define BUZZER_PIN 3

const char* ssid = "Udaradingin";
const char* password = "woiwoiwoi";

#define BOT_TOKEN "8583077078:AAG3JtRBUt7Zy4AKJqqHurIHQg7mJGCzkBM"
#define CHAT_ID "7239861208"

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

int thresholdMedium = 2000;
int thresholdHigh = 3000;

int beepShort = 150;
int beepLong = 300;
int beepGap = 80;

// ================= TIMER =================
unsigned long lastAlert = 0;
unsigned long lastBotCheck = 0;

// ================= STATUS =================
String gasStatus = "NORMAL";
int lastADC = 0;

// med
void klaksonMedium() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepShort);
    digitalWrite(BUZZER_PIN, LOW);
    delay(400);
  }
}

// hi
void klaksonSOS() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepShort);
    digitalWrite(BUZZER_PIN, LOW);
    delay(beepGap);
  }

  delay(200);
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepLong);
    digitalWrite(BUZZER_PIN, LOW);
    delay(beepGap);
  }

  delay(200);
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(beepShort);
    digitalWrite(BUZZER_PIN, LOW);
    delay(beepGap);
  }
  delay(200);
}

//  turu
void enterLightSleep() {
  WiFi.setSleep(true);
  esp_sleep_enable_timer_wakeup(300000);  // 300 ms
  esp_light_sleep_start();
  WiFi.setSleep(false);
}

String buildStatusMessage() {
  String msg = "Status Gas Saat Ini\n";
  msg += "ADC: " + String(lastADC) + "\n";
  msg += "Kondisi: " + gasStatus;
  return msg;
}

void handleTelegram() {
  int newMsg = bot.getUpdates(bot.last_message_received + 1);

  while (newMsg) {
    for (int i = 0; i < newMsg; i++) {
      String text = bot.messages[i].text;
      String chat_id = bot.messages[i].chat_id;

      if (chat_id != CHAT_ID) continue;

      if (text == "/status") {
        bot.sendMessage(chat_id, buildStatusMessage(), "");
      } else {
        bot.sendMessage(chat_id, "Command tidak dikenali.\nGunakan /status", "");
      }
    }
    newMsg = bot.getUpdates(bot.last_message_received + 1);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  analogReadResolution(12);

  WiFi.begin(ssid, password);
  client.setInsecure();

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  bot.sendMessage(CHAT_ID, "ESP32 aktif dan siap menerima perintah.", "");
}

void loop() {
  lastADC = analogRead(MQ135_PIN);
  Serial.print("Nilai gas: ");
  Serial.print(lastADC);
  Serial.print("  |  Status: ");
  Serial.println(gasStatus);

  if (lastADC < thresholdMedium) {
    gasStatus = "NORMAL";
  } else if (lastADC < thresholdHigh) {
    gasStatus = "MENENGAH";
  } else {
    gasStatus = "TINGGI";
  }

  if (gasStatus == "MENENGAH" && millis() - lastAlert > 5000) {
    bot.sendMessage(CHAT_ID, buildStatusMessage(), "");
    klaksonMedium();
    lastAlert = millis();
  }
  if (gasStatus == "TINGGI" && millis() - lastAlert > 3000) {
    bot.sendMessage(CHAT_ID, buildStatusMessage(), "");
    klaksonSOS();
    lastAlert = millis();
  }
  if (millis() - lastBotCheck > 1000) {
    handleTelegram();
    lastBotCheck = millis();
  }
  if (gasStatus == "NORMAL") {
    delay(200);
  }
}
