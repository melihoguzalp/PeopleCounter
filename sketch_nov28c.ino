#include <SoftwareSerial.h>

#define TRIG_PIN_1  9
#define ECHO_PIN_1  10
#define TRIG_PIN_2  11
#define ECHO_PIN_2  12


#define agAdi "NetasStayjer";
#define agSifresi "Netas2019"

#define WIFI_BAUDRATE  115200 // Wifi modulünün baudrate hızı
#define WIFI_TX_PIN  14
#define WIFI_RX_PIN  15



SoftwareSerial esp(WIFI_RX_PIN, WIFI_TX_PIN);

int peoplein = 0;
int peopleout = 0;

int limitValue = 10; // Sensör tetiklenmesi için cm değeri

int sensor1, sensor2;

bool in = 0;

bool WIFI_CONNECTED = false;

void setup() {

  pinMode(TRIG_PIN_1, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN_1, INPUT); // Sets the echoPin as an Input
  pinMode(TRIG_PIN_2, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN_2, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600); // Serialportu açıyoruz

  wifi.begin(WIFI_BAUDRATE);

  checkWifiConnected();

}

void loop() {

  readSensors();

  if (sensor1 == 1 && sensor2 == 0)
  {
    in = 1;
    while (sensor1 != 0 || sensor2 != 0)
    {
      readSensors();
      delay(100);

    }
    peoplein += 1;

  }
  else if (sensor1 == 0 && sensor2 == 1)
  {

    in = 0;
    while (sensor1 != 0 || sensor2 != 0)
    {
      readSensors();
      delay(100);

    }
    peopleout += 1;

  }

  delay(250);

}

void readSensors() {

  sensor1 = readSensorValue(0);
  sensor2 = readSensorValue(1);

}
int readSensorValue(int which) {

  double duration = 0;
  double distance = 0;

  digitalWrite(which == 0 ? TRIG_PIN_1 : TRIG_PIN_2, LOW);
  delayMicroseconds(2);
  digitalWrite(which == 0 ? TRIG_PIN_1 : TRIG_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(which == 0 ? TRIG_PIN_1 : TRIG_PIN_2, LOW);
  duration = pulseIn(which == 0 ? ECHO_PIN_1 : ECHO_PIN_2, HIGH);
  distance = duration / 29.2 / 2;
  delay(50);

  return (distance <= limitValue) ? 1 : 0;
}


void checkWifiConnected() {

  wifi.println("AT"); // Bağlantı kontrolünü sağlıyoruz
  Serial.println("AT Yollandi");

  while (!wifi.find((char * )"OK")) {

    wifi.println("AT");
    Serial.println("Wifi bulunamadi.Tekrar deneniyor...");
    delay(100);

  }

  Serial.println("Wifi bulundu!");

  wifi.println("AT+CWMODE=1");
  Serial.println("Wifi modülünü client olarak ayarlamak için istek yollandi");

  while(!wifi.find((char * )"OK")){
    
    wifi.println("AT+CWMODE=1");
    Serial.println("Ayar yapiliyor...");
    
  }
  Serial.println("Wifi modülü client olarak ayarlandi!");
  Serial.println("Ağa Bağlanılıyor!");
  wifi.println("AT+CWJAP=\"" + agAdi + "\",\"" + agSifresi + "\"");
  //Serial.println("Aga baglaniliyor...");
  while(!wifi.find((char * )"OK"));
  Serial.println("Aga baglanildi: " + String(agAdi));
  delay(1000);

}
