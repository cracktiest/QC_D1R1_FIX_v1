#include <Arduino.h>
#include <rdm6300.h>
#include <ESP8266WiFi.h>
#include <TextFinder.h>
#include <IRremote.h>
#include <stdlib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define RDM6300_RX_PIN 13 // can be only 13 - on esp8266 force hardware uart!
#define READ_LED_PIN 16
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
int LED_B = D10;
unsigned long starmilis, curenmilis;
const unsigned long periode =5000;
const int IR_PIN =D5;
WiFiClient client;
TextFinder  finder(client);
char webtext[25];
const char* ssid     = "ROBOT V1";
const char* router   = "ROUTER 1";
const char* password = "robot%8888";
const char* host ="10.5.2.222"; //10.5.0.108
const int port = 80;
int ulangclient;
String nama,line,output,good,rework,reject,balance;
IPAddress local_IP(10,5,4,72);//249
IPAddress gateway(10,5,1,1);//(10,5,0,30);
IPAddress subnet(255,255,0,0);
char* id_device ="QC-1"; //41
char* link = "GM1";
String urlMode; 
String urlID;
String urlText;
Rdm6300 rdm6300;
IRrecv irDetect(IR_PIN);
decode_results irIn;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void(* Reset) (void) = 0;

void beep (int bep){
  digitalWrite(LED_B, HIGH); 
  delay(bep);
  digitalWrite(LED_B, LOW);
  delay(bep);
}



void bacadata(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  if (!client.connect(host, port)) {
    Serial1.println("Server Unconnect");
    lcd.setCursor(0,0);
    lcd.print("SERVER UNCONNECT");
    lcd.setCursor(0,1);
    lcd.print("  PLEASE  WAIT  ");
    beep(100);
    Reset();
  }
  urlText = "/robotik/monitoring/lihatdataQC.php?id_device=";
  urlText += id_device;
  client.print(String("GET ")+urlText+" HTTP/1.1\r\n" +"Host: "+host+ "\r\n" +"Connection: close\r\n\r\n");
  ulangclient=0;       
  while(!client.available()){
    delay(1);
    ulangclient++;
    if(ulangclient>=5000){
      lcd.setCursor(0,0);
      lcd.print("SERVER UNRESPOND");
      lcd.setCursor(0,1);
      lcd.print("SILAHKAN  TUNGGU");
      Reset();
    }
  }
  Serial1.print("Latency reading from server : "); Serial1.print(ulangclient); Serial1.println(" ms");
  lcd.clear();
  while (client.available()){
  Serial1.println(urlText);
  if (finder.getString("Nama =","|",webtext,25) !=0){
    nama = webtext;
    Serial1.print("Nama = "); Serial1.println(nama);
    display.setCursor(0, 0);
    display.println(nama); 
    }
  if (finder.getString("Line =","|",webtext,25) !=0){
    line = webtext;
    lcd.setCursor(0,0);  lcd.print("L:");
    lcd.print(line);
    Serial1.print("Line= "); Serial1.println(line);
    display.setCursor(0, 9);
    display.println(line); 
    }
  if (finder.getString("Output =","|",webtext,25) !=0){
    output = webtext;
    lcd.setCursor(9,0);lcd.print("O: ");
    lcd.print(output);
    Serial1.print("Output = "); Serial1.println(output);
    display.setCursor(0, 18);
    display.print("OUTPUT LINE :"); display.print(output);
    }
  if (finder.getString("Good =","|",webtext,25) !=0){
    good = webtext;
    lcd.setCursor(0,1);lcd.print(good);
    lcd.setCursor(5,1);lcd.print('/');
    Serial1.print("Good = "); Serial1.println(good);
    display.setCursor(0, 27);
    display.print("GOOD SEWING :"); display.print(good);
    }
  if (finder.getString("Rework =","|",webtext,25) !=0){
    rework = webtext;
    lcd.setCursor(7,1);lcd.print(rework);
    lcd.setCursor(11,1);lcd.print('/');
    Serial1.print("Rework = "); Serial1.println(rework);
    display.setCursor(0, 36);
    display.print("REWORK      :"); display.print(rework);
    }
  if (finder.getString("Reject =","|",webtext,25) !=0){
    reject = webtext;
    lcd.setCursor(13,1);lcd.print(reject);
    Serial1.print("Reject = "); Serial1.println(reject);
    display.setCursor(0, 45);
    display.print("REJECT      :"); display.print(reject);
    }
  if (finder.getString("Balance =","|",webtext,25) !=0){
    balance = webtext;
    Serial1.print("Balance = "); Serial1.println(balance);
    display.setCursor(0, 54);
    display.print("BALANCE     :"); display.print(balance);
    }
   if(line == "TAP ID"){
    lcd.setCursor(0,0); lcd.print("TAP ID CARD ANDA");
    lcd.setCursor(4,1); lcd.print("QC AKHIR");
   }
  Serial1.println("Serial ended");
  //Serial1.print(client);
  }
  display.display();
}


