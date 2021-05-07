
// ------ Required libraries, install them if you dont have them installed ------- 

#include "ani.h"
//-- #include "ani2.h"
#include "aeicon.h"
#include "aelogo.h"
#include "aelogogrey.h"
#include "aelogotitle.h"
#include "startminerbutton.h"
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson.git
#include <NTPClient.h>           //https://github.com/taranais/NTPClient

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

#define TFT_GREY 0x5AEB
#define lightblue 0x01E9
#define darkred 0xA041
#define blue 0x5D9B
#define black 0x0000
#define red 0xF800
#define green 0x07E0
#define cyan 0x07FF
#define magenta 0xF81F
#define yellow 0xFFE0
#define white 0xFFFF

#include "Orbitron_Medium_20.h"
#include <WiFi.h>

#include <WiFiUdp.h>
#include <HTTPClient.h>

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;


// --------------------------------- YOUR WIFI ----------------------------------------


const char* ssid     = "mywifi12312123";                        //Edit
const char* password = "mywifipassword";                        //Edit
String town="Rotterdam";                                        //Edit
String Country="NL";                                            //Edit
const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q="+town+","+Country+"&units=metric&APPID=";
const String key = "123123123";                                 //Edit with your key you can get for free from openweathermap.org                 

String payload=""; //whole json 
 String tmp="" ; //temperature
  String hum="" ; //humidity

String payload2=""; //whole json for AE API request
String payload3=""; //whole json for AE Price from coingecko
String payload4=""; //whole json for 2miners 
String payload5=""; //whole json for AE API reaquest for Wallet transactions

//Preparing global variabels to download the data in 
  
  String net = "mainnet"; 

  
// --------------------------- YOUR AETERNITY WALLET ----------------------------------

const String aewallet = "ak_vDE77Qgd2gjopZAVfJihuFfizgv6EKDWWtEpBcg8QpTVnFPq";                              //Edit your wallet 

//const String endpoint2 = "https://" + net + ".aeternal.io/middleware/transactions/account/" + aewallet + "/count";



const String endpoint2 = "https://mainnet.aeternal.io/middleware/names/active?limit=1";

const String endpoint3 = "https://api.coingecko.com/api/v3/simple/price?ids=aeternity&vs_currencies=usd"; // Coingecko AE price request 

//const String endpoint4 = "https://ae.2miners.com/api/accounts/" + aewallet + ""; // 2miners API request    

const String endpoint5 = "https://mainnet.aeternity.io/mdw/txs/count/" + aewallet + ""; // AE Middleware API request for transaction count    

const String endpoint6 = "https://createfeed.fivefilters.org/extract.php?url=https%3A%2F%2Fforum.aeternity.com%2Ftop&in_id_or_class=link-top-line&max=1&order=document"; // Get the latest AE forum posts 





// for the miner to control the main loop
boolean reloop = true;


// miner duino

const char* ducouser = "Techtek";  // Change this to your Duino-Coin username
const char* rigname  = "ESP32";                     // Change this if you want to display a custom rig name in the Wallet
#define LED_BUILTIN     2                           // Change this if your board has built-in led on non-standard pin (NodeMCU - 16 or 2)

#include "mbedtls/md.h" // Include software hashing library
#include "hwcrypto/sha.h" // Include hardware accelerated hashing library
#include <WiFi.h>

TaskHandle_t Task1;
TaskHandle_t Task2;

const char * host = "51.15.127.80"; // Static server IP
const int port = 2811;





StaticJsonDocument<1000> doc;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

int backlight[5] = {10,30,60,120,220};
byte b=1;


String aewalletstripped;




