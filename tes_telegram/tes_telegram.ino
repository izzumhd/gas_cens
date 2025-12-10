#include <WiFi.h>
#include <WiFiClientSecure.h>

#define BUZZER_PIN 3

const char* ssid = "Udaradingin";
const char* password = "woiwoiwoi";

String BOT_TOKEN = "8583077078:AAG3JtRBUt7Zy4AKJqqHurIHQg7mJGCzkBM";
String CHAT_ID = "7239861208";

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  client.setInsecure(); // biarin tanpa certificate

  sendTelegram("ESP32-C3 online!");
}

void loop() {
  klakson(200, 600);
  klakson(200, 300);
}

void sendTelegram(String msg) {
  if (!client.connect("api.telegram.org", 443)) {
    Serial.println("gagal connect");
    return;
  }

  String url = "/bot" + BOT_TOKEN + "/sendMessage?chat_id=" + CHAT_ID + "&text=" + msg;

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
  Serial.println("buzzer \n");
  delay(1000);
}

