//Requirements
#include "DHT.h"
#include <WiFi.h>
#include <ESP32_Supabase.h> 
#include "ArduinoJson.h"
#include <HTTPClient.h>
#include <Wire.h>
#include <RTClib.h>


//Supabase DB Setup
  Supabase db; 
  String table = "plants";// Put your target table 
  String JSON; // Put your JSON that you want to insert rows
  bool upsert = false;
  int code; 

//Wifi Setup
/*
ssid = Josu Laptop
wifi_password = 12345678901
*/
/*const char* ssid = "Josu Laptop";
const char* wifi_password = "12345678901";

const char* ssid = "nabilsphone"; 
const char* wifi_password = "12345678";

#define SSID "ufguest"
#define WIFI_PASS ""*/

//Wifi Setup
const char* ssid = "NETGEAR24";
const char* wifi_password = "boldapple026";

//Pin Definitions
#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define waterRelay 21
#define moistSensor 36 // NEEDS TO BE 36
#define lightSensor 15
#define lightRelay 23

//DHT sensor
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float cel;
float fahr;

//Variables for light values
int lightInit;  // initial value
int lightVal;

//Variables for moisture
int moisture;

//Variables for heat index
float hif;
float hic;

//Supabase API Setup
String supabase_url = "https://xzissfgunvpzgocavmvi.supabase.co";
String anon_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Inh6aXNzZmd1bnZwemdvY2F2bXZpIiwicm9sZSI6ImFub24iLCJpYXQiOjE2OTkwMzY5MjYsImV4cCI6MjAxNDYxMjkyNn0.yimNI3OEgNYs5dvwwcdO4Lr41yGEj-FE1c-tzofI4AM";

// Supabase credentials
const String email = "adassac@outlook.com";
const String password = "headstrong";
HTTPClient http;

//Setup for JSON Document
DynamicJsonDocument doc(192);

//Real Time Clock (RTC)
RTC_DS3231 rtc;
String t = "";

void setup() {
    //Establish wifi connection
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    /*//Set pin mode for relays
    pinMode(waterRelay, OUTPUT);
    pinMode(lightRelay, OUTPUT);
    pinMode(lightRelay, OUTPUT);

    //Initialize light sensor
    lightInit = analogRead(lightSensor);

    //we will take a single reading from the light sensor and store it in the lightCal        
    //variable. This will give us a prelinary value to compare against in the loop
    digitalWrite(waterRelay, HIGH);
    digitalWrite(lightRelay, HIGH);
    dht.begin();*/

    // Beginning Supabase Connection
    db.begin(supabase_url, anon_key);

    // Logging in with your account you made in Supabase
    db.login_email(email, password);

    if (!rtc.begin()) {
      Serial.println("Couldn't find RTC");
      while (1);
    }
    if (rtc.lostPower()) {
      Serial.println("RTC lost power, let's set the time!");
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set the RTC to the date & time this sketch was compiled
    }
}

void loop() {
  //REAL TIME CLOCK 
    /*DateTime now = rtc.now();
    t = t + (now.year(), DEC);
    t = t + "/";
    t = t + (now.month(), DEC);
    t = t + "/";
    t = t + (now.day(), DEC);
    t = t + " ";
    t = t + (now.hour(), DEC);
    t = t + ":";
    t = t + (now.minute(), DEC);
    t = t + ":";
    t = t + (now.second(), DEC);*/

    /*Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();*/

    /*//Read information from moisture sensor
    moisture = analogRead(moistSensor); // lower is wetter, higher is dryer about 1000-3000
    Serial.print("Moisture: ");
    Serial.println(moisture);
    if (moisture < 2500) { // more wet
        digitalWrite(waterRelay, LOW);
        //Serial.println("Relay on");
    } else {
        digitalWrite(waterRelay, HIGH);
        //Serial.println("Relay off");
    }
    delay(2000);  
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    humidity = dht.readHumidity();
    // Read temperature as Celsius (the default)
    cel = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    fahr = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(cel) || isnan(fahr)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    // Compute heat index in Fahrenheit (the default)
    hif = dht.computeHeatIndex(fahr, humidity);
    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(cel, humidity, false);

    //Print data
    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(cel);
    Serial.print(F("°C "));
    Serial.print(fahr);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F")); 

    lightVal = analogRead(lightSensor);
    Serial.print("Light diff: ");
    Serial.println(lightVal - lightInit);
    if(lightVal - lightInit <  50)
    {
        digitalWrite(lightRelay, LOW); // turn on light
    }
    else
    {
        digitalWrite(lightRelay, HIGH); // turn off light
    }
    */

    // READ FROM DB
      //query 
      //String read = db.from("plants").select("*").limit(1).doSelect();
      //.eq("name", "value").order("name", "asc", true).limit(1).doSelect();
      //Serial.println(read); //read from DB
      
    // Reset Your Query before doing everything else
    //db.urlQuery_reset();

    
    //CREATE JSON 
    JSON = "";  //Put your JSON that you want to insert rows
    /*"created_at": "",
    "name": "plant2",
    "owner_id": "",
    "temperature": 6.5,
    "humidity": 7.5,
    "soil_moisture": 10,
    "light": 15*/

    //doc["created_at"] = "";
    //doc["name"] = "Plant Two";
    doc["id"] = 2;
    doc["created_at"] = "";
    //doc["owner_id"] = "";
    //doc["temperature"] = 0;
    //doc["humidity"] = 0;
    //doc["soil_moisture"] = 0;
    //doc["light"] = 0;
    serializeJson(doc, JSON);

    Serial.println(JSON);

    // This prints:
    // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

    //WRITE TO DB
    table = "Plants"; //set table you wanto write to
    code = db.insert(table, JSON, true); //write to db
    //Print Response
    Serial.print(F("HTTP Status Code: "));
    Serial.println(code);

    /*http.begin(supabase_url);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", anon_key);

    // Data to send (modify this based on your schema)
    String postData = "{\"name\": \"plant2\"}";

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();*/
    
    //RESET
    db.urlQuery_reset();

    //delay process
    delay(30000);
}
