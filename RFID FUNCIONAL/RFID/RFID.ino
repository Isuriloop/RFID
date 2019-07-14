#include <SPI.h>
#include <MFRC522.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
 
constexpr uint8_t RST_PIN = 5;     
constexpr uint8_t SS_PIN = 15;     
MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 
static int uuid;
int lcdColumns = 16;
int lcdRows = 2;
DynamicJsonBuffer jsonBuffer;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 


byte nuidPICC[4];
 
void setup() { 
  Serial.begin(115200);
    WiFi.begin("santy", "ohloldjango");   //WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
 
    delay(500);
    Serial.println("Waiting for connection");
 
  }  
  SPI.begin(); // INICIA SPI bus
  rfid.PCD_Init(); // INICIA MFRC522 
 
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
 
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
    lcd.init();              
    lcd.backlight();
  b
    
}
 
void loop() {
  
    for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    lcd.scrollDisplayLeft();
  }
  delay(500);
 
    
   if(WiFi.status()== WL_CONNECTED){ 
    //Print request response payload
 
  if ( ! rfid.PICC_IsNewCardPresent())
    return;
 
  if ( ! rfid.PICC_ReadCardSerial())
    return;
 
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));
 
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }
 
  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
 
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
   int i;
    Serial.println(F("tarjeta:"));
    Serial.print(F("en hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("en dec: "));
     printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println(i);   
    Serial.println();
    //Serial.println(uuid);
//GETALUMNO

getAlumno(uuid);

    
  }


  else{
  Serial.println(F("tarjeta ya verificada."));
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  getAlumno(uuid);

  }

  }else{
 
    Serial.println("Error in WiFi connection");   
 
 }
}




 void RegistrarAlumno(int uuid) {
  String post="nombre=Isuri&apellidoPaterno=jimenez&apellidoMaterno=lopez&carrera=ids&matricula=173293&rfid=";
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin("http://192.168.43.197:8000/api/v1/alumno/alumno_lista/");      //Specify request destination
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
   int httpCode = http.POST(post+uuid);

   //Send the request000
   String payload = http.getString();                  //Get the response payload
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println("Se registro el alumno :"+payload);
   lcd.clear();
   lcd.print("Se registro el Alumno Isuri");//Print request response payload
   http.end(); 
  }


  
  void getAlumno(int uuid) {
    
HTTPClient http;
String link ="http://192.168.43.197:8000/api/v1/alumno/alumno_detail/";
String index="/";
http.begin(link+uuid+index);
     int httpCode = http.GET();
     String payload = http.getString();
     Serial.println(payload);//Get the request response payload
    JsonObject& root = jsonBuffer.parseObject(payload);
    int id = root["id"];
//     Serial.println(id); 
    if (httpCode > 0) { //Check the returning code
                    //Print the response payload
    if(payload.equals("404")){
      RegistrarAlumno(uuid);
      postRegistro(uuid);
     
    }else{
    JsonObject& root = jsonBuffer.parseObject(payload);
     int id = root["id"];
      ComprobarFecha(id);
    }
    
                }
  http.end();


  }
  void postRegistro(int uuid){
HTTPClient http;
String link ="http://192.168.43.197:8000/api/v1/alumno/alumno_detail/";
String index="/";
http.begin(link+uuid+index);
     int httpCode = http.GET();
     String payload = http.getString();
     Serial.println(payload);//Get the request response payload
     JsonObject& root = jsonBuffer.parseObject(payload);
     int id = root["id"];
  
    RegistrarAsistencia(id);
     http.end();
  }

  void ComprobarFecha(int id){
HTTPClient http;
String link ="http://192.168.43.197:8000/api/v1/asistencia/asistencia_filtrado/";
String index="/";
http.begin(link+id+index);
     int httpCode = http.GET();
     String payload = http.getString();
     Serial.println(payload);//Get the request response payload
    // JsonObject& root = jsonBuffer.parseObject(payload);
  //   int id = root[String("id")];
     //Serial.println(id); 

    if (httpCode > 0) { //Check the returning code
                    //Print the response payload
    if(payload.equals("1")){
      //Serial.println("--------YA SE REGISTRO LA ASISTENCIA DE ESTA FECHA--------"); 
       lcd.clear();
      lcd.print("Ya existe una Asistencia de Hoy");
    }else if(payload.equals("0")) {
      
      RegistrarAsistencia(id);
    }
    
                }
  http.end();


    
  }


  void RegistrarAsistencia(int id) {
   String post ="idAlumno=";
   HTTPClient http;    //Declare object of class HTTPClient
   http.begin("http://192.168.43.197:8000/api/v1/asistencia/asistencia_lista/");      //Specify request destination
   http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //Specify content-type header
   int httpCode = http.POST(post+id);
   //Send the request000
   String payload = http.getString();                  //Get the response payload
   Serial.println(httpCode);   //Print HTTP return code
   Serial.println("Se registro la asistencia "+payload);    //Print request response payload
   lcd.clear();
   lcd.print("Se registro la asistencia ");
   http.end(); 
  }
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }



  
}
 

void printDec(byte *buffer, byte bufferSize) {

String total;
  for (byte i = 0; i < bufferSize; i++) {
   // Serial.print(buffer[i] < 0x10 ? " 0" : " ");
   // Serial.print(buffer[i], DEC);
    uuid=buffer[i], DEC;
    
 
  }
   
}
