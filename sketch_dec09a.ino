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

 

bool sentCommand = false;

 

int peopleIn = 0;

int peopleOut = 0;

 

int sensorEngelMesafesi = 20; // Sensör tetiklenmesi için cm değeri

 

int sensor1 = 0, sensor2 = 0;

int sensor1Last = 0, sensor2Last = 0;

 

bool flagIn = false;

bool flagOut = false;

 

void initEsp(); // Wifi bağlantısı sağlıyor

void connectToServer(); // Servere baglantı saglanıyor

void disconnectFromServer(); // Serverden baglanti kesiliyor

void sendData(); // Thingspeak'e datayı yolluyor

void readSensor(); // Sensörlerden veri okuyor

int readSensorValue(int); // return= 0: Engel yok , 1: Engel var

bool sendCommandEsp(String, char*, int); // Esp modulüne komut yollar

 

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

    flagIn = true;

  else if (sensor1 == 0 && sensor2 == 1)

    flagOut = true;

 

  if (flagIn) {

 

    if (sensor1Last == 1 && sensor2 == 0 && sensor2Last == 0) {

      // Girmekten vazgeçince

      flagIn = false;

 

    } else if ( ( (sensor1Last == 1 || sensor1 == 1) && sensor2 == 1 && sensor2Last == 0) ) {

      // İki sensör önünde bekleyince

 

    } else if ( (sensor1Last == 1 || sensor1 == 0) && sensor2Last == 1 && sensor2 == 0 ) {

      // İçeri girince

      peopleIn++;

      flagIn = false;

      sendData();

 

    }

 

  } else if (flagOut) {

 

    if (sensor2Last == 1 && sensor1 == 0 && sensor1Last == 0) {

      // Çıkmaktan vazgeçince

      flagOut = false;

 

    } else if ( ( (sensor2Last == 1 || sensor2 == 1) && sensor1 == 1 && sensor1Last == 0) ) {

      // İki sensör önünde bekleyince

 

    } else if ( (sensor2Last == 1 || sensor2 == 0) && sensor1Last == 1 && sensor1 == 0 ) {

      // Dışarı çıkınca

      peopleOut++;

      flagOut = false;

      sendData();

 

    }

 

 

  }

 

 

}

 

void readSensors() {

 

  sensor1Last = sensor1;

  sensor2Last = sensor2;

 

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

 

 

  Serial.println("AT Yollandi.Esp modulünden cevap bekleniyor...");

  sentCommand = sendCommandEsp("AT", (char*)"OK", 10);

 

  if (sentCommand)

    Serial.println("Wifi modulune baglanildi.");

  else

  {

    Serial.println("Wifi modulune baglanilamadi!!");

    return;

  }

 

 

  Serial.println("Esp modülünü client olarak ayarlamak için istek yollandi");

  sentCommand = sendCommandEsp("AT+CWMODE=1", (char*)"OK", 10);

 

  if (sentCommand)

    Serial.println("Wifi modülü client olarak ayarlandi.");

  else

  {

    Serial.println("Wifi modülü client olarak ayarlanamadi!!");

    return;

  }

 

  String connectionCommand = "AT+CWJAP=";

  connectionCommand += "\"";

  connectionCommand += WIFI_NAME;

  connectionCommand += "\"";

  connectionCommand += ",";

  connectionCommand += "\"";

  connectionCommand += WIFI_PASSWORD;

  connectionCommand += "\"";

 

  Serial.println("Aga baglaniliyor...");

  sentCommand = sendCommandEsp(connectionCommand, (char*)"OK", 10);

 

  if (sentCommand)

    Serial.println("Aga baglanildi: " + String(WIFI_NAME));

  else

  {

    Serial.println("Aga baglanilamadi: " + String(WIFI_NAME));

    return;

  }

 

  connectToServer();

 

}

 

void connectToServer() {

 

  String connect_ = "\"TCP\"";

  connect_ += ",";

  connect_ += "\"" + HOST + "\"";

  connect_ += ",";

  connect_ += PORT;

 

  Serial.println("Thingspeak serverine baglaniliyor..." + String("(" + connect_ + ")"));

  sentCommand = sendCommandEsp("AT+CIPSTART=" + connect_, (char*)"OK", 10);

 

  if (sentCommand)

    Serial.println("Thingspeak serverine baglanti kuruldu.");

  else

    Serial.println("Thingspeak serverine baglanti kurulamadi !!");

 

}

 

void disconnectFromServer() {

 

 

  Serial.println("Serverden baglanti kopariliyor...");

  sentCommand = sendCommandEsp("AT+CIPCLOSE", (char*)"OK", 10);

 

  if (sentCommand)

    Serial.println("Serverden baglanti koptu.");

  else

    Serial.println("Serverden baglanti kopamadi !");

 

}

 

void sendData() {

 

  connectToServer();

 

  String veri = "GET ";

  veri += "/update?";

  veri += "api_key=" + API_WRITE;

  veri += "&";

  veri += FIELD_PEOPLEIN;

  veri += "=";

  veri += String(peopleIn);

  veri += "&";

  veri += FIELD_PEOPLEOUT;

  veri += "=";

  veri += String(peopleOut);

  veri += "\r\n";

 

  Serial.println("Gönderilecek veri boyutu ayarlandi:" + String(veri.length()));

  Serial.println("Cevap bekleniyor...");

 

  sentCommand = sendCommandEsp("AT+CIPSEND=" + String(veri.length()), (char*)">", 10);

 

  if (sentCommand) {

 

    Serial.println("Veri gönderiliyor...");

 

    Serial3.print(veri);

 

    Serial.println("Veri Gönderildi");

    Serial.println(veri);

 

  } else

    Serial.println("Veri gonderilemedi !!");

 

  disconnectFromServer();

 

}

 

bool sendCommandEsp(String command, char* response, int timeout) {

 

  bool count = 0;

  bool ok = true;

  Serial3.println(command);

  while (!Serial3.find(response)) {

 

    Serial3.println(command);

    if ( (++count) > timeout)

    {

      ok = false;

      break;

    }

 

  }

 

  return ok;

 

}