void setup(void) {

  
   pinMode(0,INPUT_PULLUP);
   pinMode(35,INPUT);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);

  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  ledcWrite(pwmLedChannelTFT, backlight[b]);

  // AE boot splashscreen
  tft.fillScreen(TFT_WHITE);
  tft.setSwapBytes(true);
  tft.pushImage(38, 13,  165, 112, aelogo);

  delay(4000);

  Serial.begin(115200);
  tft.fillScreen(TFT_BLACK);
  tft.pushImage(100, 60,  44, 21, aelogogrey);
  tft.print("Blinkit is starting...!");
  tft.print(".");
  tft.println();
    tft.print(".");
  tft.println("");
  tft.print("Connecting to: ");
  tft.println("");
  tft.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    tft.print(".");
  }
  
  tft.println("");
  tft.println("WiFi connected.");
  tft.println("IP address: ");
  tft.println(WiFi.localIP());
  delay(4000);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setSwapBytes(true);
        
          tft.setCursor(2, 232, 1);
          tft.println(WiFi.localIP());



          // AE title
          tft.fillScreen(TFT_WHITE);
          tft.pushImage(45, 3,  135, 22, aelogotitle);
          


           

          // Temp
          tft.setTextColor(TFT_WHITE,lightblue);
          tft.setCursor(4, 152, 2);
          tft.println("TEMP:");

          //Humidity
          tft.setCursor(4, 192, 2);
          tft.println("HUM: ");
          tft.setTextColor(TFT_BLACK,TFT_WHITE);

          //Town
          tft.setFreeFont(&Orbitron_Medium_20);
          tft.setCursor(96, 42);
          tft.println(town);

          //AE Wallet        
          tft.setTextFont(1);
          tft.setCursor(6, 66, 1);
            String aewalletstripped=aewallet;       
            aewalletstripped.remove(12, 300); 
          tft.println(aewalletstripped + "...");
          tft.setTextColor(TFT_GREY,TFT_WHITE);








           tft.fillRect(68,152,1,74,TFT_GREY);

           for(int i=0;i<b+1;i++)
           tft.fillRect(78+(i*7),216,3,10,blue);




// Initialize a NTPClient to get time
  timeClient.begin(); 
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(3600);   /*Edit                    */
  getData();
  delay(500);
  getData2();
  delay(500);
  getData3();
  delay(500);
  //getData4();
  //delay(500);
  getData5();
  delay(500);
  getData6();
  
}


int i=0;
String tt="";
int count=0;
bool inv=1;
int press1=0; 
int press2=0;////

int frame=0;
int frameB=0;
String curSeconds="";








void loop() {

if (reloop == true){


 // tft.pushImage(0, 0,  40, 18, aeicon);


  
  // tft.pushImage(138, 10,  120, 32, ani2[frameB]);
  // frameB++;
   //if(frameB>=10)
   //frameB=0; 

  //tft.pushImage(0, 122,  135, 65, ani[frame]);
   //frame++;
   //if(frame>=10)
   //frame=0;
   
   tft.pushImage(166, 103,  124, 29, startminerbutton);

   


    // Date
    tft.setTextColor(TFT_BLACK,white);
    tft.setTextFont(1);
    tft.setCursor(6, 48);
    tft.println(dayStamp);
    tft.setTextColor(TFT_BLACK,white);

          while(!timeClient.update()) {
          timeClient.forceUpdate();
  }
  // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();
  //Serial.println(formattedDate);

 
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
 
 
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

         //seconds
         if(curSeconds!=timeStamp.substring(6,8)){
         tft.fillRect(8,170,48,28,darkred);
         tft.setTextFont(1);
         tft.setCursor(73, 48);
         tft.println(timeStamp.substring(6,8));
         curSeconds=timeStamp.substring(6,8);
         }

          //clock
         tft.setFreeFont(&Orbitron_Light_24);      
         String current=timeStamp.substring(0,5);
         if(current!=tt)
         {
          tft.fillRect(3,28,85,30,TFT_WHITE);
          tft.setCursor(5, 47);
          tft.println(timeStamp.substring(0,5));
          tt=timeStamp.substring(0,5);
         }
  
  delay(80);
  
 

   if(digitalRead(35)==0){
   if(press2==0)
   {press2=1;
   tft.fillRect(78,216,44,12,TFT_BLACK);
 
   b++;
   if(b>=5)
   b=0;

   for(int i=0;i<b+1;i++)
   tft.fillRect(78+(i*7),216,3,10,blue);
   ledcWrite(pwmLedChannelTFT, backlight[b]);}
   }else press2=0;

   if(digitalRead(0)==0){
   if(press1==0)
   {press1=1;
   inv=!inv;
   tft.invertDisplay(inv);}
  


   // start the miner 
   reloop = false;              // stop the main loop
    tft.fillScreen(TFT_WHITE);

    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE,black);
    tft.setCursor(36, 55, 2);
    tft.println("CORE1");

        tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE,black);
    tft.setCursor(166, 55, 2);
    tft.println("CORE2");
    
   startduinominer();
   
   
   }else press1=0;
   
   if(count==0)
   getData();
   count++;
   if(count>2000)
   count=0;


}
}


void getData2() // get AE Data
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(endpoint2); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
         payload2 = http.getString();
        // Serial.println(httpCode);
        Serial.println(payload2);      
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 char inp[1000];
 payload2.toCharArray(inp,1000);

 deserializeJson(doc,inp);
  
   Serial.println("Tx: "+payload2);
   
 }



