#define TRIG_PIN_1 9
#define ECHO_PIN_1 10
#define TRIG_PIN_2 11
#define ECHO_PIN_2 12

int peoplein = 0;
int peopleout = 0;

int limitValue = 10; // Sensör tetiklenmesi için cm değeri

int sensor1,sensor2;

bool in = 0;

void setup() {

pinMode(TRIG_PIN_1, OUTPUT); // Sets the trigPin as an Output
pinMode(ECHO_PIN_1, INPUT); // Sets the echoPin as an Input
pinMode(TRIG_PIN_2, OUTPUT); // Sets the trigPin as an Output
pinMode(ECHO_PIN_2, INPUT); // Sets the echoPin as an Input

Serial.begin(9600); // Starts the serial communication

}

void loop() {
  readSensors();

  if(sensor1 == 1 && sensor2 == 0)
    {
      in = 1;
      while(sensor1 != 0 || sensor2 != 0)
      {
        readSensors();
        delay(100);
        
      }
      peoplein += 1;
      
    }
  else if(sensor1 == 0 && sensor2 == 1)
    {

      in = 0;
      while(sensor1 != 0 || sensor2 != 0)
      {
        readSensors();
        delay(100);
        
      }
      peopleout += 1;
      
    }

  Serial.println("In:" + String(peoplein));
  Serial.println("Out:" + String(peopleout));
  
  delay(250);
  
}

void readSensors(){

  sensor1 = readSensorValue(0);
  sensor2 = readSensorValue(1);
  
}
int readSensorValue(int which){

  double duration = 0;
  double distance = 0;

  digitalWrite(which==0?TRIG_PIN_1:TRIG_PIN_2, LOW);
  delayMicroseconds(2); // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(which==0?TRIG_PIN_1:TRIG_PIN_2, HIGH);
  delayMicroseconds(10);
  digitalWrite(which==0?TRIG_PIN_1:TRIG_PIN_2, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(which==0?ECHO_PIN_1:ECHO_PIN_2, HIGH);
  // Calculating the distance
  distance= duration*0.034/2;
  delayMicroseconds(30);

  Serial.println("Mesafe_" + String(which) + ": "+ String(distance));
  return (distance <= limitValue)?1:0;
}