void tombol(String key){
  beep(100);
  urlMode = "/robotik/monitoring/tombolQC.php?tombol=";
  urlMode += key;
  urlMode += "&id_device=";
  urlMode += id_device;
  urlMode += "&link=";
  urlMode += link;
  if (client.connect(host, port)) {
        client.print(String("GET ")+urlMode+" HTTP/1.1\r\n" +"Host: "+host+ "\r\n" +"Connection: close\r\n\r\n");
        Serial1.println(urlMode);  
        ulangclient=0;
        while(!client.available()){
          delay(1);
          ulangclient++;
          if(ulangclient>=5000){
            lcd.setCursor(0,0);
            lcd.print("SERVER UNRESPOND");
            lcd.setCursor(0,1);
            lcd.print("SILAHKAN  TUNGGU");
            Reset();
          }
        }
        Serial1.print("Latency writing to server : "); Serial1.println(ulangclient);
      }        
      bacadata();
}

void decodeIR() // Indicate what key is pressed
{   
  Serial1.println(irIn.value,HEX);
  switch (irIn.value) { // compare the value to the following cases
  
    case 0xFFA25D: // if the value is equal to 0xFD00FF
      Serial1.println("Tombol 1 ditekan");
      tombol("1");
      break;
    case 0xFF629D:
      Serial1.println("Tombol 2 ditekan");
      tombol("2");
      break;
    case 0xFFE21D:
      Serial1.println("Tombol 3 ditekan");
      tombol("3");
      break;
    case 0xFF22DD:
      Serial1.println("Tombol 4 ditekan");
      tombol("4");
      break;
    case 0xFF02FD:
      Serial1.println("Tombol 5 ditekan");
      tombol("5");
      break ;
    case 0xFFC23D:
      Serial1.println("Tombol 6 ditekan");
      tombol("6");
      break ;
    case 0xFFE01F:
      Serial1.println("Tombol 7 ditekan");
      tombol("7");
      break ;
    case 0xFFA857:
      Serial1.println("Tombol 8 ditekan");
      tombol("8");
      break ;
    case 0xFF906F:
      Serial1.println("Tombol 9 ditekan");
      tombol("9");
      break;
    case 0xFF38C7:
      Serial1.println("Tombol OK ditekan");
      tombol("OK");
      break;
    case 0xFF6897:
      lcd.setCursor(0,0); lcd.print(" PROGRAM  RESET ");
      beep(500);
      Reset();
      break;
    case 0xFFB04F:
      beep(500);
      return setup();
      break;
      default:
      beep(200);
      bacadata();
      break;
  }
}

void rfid(){
  if (rdm6300.update()){
    beep(100);
    String getID= String(rdm6300.get_tag_id(), HEX);
    Serial1.println(getID);
    digitalWrite(READ_LED_PIN, rdm6300.is_tag_near());
    delay(10);
    urlID = "/robotik/monitoring/getIDQC.php?nokartu=" ;
    urlID += getID;
    urlID +="&id_device=";
    urlID += id_device;
    urlID += "&link=";
    urlID += link;   
  if (!client.connect(host, port)) {
    Serial1.println("Server Unconnect");
    lcd.setCursor(0,0);
    lcd.print("SERVER UNCONNECT");
    beep(100);
    Reset();
  }
  if (client.connect(host, port)) {
    client.print(String("GET ")+urlID+" HTTP/1.1\r\n" +"Host: "+host+ "\r\n" +"Connection: close\r\n\r\n");  
    Serial1.println(urlID);
    // delay(100);  
    ulangclient = 0;
         while(!client.available()){
           delay(1);
           ulangclient++;
           if(ulangclient>=5000){
             lcd.setCursor(0,0);
             lcd.print("SERVER UNRESPOND");
             lcd.setCursor(0,1);
             lcd.print("SILAHKAN  TUNGGU");
             Reset();
           }
         }
  }
   Serial1.print("Latency from writing rfid to server : "); Serial1.println(ulangclient);      
  rdm6300.end();
  rdm6300.begin(RDM6300_RX_PIN);
  bacadata();
  } 
}


void setup() {
  Serial1.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial1.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(3);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("PT. GGI"));
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println("Majalengka 1         ");
  display.println("ROBOTIC & DEVELOPMENT");

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print("QC FINAL");

  display.display();
  Serial1.print("Connecting to ");
  Serial1.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode(READ_LED_PIN, OUTPUT);
  pinMode(LED_B, OUTPUT);
  digitalWrite(READ_LED_PIN, HIGH);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  irDetect.enableIRIn(); 
  rdm6300.begin(RDM6300_RX_PIN);
  WiFi.config(local_IP,gateway,subnet);
  while (WiFi.status() != WL_CONNECTED) {
    beep(50);
    Serial1.print(".");
    lcd.setCursor(0,0);
    lcd.print("Wifi  Connecting");
    lcd.setCursor(0,1);
    lcd.print("  to  ");
    lcd.print(router); lcd.print("  ");
  }
  Serial1.println("");
  Serial1.print("WiFi connected to ");
  Serial1.println(ssid);
  Serial1.println("IP address: ");
  Serial1.println(WiFi.localIP());
  lcd.setCursor(0,0);
  lcd.print(" Wifi connected ");
  bacadata();
}

void loop() 
{
//  curenmilis=millis();
//  if(curenmilis-starmilis>periode)
//    {
//    bacadata();
//    starmilis=curenmilis;
//    }
  if (irDetect.decode(&irIn)) 
  {
    decodeIR(); 
    IrReceiver.resume(); 
  }
  rfid();
}