// get latest AE registered name
 void getData()
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(endpoint2); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
         payload = http.getString();
        // Serial.println(httpCode);


  payload.replace("name", "");
  payload.replace("hash", "");
  payload.replace("name_hash", "");
  payload.replace("tx_hash", "");
  payload.replace("key", "");
  payload.replace("account_pubkey", "");
  payload.replace("created_at_height", "");
  payload.replace("auction_end_height", "");
  payload.replace("owner", "");
  payload.replace("expires_at", "");
  payload.replace("pointers", "");
  payload.replace("null", "");      
  payload.replace("hashnmt", "");
  payload.replace("{", "");
  payload.replace("}", "");
  payload.replace(":", "");
  payload.replace("\"", "");
  payload.replace("_", "");
  payload.replace(",", "");
  payload.replace("", "");
  payload.remove(25, 300); 
  payload.remove(0, 1);
  payload.replace("chain", "chain                    ");
  payload.remove(25, 300); 
  

        Serial.println(payload);      
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 char inp[1000];
 payload.toCharArray(inp,1000);




 deserializeJson(doc,inp);
  
  String tmp2 = doc["main"]["temp"];
  String hum2 = doc["main"]["humidity"];
  tmp=tmp2;
  hum=hum2;

  
   Serial.println("Temperature"+String(tmp));
   Serial.println("Humidity"+hum);
   Serial.println(town);
   Serial.println("Ae Wallet: "+aewallet);


    // put the latest AE name that is registerd on the screen
    tft.setTextFont(2);
    tft.setTextColor(TFT_BLACK,white);
    tft.setCursor(6, 100, 2);
    tft.println(payload);
   
   
 }



// AE price form coingecko
 void getData3()
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(endpoint3); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
         payload = http.getString();
        // Serial.println(httpCode);


  payload.replace("aeternity", "");
  payload.replace("usd", "");
  payload.replace("{", "");
  payload.replace("}", "");
  payload.replace(":", "");
  payload.replace("\"", "");
  payload.replace("_", "");
  payload.replace(",", "");
  payload.replace("", "");
  payload.remove(4, 4); 

 char inp[1000];
 payload.toCharArray(inp,1000);
String payload3 = payload;
        Serial.println(payload3); 

    // put the AE price on the display 
    tft.setFreeFont(&Orbitron_Medium_20);
    tft.setTextColor(TFT_BLACK,white);
    tft.setCursor(6, 98, 2);
    tft.println("AE: " + payload3);
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 char inp[1000];
 payload.toCharArray(inp,1000);




   
 }


/*
 // Getting your 2miners rig details
 void getData4()
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(endpoint4); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
         payload = http.getString();
        // Serial.println(httpCode);


  payload.replace("aeternity", "");
  payload.replace("usd", "");
  payload.replace("{", "");
  payload.replace("}", "");
  payload.replace(":", "");
  payload.replace("\"", "");
  payload.replace("_", "");
  payload.replace(",", "");
  payload.replace("", "");
  payload.remove(0, 38); 
  payload.remove(8, 120000);
  payload.replace("h", "");
  payload.replace("a", "");
  payload.replace("s", "");
  payload.replace("h", "");
  payload.replace("r", "");
  payload.replace("a", "");
  payload.replace("t", "");
  payload.replace("e", "");

 char inp[1000];
 payload.toCharArray(inp,1000);
String payload4 = payload;
        Serial.println(payload4); 

    // put the Hashrate on the display 
    tft.setFreeFont(&Orbitron_Medium_20);
    tft.setTextColor(TFT_GREY,white);
    tft.setCursor(120, 98, 2);
    tft.println("H/s: " + payload4);
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 char inp[1000];
 payload.toCharArray(inp,1000);




   
 }
*/




// Getting your transaction from the AE middleware API
 void getData5()
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(endpoint5); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
         payload = http.getString();
        // Serial.println(httpCode);


  payload.replace("contract_call_tx", "");
  payload.replace("caller_id", "");
  payload.replace("name_claim_tx", "");
  payload.replace("account_id", "");
  payload.replace("name_preclaim_tx", "");
  payload.replace("account_id", "");
  payload.replace("name_update_tx", "");
  payload.replace("account_id", "");
  payload.replace("spend_tx", "");
  payload.replace("recipient_id", "");
  payload.replace("sender_id", "        ");
  payload.replace("{", "");
  payload.replace("}", "");
  payload.replace(":", "");
  payload.replace("\"", "");
  payload.replace("_", "");
  payload.replace(",", "");
  payload.replace("", "");
  payload.remove(0, 4); 


 char inp[1000];
 payload.toCharArray(inp,1000);
