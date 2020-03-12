#include <SoftwareSerial.h>

#define TRIG_PIN_1 9
#define ECHO_PIN_1 10
#define TRIG_PIN_2 11
#define ECHO_PIN_2 12

#define WIFI_NAME "iPhone"
#define WIFI_PASSWORD "melih123"
#define WIFI_BAUDRATE 115200 // Wifi modulünün baudrate hızı

// Thingspeak configuration
#define HOST "api.thingspeak.com"
#define PORT "80"
#define CHANNEL_ID 924094
#define API_WRITE 8152CS4JYC09WD9I // to write data
#define API_READ BOCVTZ6E5EZ1CK77 // to read data
#define FIELD_IN PeopleIn
#define FIELD_OUT PeopleOut


int peoplein = 0;
int peopleout = 0;

int limitValue = 10; // Sensör tetiklenmesi için cm değeri

int sensor1, sensor2;

bool in = 0;

bool WIFI_CONNECTED = false;

int count = 0;
bool ok = false;

void setup() {

  pinMode(TRIG_PIN_1, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN_1, INPUT); // Sets the echoPin as an Input
  pinMode(TRIG_PIN_2, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN_2, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600); // Serialportu açıyoruz

  Serial3.begin(WIFI_BAUDRATE);

  checkWifiConnected();

}




void checkWifiConnected() {

  Serial3.println("AT"); // Bağlantı kontrolünü sağlıyoruz
  Serial.println("AT Yollandi.Modulden cevap bekleniyor...");

  while (!Serial3.find((char * )"OK")) {

    Serial3.println("AT");
    Serial.println("Wifi modulune baglanilamadi.Tekrar deneniyor...");
    delay(750);

  }

  Serial.println("Wifi bulundu!");

  Serial3.println("AT+CWMODE=1");
  Serial.println("Wifi modülünü client olarak ayarlamak için istek yollandi");

  while (!Serial3.find((char * )"OK")) {

    Serial3.println("AT+CWMODE=1");
    Serial.println("Ayar yapiliyor...");
    delay(750);

  }
  Serial.println("Wifi modülü client olarak ayarlandi!");

  String connectionCommand = String("AT+CWJAP=\"") + WIFI_NAME + "\",\"" + WIFI_PASSWORD + "\"";
  Serial3.println(connectionCommand);
  
  Serial.println("Aga baglaniliyor...");

  count = 0;
  ok = true;
  while (!Serial3.find((char * )"OK")) {
    if (count == 10)
    { ok = false;
      break;
    }
    count++;
  }

  if(ok)
    Serial.println("Aga baglanildi: " + String(WIFI_NAME));
  else
    Serial.println("Aga baglanilamadi!!: " + String(WIFI_NAME));


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
  String data = "GET https://api.thingspeak.com/update?api_key=ADXHLE6CHOSG8W9E";
  data += "&field1=";
  data += String(peoplein);
  data += "&field2 =";
  data += String(peopleout);
  data += "\r\n\r\n";

  Serial3.println("AT + CIPSEND=");
  Serial3.println(data.length() + 2);
  delay(2000);

  if(Serial3.find((char*)">")){
    Serial3.println(data);
    Serial.println("veri gönderildi");
    delay(1000);
    
  }
  Serial.println("Baglantı Kapatıldı.");
  Serial3.println("AT + CIPCLOSE");
  delay(1000);

  
 // Serial.println("In:" + String(peoplein));
 // Serial.println("Out:" + String(peopleout));
 // delay(250);

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

  //Serial.println("Mesafe_" + String(which) + ": "+ String(distance));
  return (distance <= limitValue) ? 1 : 0;
}
