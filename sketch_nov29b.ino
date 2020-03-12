#define TRIG_PIN_1 9

#define ECHO_PIN_1 10

#define TRIG_PIN_2 11

#define ECHO_PIN_2 12

 

// Wifi configuration

#define WIFI_NAME "oguzalp"

#define WIFI_PASSWORD "658915344"

#define WIFI_BAUDRATE 115200 // Wifi modulünün baudrate hızı

 

// Thingspeak configuration

String HOST = "api.thingspeak.com";

String PORT = "80";

String CHANNEL_ID = "924094";

String API_WRITE = "8152CS4JYC09WD9I"; // to write data

String API_READ = "BOCVTZ6E5EZ1CK77"; // to read data

String FIELD_PEOPLEIN = "field1";

String FIELD_PEOPLEOUT = "field2";

 

 

int peopleIn = 0;

int peopleOut = 0;

 

int sensorEngelMesafesi = 30; // Sensör tetiklenmesi için cm değeri

 

int sensor1 = 0, sensor2 = 0;

bool in = 0;

 

int count = 0;

bool ok = true;

 

void initEsp(); // Wifi bağlantısı sağlıyor

void connectToServer(); // Servere baglantı saglanıyor

void disconnectFromServer(); // Serverden baglanti kesiliyor

void sendData(); // Thingspeak'e datayı yolluyor

void readSensor(); // Sensörlerden veri okuyor

int readSensorValue(int); // return= 0: Engel yok , 1: Engel var

 

long time_data = 0;

 

void setup() {

 

  pinMode(TRIG_PIN_1, OUTPUT); // Sets the trigPin as an Output

  pinMode(ECHO_PIN_1, INPUT); // Sets the echoPin as an Input

  pinMode(TRIG_PIN_2, OUTPUT); // Sets the trigPin as an Output

  pinMode(ECHO_PIN_2, INPUT); // Sets the echoPin as an Input

 

  Serial.begin(9600); // Serial monitoru başlatıyoruz

  Serial3.begin(WIFI_BAUDRATE); // Esp modulü ile haberleşmeyi başlatıyor

 

  initEsp(); // Esp modulünü kuruyoruz

 

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

    peopleIn += 1;

 

  }

  else if (sensor1 == 0 && sensor2 == 1)

  {

 

    in = 0;

    while (sensor1 != 0 || sensor2 != 0)

    {

      readSensors();

      delay(100);

 

    }

    peopleOut += 1;

 

  }

 

  sendData();

  delay(100);

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

 

  return (distance <= sensorEngelMesafesi) ? 1 : 0;

}

 

 

void initEsp() {

 

  Serial3.println("AT"); // Esp ile bağlantı sağlanıyor

  Serial.println("AT Yollandi.Esp modulünden cevap bekleniyor...");

 

  while (!Serial3.find((char*)"OK")) {

 

    Serial3.println("AT");

    Serial.println("Esp modulune baglanilamadi.Tekrar deneniyor...");

    delay(100);

 

  }

 

  Serial.println("Wifi modulune baglanildi!");

 

  Serial3.println("AT+CWMODE=1"); // Esp modulünü client olarak ayarlıyoruz

  Serial.println("Esp modülünü client olarak ayarlamak için istek yollandi");

 

 

  while (!Serial3.find((char*)"OK")) {

 

    Serial3.println("AT+CWMODE=1");

    Serial.println("Ayar yapiliyor...");

    delay(100);

 

  }

 

 

  Serial.println("Wifi modülü client olarak ayarlandi!");

 

  String connectionCommand = "AT+CWJAP=";

  connectionCommand += "\"";

  connectionCommand += WIFI_NAME;

  connectionCommand += "\"";

  connectionCommand += ",";

  connectionCommand += "\"";

  connectionCommand += WIFI_PASSWORD;

  connectionCommand += "\"";

 

  Serial3.println(connectionCommand);

  Serial.println("Aga baglaniliyor...");

 

  while (!Serial3.find((char*)"OK")) {

    Serial3.println(connectionCommand);
    Serial.println("Aga baglanildi: " + String(WIFI_NAME));
    delay(1000);

  }

 

/*  if (ok)

    Serial.println("Aga baglanildi: " + String(WIFI_NAME));

  else

    Serial.println("Aga baglanilamadi!!: " + String(WIFI_NAME));
*/
 

  connectToServer();

 

}

 

void connectToServer() {

 

  String connect_ = "\"TCP\"";

  connect_ += ",";

  connect_ += "\"" + HOST + "\"";

  connect_ += ",";

  connect_ += PORT;

 

  Serial3.println("AT+CIPSTART=" + connect_);

  Serial.println("Thingspeak serverine baglaniliyor..." + String("(" + connect_ + ")"));

 

 /* while (!Serial3.find((char*)"OK")) {

    Serial3.println("AT+CIPSTART=" + connect_);

    delay(100);

  }

 */

 if(Serial3.find((char*)"Error")){

    Serial.println("AT + CIPSTART Error");
  
 }

  Serial.println("Thingspeak serverine baglanti kuruldu.");

 

}

 
/*
void disconnectFromServer() {

 

  Serial3.println("AT+CIPCLOSE");

  Serial.println("Serverden baglanti kopariliyor...");

 

  while (!Serial3.find((char*)"OK")) {

    Serial3.println("AT+CIPCLOSE");

    delay(100);

  }

 

  Serial.println("Serverden baglanti koptu.");

 

}

 */

void sendData() {

 

  connectToServer();

 

  String veri = "GET ";

  veri += "/update?";

  veri += "api_key=" + API_WRITE;

  veri += "&FIELD_PEOPLEIN=";

  veri += String(peopleIn);

  veri += "&FIELD_PEOPLEOUT=";

  veri += String(peopleOut);

  veri += "\r\n\r\n";

 

  Serial3.print("AT+CIPSEND=");
  Serial3.print(veri.length() + 2);
  
  Serial.println("Gönderilecek veri boyutu ayarlandi:" + String(veri.length()));

 

  Serial.println("Cevap bekleniyor...");

 delay(2000);

  if (Serial3.find((char*)">")) {

    Serial3.print(veri);
    Serial.println(veri);
    Serial.println("Veri Gönderildi..");

    delay(1000);

  }

 
Serial.println("Bağlantı Kapatildi.");

    Serial3.print("AT + CIPCLOSE");
 

  //disconnectFromServer();

 

}
