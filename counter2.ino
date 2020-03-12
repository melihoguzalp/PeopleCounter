#define MZ_IN_1 18

#define MZ_OUT_1 19

#define MZ_IN_2 20

#define MZ_OUT_2 21

 

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

 

 

void runAlgorithm();

void readMz1(); // Sol sensorler tetiklendiginde algoritma fonksiyonu

void readMz2(); // Sağ sensorler tetiklendiginde algoritma fonksiyonu

 

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

 

int last_stat_1[2] = {0, 0};

int last_stat_2[2] = {0, 0};

 

int index_in_1 = 0;

int index_in_1_last = -1;

int index_out_1 = 0;

int index_out_1_last = -1;

 

int index_in_2 = 0;

int index_in_2_last = -1;

int index_out_2 = 0;

int index_out_2_last = -1;

 

int data_changed = 0;

int data_changed_last = 0;

 

void setup() {

 

  pinMode(MZ_IN_1, INPUT);

  pinMode(MZ_OUT_1, INPUT);

  pinMode(MZ_IN_2, INPUT);

  pinMode(MZ_OUT_2, INPUT);

 

  digitalWrite(MZ_IN_1, HIGH);

  digitalWrite(MZ_OUT_1, HIGH);

  digitalWrite(MZ_IN_2, HIGH);

  digitalWrite(MZ_OUT_2, HIGH);

 

 

  attachInterrupt(digitalPinToInterrupt(MZ_IN_1), readMz1, CHANGE);

  attachInterrupt(digitalPinToInterrupt(MZ_OUT_1), readMz1, CHANGE);

  attachInterrupt(digitalPinToInterrupt(MZ_IN_2), readMz2, CHANGE);

  attachInterrupt(digitalPinToInterrupt(MZ_OUT_2), readMz2, CHANGE);

 

  Serial.begin(9600); // Serial monitoru başlatiyoruz

  Serial3.begin(WIFI_BAUDRATE); // Esp modulü ile haberleşmeyi başlatiyor

 

  initEsp(); // Esp modulünü kuruyoruz

  sendData(); // İlk açılış için 0 değerini servera yolluyoruz

 

}

 

 

void loop() {

 

  if (data_changed != 0) {

 

    data_changed_last = data_changed;

 

    sendData();

 

    data_changed -= data_changed_last;

 

  }

 

}

 

 

void readMz1() {

 

  last_stat_1[0] = digitalRead(MZ_IN_1);

  last_stat_1[1] = digitalRead(MZ_OUT_1);

  runAlgorithm(1);

 

}

 

void readMz2() {

 

  last_stat_2[0] = digitalRead(MZ_IN_2);

  last_stat_2[1] = digitalRead(MZ_OUT_2);

  runAlgorithm(2);

 

}

 

void runAlgorithm(int i) {

 

  if (i == 1) {

 

    if (last_stat_1[0] == 0 && last_stat_1[1] == 1) {

 

      if (index_in_1_last == -1) {

 

        index_in_1 = 0;

        index_in_1_last = 0;

 

      }

 

      if (index_out_1_last == 1) {

 

        index_out_1 = 2;

        index_out_1_last = 2;

 

      }

 

    } else if (last_stat_1[0] == 0 && last_stat_1[1] == 0) {

 

      if (index_in_1_last == 0) {

 

        index_in_1 = 1;

        index_in_1_last = 1;

 

      }

 

      if (index_out_1_last == 0) {

 

        index_out_1 = 1;

        index_out_1_last = 1;

 

      }

 

    } else if (last_stat_1[0] == 1 && last_stat_1[1] == 0) {

 

      if (index_in_1_last == 1) {

 

        index_in_1 = 2;

        index_in_1_last = 2;

 

      }

 

      if (index_out_1_last == -1) {

 

        index_out_1 = 0;

        index_out_1_last = 0;

 

      }

 

    } else if (last_stat_1[0] == 1 && last_stat_1[1] == 1) {

 

      if (index_in_1_last == 2) {

 

        peopleIn++;

        data_changed++;

        Serial.println("PeopleIn: " + String(peopleIn));

 

      }

 

      if (index_out_1_last == 2) {

 

        peopleOut++;

        data_changed++;

        Serial.println("PeopleOut: " + String(peopleOut));

 

      }

 

      index_in_1 = 3;

      index_in_1_last = -1;

 

      index_out_1 = 3;

      index_out_1_last = -1;

 

    }

 

  } else if (i == 2) {

 

 

    if (last_stat_2[0] == 0 && last_stat_2[1] == 1) {

 

      if (index_in_2_last == -1) {

 

        index_in_2 = 0;

        index_in_2_last = 0;

 

      }

 

      if (index_out_2_last == 1) {

 

        index_out_2 = 2;

        index_out_2_last = 2;

 

      }

 

    } else if (last_stat_2[0] == 0 && last_stat_2[1] == 0) {

 

      if (index_in_2_last == 0) {

 

        index_in_2 = 1;

        index_in_2_last = 1;

 

      }

 

      if (index_out_2_last == 0) {

 

        index_out_2 = 1;

        index_out_2_last = 1;

 

      }

 

    } else if (last_stat_2[0] == 1 && last_stat_2[1] == 0) {

 

      if (index_in_2_last == 1) {

 

        index_in_2 = 2;

        index_in_2_last = 2;

 

      }

 

      if (index_out_2_last == -1) {

 

        index_out_2 = 0;

        index_out_2_last = 0;

 

      }

 

    } else if (last_stat_2[0] == 1 && last_stat_2[1] == 1) {

 

      if (index_in_2_last == 2) {

 

        peopleIn++;

        data_changed++;

        Serial.println("PeopleIn: " + String(peopleIn));

 

      }

 

      if (index_out_2_last == 2) {

 

        peopleOut++;

        data_changed++;

        Serial.println("PeopleOut: " + String(peopleOut));

 

      }

 

      index_in_2 = 3;

      index_in_2_last = -1;

 

      index_out_2 = 3;

      index_out_2_last = -1;

 

    }

 

 

  }

 

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
