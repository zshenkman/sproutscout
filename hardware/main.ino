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

//Setup for JSON
DynamicJsonDocument doc(192);

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

    //Set pin mode for relays
    pinMode(waterRelay, OUTPUT);
    pinMode(lightRelay, OUTPUT);
    pinMode(lightRelay, OUTPUT);

    //Initialize light sensor
    lightInit = analogRead(lightSensor);

    //we will take a single reading from the light sensor and store it in the lightCal        
    //variable. This will give us a prelinary value to compare against in the loop
    digitalWrite(waterRelay, HIGH);
    digitalWrite(lightRelay, HIGH);
    dht.begin();

    // Beginning Supabase Connection
    db.begin(supabase_url, anon_key);

    // Logging in with your account you made in Supabase
    db.login_email(email, password);
}

void loop() {
    //Read information from moisture sensor
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

    // READ FROM DB
      //query 
      //String read = db.from("plants").select("*").limit(1).doSelect();
      //.eq("name", "value").order("name", "asc", true).limit(1).doSelect();
      //Serial.println(read); //read from DB
      
    // Reset Your Query before doing everything else
    //db.urlQuery_reset();

    
    //CREATE JSON 
    JSON = "";  //Put your JSON that you want to insert rows
    /*doc["created_at"] = "";
    doc["id"] = "55e3045c-ce3e-4b1c-99cc-b3041edc3dce";
    doc["created_at"] = "";
    doc["owner_id"] = "";
    doc["temperature"] = 7.1;
    doc["humidity"] = 8.9;
    doc["soil_moisture"] = 0;
    doc["light"] = 2;*/

    //TEST VALUES
    /*doc["temperature"] = 7.1;
    doc["humidity"] = 8.9;
    doc["soil_moisture"] = 0;
    doc["light"] = 2;
    serializeJson(doc, JSON);

    doc["temperature"] = 76.8;
    doc["humidity"] = 60.9;
    doc["soil_moisture"] = moisture;
    doc["light"] = lightVal;*/

    doc["temperature"] = fahr;
    doc["humidity"] = humidity;
    doc["soil_moisture"] = moisture;
    doc["light"] = lightVal

    // This prints: Serial.println(JSON);
    // something like this --> {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

    //WRITE TO DB
    table = "plants"; //set table you wanto write to
    int code = db.update(table).eq("id", "55e3045c-ce3e-4b1c-99cc-b3041edc3dce").doUpdate(JSON);
    
    //PRINT RESPONSE CODE
    Serial.print(F("HTTP Status Code: "));
    Serial.println(code);
    
    //RESET
    db.urlQuery_reset();

    //delay process
    delay(30000);
}
