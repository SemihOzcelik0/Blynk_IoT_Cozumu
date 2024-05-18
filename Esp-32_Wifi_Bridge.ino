#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Cihaz Bilgileri
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

// Token
char auth[] = BLYNK_AUTH_TOKEN;

// Modem Bilgileri
char ssid[] = ""; // Wi-Fi Ağ Adı
char pass[] = ""; // Wi-Fi Şifresi

// Saat NTP Server
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
int sleepCheck = 0;

// Esp32 Wifi Bilgileri
const char *myssid = "Esp-32_Bridge";
const char *mypassword = "9xURWQ1wjcQge41V";

int clientPort = 8080;

// TezgahBilgileri

#define tezgahCheck V6
int checkValue = 0;

// !!! Her CNC tezgahının statik ip adresi diziye bir eleman olarak aşağıdaki gibi eklenmeli !!!
IPAddress tezgah[] = {{192, 168, 4, 100}, {192, 168, 4, 101}};

int tezgahCount = sizeof(tezgah) / sizeof(tezgah[0]);

struct Tezgah
{
  String name;
  int passPin;
  int resultPin;
};

Tezgah tezgahlar[] = {{"Daewoo", V1, V4}, {"Hummer", V3, V5}};

WiFiServer server(8081);
WiFiClient esp8266Client;

#define onboardLed 2
#define tezgah1_led V0
#define tezgah2_led V2

WidgetTerminal terminal(V7);
WidgetTerminal terminal2(V8);

BLYNK_WRITE_DEFAULT()
{
  int pin = request.pin;
  int pinValue = param.asInt();
  for (int i = 0; i < tezgahCount; i++)
  {
    if (pinValue == 1 && pin == tezgahlar[i].passPin)
    {
      bool result = sendCommandToESP8266("/pass", tezgah[i], clientPort, tezgahlar[i].name);
      Blynk.virtualWrite(tezgahlar[i].resultPin, result ? 1 : 0);
    }
  }
}

BLYNK_WRITE(tezgah1_led)
{
  int pinValue = param.asInt();

  if (pinValue == 1)
  {
    terminal.println("Led On");
    sendCommandToESP8266("/", tezgah[0], clientPort, "Tezgaha");
  }
  else if (pinValue == 0)
  {
    terminal.println("Led Off");
    sendCommandToESP8266("/other", tezgah[0], clientPort, "Tezgaha");
  }
}

BLYNK_WRITE(tezgah2_led)
{
  int pinValue = param.asInt();

  if (pinValue == 1)
  {
    terminal.println("Led On");
    sendCommandToESP8266("/", tezgah[1], clientPort, "Tezgaha");
  }
  else if (pinValue == 0)
  {
    terminal.println("Led Off");
    sendCommandToESP8266("/other", tezgah[1], clientPort, "Tezgaha");
  }
}

BLYNK_WRITE(tezgahCheck)
{
  int pinValue = param.asInt();

  if (pinValue == 1)
  {
    checkValue = 0;
    terminal.println("Tezgahların Durumu Kontrol Ediliyor...");
  }
}

bool sendCommandToESP8266(String command, IPAddress board_ip, int board_port, String tezgahAdi)
{
  WiFiClient client;
  terminal.println(tezgahAdi + " --> Komut Gönderiliyor: " + command);
  if (client.connect(board_ip, board_port))
  {
    terminal.println("Komut Gönderildi.");
    client.print(String("GET ") + command + " HTTP/1.1\r\n" +
                 "Host: " + board_ip.toString() + "\r\n" +
                 "Connection: close\r\n\r\n");

    delay(500);

    while (client.connected())
    {
      String line = client.readStringUntil('\n');
      if (line == "\r")
      {
        break;
      }
    }

    while (client.available())
    {
      String line = client.readStringUntil('\r');
      terminal.println(tezgahAdi + ": " + line);
    }

    client.stop();
    return true;
  }
  else
  {
    // terminal.println(tezgahAdi +" ile bağlantı sağlanamadı.");
    return false;
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Blynk.begin(auth, ssid, pass);

  WiFiManager wifiManager;
  wifiManager.autoConnect("WiFi Ayarla");
  WiFi.softAP(myssid, mypassword, 5);
  IPAddress IP = WiFi.softAPIP();
  terminal.print("WiFi Ağı Yayınlandı. IP Adresi: ");
  terminal.println(IP);

  digitalWrite(onboardLed, HIGH);
  delay(1000);
  digitalWrite(onboardLed, LOW);

  server.begin();

  pinMode(onboardLed, OUTPUT);
}

void loop()
{
  Blynk.run();
  timeClient.update();

  if (esp8266Client = server.available())
  {
    if (esp8266Client.connected())
    {
      String notification = esp8266Client.readStringUntil('\n');
      esp8266Client.stop();
      terminal2.println(notification);

      // Bildirimi Blynk'e gönderme
      if (notification == "daewoo")
      {
        Blynk.logEvent("daewoo_hata_verdi");
        Blynk.virtualWrite(V5, 0);
      }
      else if (notification == "hummer")
      {
        Blynk.logEvent("hummer_hata_verdi");
        Blynk.virtualWrite(V4, 0);
      }
    }
  }

  if (checkValue == 0)
  {
    for (int i = 0; i < tezgahCount; i++)
    {
      bool result = sendCommandToESP8266("/check", tezgah[i], clientPort, tezgahlar[i].name);
      Blynk.virtualWrite(tezgahlar[i].resultPin, result ? 1 : 0);
      result ? terminal.println(tezgahlar[i].name + " Devrede...") : terminal.println(tezgahlar[i].name + " Erişilemedi...");
    }
    checkValue = 1;
  }

  if (timeClient.getHours() + 3 == 8 && sleepCheck == 0)
  {
    for (int i = 0; i < tezgahCount; i++)
    {
      bool result = sendCommandToESP8266("/sleep", tezgah[i], clientPort, tezgahlar[i].name);
      Blynk.virtualWrite(tezgahlar[i].resultPin, result ? 1 : 0);
      result ? terminal.println(tezgahlar[i].name + " Pasif Moda Geçti...") : terminal.println(tezgahlar[i].name + " Erişilemedi...");
      // bildirim ayarla
    }
    Blynk.logEvent("pasif_mod_devrede");
    sleepCheck = 1;
  }

  if (timeClient.getMinutes() + 3 == 18 && sleepCheck == 1)
  {
    for (int i = 0; i < tezgahCount; i++)
    {
      bool result = sendCommandToESP8266("/awake", tezgah[i], clientPort, tezgahlar[i].name);
      Blynk.virtualWrite(tezgahlar[i].resultPin, result ? 1 : 0);
      result ? terminal.println(tezgahlar[i].name + " Aktif...") : terminal.println(tezgahlar[i].name + " Erişilemedi...");
    }
    Blynk.logEvent("sensrler_devrede");
    sleepCheck = 0;
  }

  digitalWrite(onboardLed, HIGH);
}