String payload5 = payload;
        Serial.println(payload5); 

    // puts the transaction count for the ae wallet on the screen
    tft.setTextFont(1);
    tft.setTextColor(TFT_BLACK,white);
    tft.setCursor(110, 66, 1);
    tft.println("Tx/Rx: " + payload5);
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 char inp[1000];
 payload.toCharArray(inp,1000);




   
 } 







 // Getting the latest AE forum post
 void getData6()
{
    tft.fillRect(1,170,64,20,TFT_BLACK);
    tft.fillRect(1,210,64,20,TFT_BLACK);
   if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin(endpoint6); //Specify the URL
    int httpCode = http.GET();  //Make the request
 
    if (httpCode > 0) { //Check for the returning code
 
         payload = http.getString();
        // Serial.println(httpCode);


  payload.remove(0, 803);
  payload.replace("title", ""); 
  payload.replace(">", "");
  payload.replace("<", "");
  payload.replace("{", "");
  payload.replace("}", "");
  payload.replace(":", "");
  payload.replace("\"", "");
  payload.replace("_", "");
  payload.replace(",", "");
  payload.replace("", "");
  payload.remove(21, 800); 
  


 char inp[1000];
 payload.toCharArray(inp,1000);
String payload6 = payload;
        Serial.println(payload6); 

    // puts the transaction count for the ae wallet on the screen
    tft.setTextFont(1);
    tft.setTextColor(TFT_BLACK,white);
    tft.setCursor(16, 67, 1);
    tft.println(payload6+"...");
        
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
  }
 char inp[1000];
 payload.toCharArray(inp,1000);




   
 } 







//miner

         // Task1code
void Task1code( void * pvParameters ) {
  unsigned int Shares1 = 0; // Share variable

  Serial.println("\nCORE1 Connecting to Duino-Coin server...");
  // Use WiFiClient class to create TCP connection
  WiFiClient client1;
  client1.setTimeout(2);
  Serial.println("CORE1 is connected: " + String(client1.connect(host, port)));

  String SERVER_VER = client1.readString(); // Server sends SERVER_VERSION after connecting
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led
  Serial.println("CORE1 Connected to the server. Server version: " + String(SERVER_VER));
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led

  while (client1.connected()) {
    Serial.println("CORE1 Asking for a new job for user: " + String(ducouser));
    client1.print("JOB," + String(ducouser) + ",ESP32"); // Ask for new job

    String       hash1 = client1.readStringUntil(','); // Read data to the first peroid - last block hash
    String        job1 = client1.readStringUntil(','); // Read data to the next peroid - expected hash
    unsigned int diff1 = client1.readStringUntil('\n').toInt() * 100 + 1; // Read and calculate remaining data - difficulty
    job1.toUpperCase();
    const char * c = job1.c_str();

    size_t len = strlen(c);
    size_t final_len = len / 2;
    unsigned char* job11 = (unsigned char*)malloc((final_len + 1) * sizeof(unsigned char));
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
      job11[j] = (c[i] % 32 + 9) % 25 * 16 + (c[i + 1] % 32 + 9) % 25;

    byte shaResult1[20];

    Serial.println("CORE1 Job received: " + String(hash1) + " " + String(job1) + " " + String(diff1));
    unsigned long StartTime1 = micros(); // Start time measurement

    for (unsigned long iJob1 = 0; iJob1 < diff1; iJob1++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String hash11 = String(hash1) + String(iJob1);
      const unsigned char* payload1 = (const unsigned char*) hash11.c_str();
      unsigned int payloadLenght1 = hash11.length();

      esp_sha(SHA1, payload1, payloadLenght1, shaResult1);

      if (memcmp(shaResult1, job11, sizeof(shaResult1)) == 0) { // If result is found
        unsigned long EndTime1 = micros(); // End time measurement
        unsigned long ElapsedTime1 = EndTime1 - StartTime1; // Calculate elapsed time
        float ElapsedTimeMiliSeconds1 = ElapsedTime1 / 1000; // Convert to miliseconds
        float ElapsedTimeSeconds1 = ElapsedTimeMiliSeconds1 / 1000; // Convert to seconds
        float HashRate1 = iJob1 / ElapsedTimeSeconds1; // Calculate hashrate
        client1.print(String(iJob1) + "," + String(HashRate1) + ",ESP32 CORE1 Miner v2.3," + String(rigname)); // Send result to server
        String feedback1 = client1.readStringUntil('\n'); // Receive feedback
        Shares1++;
        Serial.println("CORE1 " + String(feedback1) + " share #" + String(Shares1) + " (" + String(iJob1) + ")" + " Hashrate: " + String(HashRate1));
                
        if (HashRate1 < 4000) {
          Serial.println("CORE1 Low hashrate. restarting");
          //client1.stop();     
          //esp_restart();  //Disabed (for now) so it wont restart
        }
        break; // Stop and ask for more work
      }
    }
  }
  Serial.println("CORE1 Not connected. Restarting core 1");
  client1.stop();
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 0);
  vTaskDelete( NULL );
}

