//inisialisasi library, pin, variabel, dan kondisi awal
#include <SPI.h>
#include <SimpleTimer.h>
#include <DHT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ECHO_PIN1 2
#define TRIG_PIN1 3
#define ECHO_PIN2 4
#define TRIG_PIN2 5
#define SERVOPIN 6
#define DHTPIN 7

SimpleTimer timer;
DHT dht(DHTPIN, DHT11);
Servo myservo;
LiquidCrystal_I2C lcd(0x27, 20, 4);

int pelanggan, currentPeople = 0, timeoutCounter = 0,
    speedVal = 0;
float sensor1Val, sensor2Val, suhu, kelembaban,
      uSangatDingin, uDingin, uSejuk, uPanas, uSangatPanas,
      uKering, uNormal, uBasah,
      uSangatSedikit, uSedikit, uSedang, uBanyak, uSangatBanyak,
      statusVal;
String sequence = "", outputFuzzy;
//Fungsi pembacaan jarak sensor ultrasonik
float measureDistance(int PIN_T, int PIN_E){
  pinMode(PIN_T,OUTPUT);
  digitalWrite(PIN_T,LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_T,HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_T,LOW);
  pinMode(PIN_E,INPUT);
  long duration = pulseIn(PIN_E,HIGH);
  return duration * 0.034/2;
}
//Fungsi pemetaan nilai input - defuzzifikasi
float hitungMember(int pilih, float Nilai, float A, float B, float C){
  switch(pilih){
    case 1: 
      if (Nilai >= A && Nilai <= B) {return 1;}
      if (Nilai > B && Nilai < C) {return (C - Nilai) / (C - B);}
      if (Nilai >= C) {return 0;}
      break;
    case 2: 
      if (Nilai <= A || Nilai >= C){ return 0;}
      if (Nilai > A && Nilai < B){ return (Nilai - A) / (B - A);}
      if (Nilai > B && Nilai < C){ return (C - Nilai) / (C - B);}
      if (Nilai == B){ return 1;}
      break;
    case 3: 
      if (Nilai <= A){ return 0;}
      if (Nilai > A && Nilai < B){ return (Nilai - A) / (B - A);}
      if (Nilai >= B){ return 1;}
      break;
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  myservo.attach(SERVOPIN);
  myservo.write(90);
  Wire.begin();
  lcd.begin(20, 4);
  lcd.backlight();
  timer.setInterval(2000, readDHT);
}

void loop() {
  timer.run();
  countPeople();
  fuzzyLogic();
  String minta = "";
  while(Serial.available()>0){
    minta += char(Serial.read());
  }
  //Buang spasi data yang diterima
  minta.trim();
  //Menguji variabel minta
  if(minta == "Ya"){
    //kirim data
    kirimData();
  }
  //Mengosongkan variabel minta
  minta = "";
}
