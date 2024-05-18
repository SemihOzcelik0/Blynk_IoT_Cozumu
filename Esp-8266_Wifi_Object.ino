#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

Servo servo;

const char *ssid = "Esp-32_Bridge";        // ESP32 WiFi ağı adı
const char *password = "9xURWQ1wjcQge41V"; // ESP32 WiFi şifresi

//!!! HER MODÜLÜN STATİK IP ADRESİ FARKLI OLMALI !!! "orn: 192, 168, 4, 101"
IPAddress staticIP(192, 168, 4, 100); // ESP8266'nın statik IP adresi
IPAddress gateway(192, 168, 4, 1);    // Ağ geçidi IP adresi
IPAddress subnet(255, 255, 255, 0);   // Alt ağ maskesi

ESP8266WebServer server(8080);

IPAddress esp32IP(192, 168, 4, 1);
int esp32Port = 8081;

unsigned long previousMillis1 = 0;
const unsigned long interval1 = 5000;

unsigned long previousMillis2 = 0;
const unsigned long interval2 = 1000;

unsigned long previousMillis3 = 0;
const unsigned long interval3 = 2000;

int send = 0;
int mod = 1; // Mod-0: Buzzer || Mod-1: Bildirim
int buz = 0;
int bzr = 0;

#define sensor A0
#define button 5
#define buzzer 3

const int ledPin = 2;
const int ledr = 0; // 13
const int ledg = 15;
const int ledb = 1;

void handleRoot()
{
  digitalWrite(ledPin, LOW); // LED'i yak
  server.send(200, "text/plain", "LED yanıyor");
}

void handleOther()
{
  digitalWrite(ledPin, HIGH); // LED'i söndür
  server.send(200, "text/plain", "LED söndü");
}

void handlePass()
{
  send = 0;
  servo.write(0);
  server.send(200, "text/plain", "Sensör Devrede");
  digitalWrite(ledPin, LOW);
  delay(50);
  digitalWrite(ledPin, HIGH);

  if (mod == 1)
  {
    digitalWrite(ledr, HIGH);
    digitalWrite(ledg, LOW);
    digitalWrite(ledb, HIGH);
  }
  else if (mod == 0)
  {
    digitalWrite(ledr, LOW);
    digitalWrite(ledg, LOW);
    digitalWrite(ledb, HIGH);
  }
}

void handleSleep()
{
  mod = 0;
  digitalWrite(ledPin, LOW);
  delay(50);
  digitalWrite(ledPin, HIGH);

  digitalWrite(ledr, LOW);
  digitalWrite(ledg, LOW);
  digitalWrite(ledb, HIGH);

  server.send(200, "text/plain", "Buzzer Aktif Bildirimler Kapalı...");
}

void handleAwake()
{
  mod = 1;
  digitalWrite(ledPin, LOW);
  delay(50);
  digitalWrite(ledPin, HIGH);

  digitalWrite(ledr, HIGH);
  digitalWrite(ledg, LOW);
  digitalWrite(ledb, HIGH);

  server.send(200, "text/plain", "Buzzer Kapalı Bildirimler Aktif...");
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  pinMode(ledr, OUTPUT);
  pinMode(ledg, OUTPUT);
  pinMode(ledb, OUTPUT);

  pinMode(buzzer, OUTPUT);
  // Serial.begin(9600);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }

  // Statik IP ayarını yapma
  WiFi.config(staticIP, gateway, subnet);

  Serial.println("");
  Serial.println("WiFi bağlandı!");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/other", handleOther);
  server.on("/pass", handlePass);
  server.on("/sleep", handleSleep);
  server.on("/awake", handleAwake);

  server.begin();
  Serial.println("Web sunucusu başlatıldı");

  digitalWrite(buzzer, HIGH);

  digitalWrite(ledPin, HIGH); // LED'i söndür

  servo.attach(4);
  servo.write(0);

  digitalWrite(ledr, HIGH);
  digitalWrite(ledg, LOW);
  digitalWrite(ledb, HIGH);
}

void loop()
{
  unsigned long currentMillis = millis();

  server.handleClient();

  if (currentMillis - previousMillis1 >= interval1)
  {
    previousMillis1 = currentMillis;

    if (analogRead(sensor) > 1023 && send == 0)
    {
      digitalWrite(ledr, LOW);
      digitalWrite(ledg, HIGH);
      digitalWrite(ledb, HIGH);
      send = 1;
      buz = 1;
      if (mod == 1)
      {
        servo.write(180);
        sendNotificationToESP32("hummer");
        buz = 1;
      }
      else if (mod == 0)
      {
        buz = 1;
      }
    }
  }

  if (currentMillis - previousMillis3 >= interval3)
  {
    previousMillis3 = currentMillis;

    if (buz == 1 && bzr <= 5)
    {
      for (int j = 0; j < 5; j++)
      {
        digitalWrite(buzzer, LOW);
        delay(250);
        digitalWrite(buzzer, HIGH);
        delay(50);
      }
      bzr += 1;
    }
  }

  if (bzr >= 5)
  {
    bzr = 0;
    buz = 0;
  }

  if (currentMillis - previousMillis2 >= interval2)
  {
    previousMillis2 = currentMillis;
    // sendNotificationToESP32(String(analogRead(sensor)));

    if (digitalRead(button) == 0)
    {
      send = 0;
      servo.write(0);
      Serial.println("Buton Aktif");
      digitalWrite(ledPin, LOW);
      delay(50);
      digitalWrite(ledPin, HIGH);

      digitalWrite(ledr, HIGH);
      digitalWrite(ledg, LOW);
      digitalWrite(ledb, HIGH);
    }
  }
}

void sendNotificationToESP32(String message)
{
  WiFiClient client;
  int check = 0;
  while (check < 3)
    if (client.connect(esp32IP, esp32Port))
    {
      client.print(message);
      Serial.println("Bildirim Gönderildi.");
      client.stop();
      break;
    }
    else
    {
      Serial.println("ESP32 ile bağlantı kurulamadı.");
      check += 1;
    }
}
