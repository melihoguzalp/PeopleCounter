#define TRIG_PIN_1 9

#define ECHO_PIN_1 2

#define TRIG_PIN_2 11

#define ECHO_PIN_2 3

#define MZ_IN 18

#define MZ_OUT 19

 

//    Interrupt pins: 2, 3, 18, 19, 20, 21 for Arduino Mega

 

 

 

// ---------- Wifi configuration -----------

 

#define WIFI_NAME "oguzalp"

#define WIFI_PASSWORD "658915344"

#define WIFI_BAUDRATE 115200 // Wifi modulünün baudrate hizi

 

// ---------- Wifi configuration -----------

 

 

 

// ----------------WIFI-ESP--------------------

 

void initEsp(); // Wifi baglantisi sagliyor

void connectToServer(); // Servere baglanti saglaniyor

void disconnectFromServer(); // Serverden baglanti kesiliyor

void sendData(); // Thingspeak'e datayi yolluyor

bool sendCommandEsp(String, char*, int); // Esp modulüne komut yollar

 

// ----------------WIFI-ESP--------------------

 

 

 

// ------------------ Sensor ---------------------

 

void readSensors(); // Sensörlerden veri okuyor

int readSensorValue(int); // return= 0: Engel yok , 1: Engel var

void trigSensors(int); // Sensorlerden sinyal yolluyor

void trigIn(); // Sensor tetiklendiginde

void trigOut();

void readMz();

 

// ------------------ Sensor ---------------------

 

 

 

//-------------- Thingspeak configuration---------------

 

String HOST = "api.thingspeak.com";

String PORT = "80";

String CHANNEL_ID = "924094";

String API_WRITE = "8152CS4JYC09WD9I"; // to write data

String API_READ = "BOCVTZ6E5EZ1CK77"; // to read data

String FIELD_PEOPLEIN = "field1";

String FIELD_PEOPLEOUT = "field2";

 

//-------------- Thingspeak configuration---------------

 

 

 

 

bool sentCommand = false;

 

int peopleIn = 0;

int peopleOut = 0;

 

int sensorEngelMesafesi = 20; // Sensör tetiklenmesi için cm degeri

 

int sensor1 = 0, sensor2 = 0;

int sensor1Last = 0, sensor2Last = 0;

 

bool flagIn = false;

bool flagOut = false;

 

int last_stat[2] = {0, 0};

 

int index_stat_in = 0;

int index_stat_in_last = -1;

 

int index_stat_out = 0;

int index_stat_out_last = -1;

 

void setup() {

 

 

  pinMode(TRIG_PIN_1, OUTPUT); // Sets the trigPin as an Output

  pinMode(ECHO_PIN_1, INPUT); // Sets the echoPin as an Input

  pinMode(TRIG_PIN_2, OUTPUT); // Sets the trigPin as an Output

  pinMode(ECHO_PIN_2, INPUT); // Sets the echoPin as an Input

  pinMode(MZ_IN, INPUT);

  pinMode(MZ_OUT, INPUT);

 

  digitalWrite(MZ_IN, HIGH);

  digitalWrite(MZ_OUT, HIGH);

 

  //attachInterrupt(digitalPinToInterrupt(MZ_IN), trigIn, CHANGE);

  //attachInterrupt(digitalPinToInterrupt(MZ_OUT), trigOut, CHANGE);

 

  Serial.begin(9600); // Serial monitoru başlatiyoruz

  Serial3.begin(WIFI_BAUDRATE); // Esp modulü ile haberleşmeyi başlatiyor

 

  //initEsp(); // Esp modulünü kuruyoruz

 

}

 

 

 

void loop() {

 

  readMz();

 

  Serial.println("In: " + String(peopleIn));

  Serial.println("Out: " + String(peopleOut));

  Serial.println("\n\n");

 

}

 

 

void trigIn() {

 

  readMz();

 

}

 

void trigOut() {

 

  readMz();

 

}

 

void readMz() {

 

  //last_stat[0] = digitalRead(MZ_IN);

  //last_stat[1] = digitalRead(MZ_OUT);

 

  last_stat[0] = readSensorValue(0);

  last_stat[1] = readSensorValue(1);

 

  if (last_stat[0] == 0 && last_stat[1] == 1) {

 

    if (index_stat_in_last == -1) {

 

      index_stat_in = 0;

      index_stat_in_last = 0;

 

    }

 

    if (index_stat_out_last == 1) {

 

      index_stat_out = 2;

      index_stat_out_last = 2;

 

    }

 

  } else if (last_stat[0] == 0 && last_stat[1] == 0) {

 

    if (index_stat_in_last == 0) {

 

      index_stat_in = 1;

      index_stat_in_last = 1;

 

    }

 

    if (index_stat_out_last == 0) {

 

      index_stat_out = 1;

      index_stat_out_last = 1;

 

    }

 

  } else if (last_stat[0] == 1 && last_stat[1] == 0) {

 

    if (index_stat_in_last == 1) {

 

      index_stat_in = 2;

      index_stat_in_last = 2;

 

    }

 

    if (index_stat_out_last == -1) {

 

      index_stat_out = 0;

      index_stat_out_last = 0;

 

    }

 

  } else if (last_stat[0] == 1 && last_stat[1] == 1) {

 

    if (index_stat_in_last == 2) {

 

      peopleIn++;

 

    }

 

    if (index_stat_out_last == 2) {

 

      peopleOut++;

 

    }

 

    index_stat_in = 3;

    index_stat_in_last = -1;

 

    index_stat_out = 3;

    index_stat_out_last = -1;

 

  }

 

}

 

void trigSensors(int which) {

 

  digitalWrite(which == 0 ? TRIG_PIN_1 : TRIG_PIN_2, LOW);

  delayMicroseconds(2);

  digitalWrite(which == 0 ? TRIG_PIN_1 : TRIG_PIN_2, HIGH);

  delayMicroseconds(10);

  digitalWrite(which == 0 ? TRIG_PIN_1 : TRIG_PIN_2, LOW);

  delay(50);

 

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

 

  //return (distance <= sensorEngelMesafesi) ? 1 : 0;

  return (distance <= sensorEngelMesafesi) ? 0 : 1;

 

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
