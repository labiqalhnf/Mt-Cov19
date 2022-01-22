//Inisialisasi library, pin, variabel, dan kondisi awal
#include <SoftwareSerial.h>
#include <RBDdimmer.h>
#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>

#define outputPin  D2
#define zerocross  D1
//konfigurasi ThingerIO
#define USERNAME "labiqalhnf"
#define DEVICE_ID "MonitorToko_Covid19"
#define DEVICE_CREDENTIAL "Kileuvp8H7kHx7"

//Variabel Serial (Rx,Tx)
SoftwareSerial DataSerial(12,13); //(D6,D7);
dimmerLamp dimmer(outputPin, zerocross);

//Millis - pengganti delay
unsigned long previousMillis = 0; //Mendeteksi waktu trakhir pengecekan
const long interval = 3000; //Melakukan permintaan data setiap 3s
//Variabel array data parsing
String arrayData[5], Status;

float suhu, kelembapan, TingkatVal, speedVal;
int orang, outVal = 0; // Nilai inisialisasi dimmer

//Variabel ThingerIO
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

//Konfigurasi WiFi
const char* ssid = "eL";
const char* password = "batubata3";

void setup() {
  Serial.begin(9600);
  DataSerial.begin(9600);
  dimmer.begin(NORMAL_MODE, ON); //dimmer initialisation: name.begin(MODE, STATE)

  //Koneksi ke WiFi
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED){
  delay(500);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected to Thinger.io");
  //Hubungkan Wemos ke ThingerIO
  thing.add_wifi(ssid, password);
  //Konversi data yang akan dikirim
  thing["Data Toko"] >> [](pson & out){
    out["Suhu"] = suhu;
    out["Kelembapan"] = kelembapan;
    out["Jumlah orang"] = orang;
    out["Exhaust"] = speedVal;
    out["Tingkat"] = TingkatVal;
  };
}

void loop() {
  //konfigurasi millis
  unsigned long currentMillis = millis(); //Baca waktu millis sekarang
  if(currentMillis - previousMillis >= interval) //jika waktu sekarang - waktu sebelumnya > 3s, maka waktunya membaca data kembali
  {
    //Update previousMillis
    previousMillis = currentMillis;
    //Prioritaskan pembacaan data dari arduino
    //Baca Data Serial
    String data = "";
    while(DataSerial.available()>0)
    {
      data += char(DataSerial.read());    
    }
    //Buang spasi data
    data.trim();
    //Pengujian data
    if(data != ""){
      //format data "suhu#kelembaban#pelanggan#speedVal#TingkatVal" = array (setelah diparsing)
      //parsing data (memecah data)
      int index = 0;
      for(int i=0; i<=data.length(); i++)
      {
        char delimiter = '#';
        if(data[i] != delimiter)
          arrayData[index] += data[i];
        else index++; //Variabel index bertambah 1
      }
      //Memastikan data yang dikirim lengkap (suhu, kelembaban, pelanggan, speedVal, TingkatVal)
      if(index == 4)
      {
        //tampilkan data ke Serial monitor
        Serial.print("Suhu          : ");
        Serial.println(arrayData[0]); //suhu
        Serial.print("Kelembapan    : ");
        Serial.println(arrayData[1]); //kelembaban
        Serial.print("Jumlah orang  : ");
        Serial.println(arrayData[2]); //pelanggan
        Serial.print("Kipas         : ");
        Serial.println(arrayData[3]); //speedVal
        Serial.print("Tingkat Bahaya : ");
        Serial.println(arrayData[4]); //TingkatVal
        Serial.println();
      }
      //Isi variabel yang akan dikirim ke Thinger.io
      suhu       = arrayData[0].toFloat();
      kelembapan = arrayData[1].toFloat();
      orang      = arrayData[2].toInt();
      speedVal   = arrayData[3].toInt();
      TingkatVal  = arrayData[4].toFloat();
      //Set kecepatan Kipas
      dimmer.setPower(speedVal);               // Set dimmer power
      //Kirim data ke ThingerIO
      thing.handle();
      //Mengosongkan variabel arrayData
      arrayData[0] = "";
      arrayData[1] = "";
      arrayData[2] = "";
      arrayData[3] = "";
      arrayData[4] = "";
    }
    //Minta data ke Arduino UNO
    DataSerial.println("Ya");
  }
}
