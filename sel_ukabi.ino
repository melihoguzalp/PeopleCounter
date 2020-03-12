#define TRIG_PIN_1 9

#define ECHO_PIN_1 10

#define TRIG_PIN_2 11

#define ECHO_PIN_2 12

 

// Wifi configuration

#define WIFI_NAME "iPhone"

#define WIFI_PASSWORD "melih123"

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

int inCount, outCount;

bool in = 0, inFlag = false, outFlag = false, goBackFlag = false;

 

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

  if (readSensorValue(0) == 1 && readSensorValue(1) == 0){
    inFlag = true;
    while(inFlag && inCount < 250 ){
      Serial.println(String(readSensorValue(0)) + " " + String(readSensorValue(1)) + " " + String(peopleIn) + " " + String(peopleOut) + " "+ String(inCount));
      if (readSensorValue(0) == 0 && readSensorValue(1) == 1){
        peopleIn += 1;
        while(readSensorValue(1))
           NULL;
       Serial.println(String(readSensorValue(0)) + " " + String(readSensorValue(1)) + " " + String(peopleIn) + " " + String(peopleOut) + " "+ String(inCount));
        break;
      }
      if (readSensorValue(0) == 0)
        goBackFlag = true;
      if (goBackFlag && readSensorValue(0) == 1){
        goBackFlag = false;
        inCount = 0;
      }
     delay(1);
     inCount += 1;
    }
    inCount = 0;
    inFlag = false;
  }

  if (readSensorValue(0) == 0 && readSensorValue(1) == 1){
    outFlag = true;
    while(outFlag && outCount < 250 ){
      Serial.println(String(readSensorValue(0)) + " " + String(readSensorValue(1)) + " " + String(peopleIn) + " " + String(peopleOut) + " " + String(outCount));
      if (readSensorValue(0) == 1 && readSensorValue(1) == 0){
        peopleOut += 1;
        while(readSensorValue(0))
          NULL;
        Serial.println(String(readSensorValue(0)) + " " + String(readSensorValue(1)) + " " + String(peopleIn) + " " + String(peopleOut) + " " + String(outCount));
        break;
      }
      if (readSensorValue(1) == 0)
        goBackFlag = true;
      if (goBackFlag && readSensorValue(1) == 1){
        goBackFlag = false;
        outCount = 0;
      }
      delay(1);
      outCount += 1;
    }
      outCount = 0;
    outFlag = false;
  }

  
 Serial.println(String(readSensorValue(0)) + " " + String(readSensorValue(1)) + " " + String(peopleIn) + " " + String(peopleOut));  
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

  sentCommand = sendCommandEsp("AT", "OK", 10);

 

  if (sentCommand)

    Serial.println("Wifi modulune baglanildi.");

  else

  {

    Serial.println("Wifi modulune baglanilamadi!!");

    return;

  }

 

 

  Serial.println("Esp modülünü client olarak ayarlamak için istek yollandi");

  sentCommand = sendCommandEsp("AT+CWMODE=1", "OK", 10);

 

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

  sentCommand = sendCommandEsp(connectionCommand, "OK", 10);

 

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

  sentCommand = sendCommandEsp("AT+CIPSTART=" + connect_, "OK", 10);

 

  if (sentCommand)

    Serial.println("Thingspeak serverine baglanti kuruldu.");

  else

    Serial.println("Thingspeak serverine baglanti kurulamadi !!");

 

}

 

void disconnectFromServer() {

 

 

  Serial.println("Serverden baglanti kopariliyor...");

  sentCommand = sendCommandEsp("AT+CIPCLOSE", "OK", 10);

 

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

 

  sentCommand = sendCommandEsp("AT+CIPSEND=" + String(veri.length()), ">", 10);

 

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