//Task2code
void Task2code( void * pvParameters ) {
  unsigned int Shares = 0; // Share variable

  Serial.println("\nCORE2 Connecting to Duino-Coin server...");
  // Use WiFiClient class to create TCP connection
  WiFiClient client;
  client.setTimeout(2);
  Serial.println("CORE2 is connected: " + String(client.connect(host, port)));

  String SERVER_VER = client.readString(); // Server sends SERVER_VERSION after connecting
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led
  Serial.println("CORE2 Connected to the server. Server version: " + String(SERVER_VER));
  digitalWrite(LED_BUILTIN, HIGH);   // Turn off built-in led
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);   // Turn on built-in led

  while (client.connected()) {
    Serial.println("CORE2 Asking for a new job for user: " + String(ducouser));
    client.print("JOB," + String(ducouser) + ",ESP32"); // Ask for new job

    String       hash = client.readStringUntil(','); // Read data to the first peroid - last block hash
    String        job = client.readStringUntil(','); // Read data to the next peroid - expected hash
    unsigned int diff = client.readStringUntil('\n').toInt() * 100 + 1; // Read and calculate remaining data - difficulty
    job.toUpperCase();
    const char * c = job.c_str();

    size_t len = strlen(c);
    size_t final_len = len / 2;
    unsigned char* job1 = (unsigned char*)malloc((final_len + 1) * sizeof(unsigned char));
    for (size_t i = 0, j = 0; j < final_len; i += 2, j++)
      job1[j] = (c[i] % 32 + 9) % 25 * 16 + (c[i + 1] % 32 + 9) % 25;

    byte shaResult[20];

    Serial.println("CORE2 Job received: " + String(hash) + " " + String(job) + " " + String(diff));

    
    unsigned long StartTime = micros(); // Start time measurement

    for (unsigned long iJob = 0; iJob < diff; iJob++) { // Difficulty loop
      yield(); // uncomment if ESP watchdog triggers
      String hash1 = String(hash) + String(iJob);
      const unsigned char* payload = (const unsigned char*) hash1.c_str();
      unsigned int payloadLength = hash1.length();

      mbedtls_md_context_t ctx;
      mbedtls_md_type_t md_type = MBEDTLS_MD_SHA1;
      mbedtls_md_init(&ctx);
      mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
      mbedtls_md_starts(&ctx);
      mbedtls_md_update(&ctx, (const unsigned char *) payload, payloadLength);
      mbedtls_md_finish(&ctx, shaResult);
      mbedtls_md_free(&ctx);

      if (memcmp(shaResult, job1, sizeof(shaResult)) == 0) { // If result is found
        unsigned long EndTime = micros(); // End time measurement
        unsigned long ElapsedTime = EndTime - StartTime; // Calculate elapsed time
        float ElapsedTimeMiliSeconds = ElapsedTime / 1000; // Convert to miliseconds
        float ElapsedTimeSeconds = ElapsedTimeMiliSeconds / 1000; // Convert to seconds
        float HashRate = iJob / ElapsedTimeSeconds; // Calculate hashrate
        client.print(String(iJob) + "," + String(HashRate) + ",ESP32 CORE2 Miner v2.3," + String(rigname)); // Send result to server
        String feedback = client.readStringUntil('\n'); // Receive feedback
        Shares++;
        Serial.println("CORE2 " + String(feedback) + " share #" + String(Shares) + " (" + String(iJob) + ")" + " Hashrate: " + String(HashRate));
       
        if (HashRate < 4000) {
          Serial.println("CORE2 Low hashrate. restarting");
          //client.stop();                
          //esp_restart();  Disabed so it wont restart
        }
        break; // Stop and ask for more work
      }
    }
  }
  Serial.println("CORE2 Not connected. Restarting core 2");
  client.stop();
  xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 2, &Task2, 1);
  vTaskDelete( NULL );
}

/// end miner 




// start mining

void startduinominer() {
  disableCore0WDT();
  disableCore1WDT();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.println("Local IP address: " + WiFi.localIP().toString());
  xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, &Task1, 0); //create a task with priority 1 and executed on core 0
  delay(250);
  xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 2, &Task2, 1); //create a task with priority 2 and executed on core 1
  delay(250);
}
     
