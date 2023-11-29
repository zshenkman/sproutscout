//Requirements
#include "DHT.h"
#include <WiFi.h>
#include <ESP32_Supabase.h> 
#include "ArduinoJson.h"

//Supabase DB Setup
  Supabase db; 
  String table = "plants";// Put your target table 
  String JSON; // Put your JSON that you want to insert rows
  bool upsert = false;
  int code; 

//Wifi Setup
/*const char* ssid = "Josu Laptop";
const char* wifi_password = "12345678901";*/
/*
const char* ssid = "nabilsphone"; 
const char* wifi_password = "12345678";*/

//#define ssid "ufdevice"
//#define wifi_password "gogators"

//Wifi Setup
const char* ssid = "NETGEAR24";
const char* wifi_password = "boldapple026";

//Pin Definitions
#define DHTPIN 4    // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11
#define waterRelay 21
#define moistSensor 36 // NEEDS TO BE 36
#define lightSensor 32
#define lightRelay 23

//DHT sensor
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float cel;
float fahr;

//Variables for light values
//int lightInit;  // initial value
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
const String email = "hello@sproutscouts.com";
const String password = "test1234";
HTTPClient http;

//Setup for JSON
DynamicJsonDocument doc(192);

void setup() {
    //Establish wifi connection
    Serial.begin(9600);
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

    //Initialize light sensor
    //lightInit = analogRead(lightSensor);

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

    //Read information from light sensor
    lightVal = analogRead(lightSensor);
    Serial.print("Light val: ");
    Serial.println(lightVal);
    if(lightVal <  2500)
    {
        digitalWrite(lightRelay, LOW); // turn on light
    }
    else
    {
        digitalWrite(lightRelay, HIGH); // turn off light
    } 
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    //humidity = dht.readHumidity();
    humidity = 13;
    // Read temperature as Celsius (the default)
    //cel = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //fahr = dht.readTemperature(true);
    fahr = 16;
    // Check if any reads failed and exit early (to try again).
    /*if (isnan(humidity) || isnan(cel) || isnan(fahr)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }*/

    /*// Compute heat index in Fahrenheit (the default)
    hif = dht.computeHeatIndex(fahr, humidity);
    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(cel, humidity, false);*/

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

    // READ LIGHT VALUES FROM DB
      //Serial.println("**************READING LIGHT VALUES FROM DB***************");
      //query 
      //String light_1 = db.from("plants").select("*").limit(1).doSelect();
      //.eq("name", "value").order("name", "asc", true).limit(1).doSelect();
        //-------------PLANT 1-------------
        String light_1 = db.from("plants").select("water_enabled").eq("id", "55e3045c-ce3e-4b1c-99cc-b3041edc3dce").order("id", "asc", true).limit(1).doSelect();
        Serial.println("Light enabled for plant 1: "); //read values for plant 2 from DB
        Serial.print(light_1); //read valyes for plant 1from DB
        // Reset Your Query before doing everything else
        
        //Deserialize 
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, light_1);
        bool light1_enabled = doc["light_enabled"];
        //long time          = doc["time"];

        // Reset Your Query before doing everything else
        db.urlQuery_reset();

        //-------------PLANT 2-------------
        String light_2 = db.from("plants").select("light_enabled").eq("id", "40bd666e-436e-4544-bdee-f81cb45ff0ba").limit(1).doSelect();
        Serial.println("Light enabled for plant 2: "); //read values for plant 2 from DB
        Serial.print(light_2); //read values for plant 2 from DB

        //Deserialize 
        deserializeJson(doc, light_2);
        bool light2_enabled = doc["light_enabled"];
        
        Serial.println("");


        // Reset Your Query before doing everything else
        db.urlQuery_reset();
          
    // READ WATER VALUES FROM DB
      Serial.println("**************READING WATER VALUES FROM DB***************");
      //query 
        //-------------PLANT 1-------------
        String water_1 = db.from("plants").select("water_enabled").eq("id", "55e3045c-ce3e-4b1c-99cc-b3041edc3dce").order("id", "asc", true).limit(1).doSelect();
        Serial.println("Water enabled for plant 1: "); //read values for plant 2 from DB
        //Serial.print(water_1); //read valuees for plant 1from DB
        
        // Remove brackets 
        water_1 = water_1.substring(1);
        water_1 = water_1.substring(0, water_1.length() - 1);
        
        // Deserialize the JSON document
        deserializeJson(doc, water_1);
        bool water1_enabled = doc["water_enabled"];
        Serial.println(water1_enabled);

        // Reset Your Query before doing everything else
        db.urlQuery_reset();

        //-------------PLANT 2-------------
        String water_2 = db.from("plants").select("water_enabled").eq("id", "40bd666e-436e-4544-bdee-f81cb45ff0ba").limit(1).doSelect();
        Serial.println("Water enabled for plant 2: "); //read values for plant 2 from DB
        //Serial.print(water_2); //read values for plant 2 from DB

        // Remove brackets 
        water_2 = water_2.substring(1);
        water_2 = water_2.substring(0, water_2.length() - 1);

        //Deserialize 
        deserializeJson(doc, water_2);
        bool water2_enabled = doc["water_enabled"];

        Serial.println(water2_enabled);
        
        // Reset Your Query before doing everything else
        db.urlQuery_reset();

    //ACTIVATE WATER RELAY
    if (water_2) {
      digitalWrite(waterRelay, HIGH);
    }
    else {
      digitalWrite(waterRelay, LOW);
    }
    
    //ACTIVATE LIGHT RELAY
    if (light_2) {
      digitalWrite(LightRelay, HIGH);
    }
    else {
      digitalWrite(LightRela, LOW);
    }
    
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
    doc["light"] = lightVal;
    serializeJson(doc, JSON);
    Serial.println(JSON);

    // This prints: Serial.println(JSON);
    // something like this --> {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

    // WRITE TO DB
    table = "plants"; //set table you want to write to
    // Plant 1 ID - 55e3045c-ce3e-4b1c-99cc-b3041edc3dce
    // Plant 2 ID - 40bd666e-436e-4544-bdee-f81cb45ff0ba
    /*int code = db.update(table).eq("id", "40bd666e-436e-4544-bdee-f81cb45ff0ba").doUpdate(JSON);
    
    //PRINT RESPONSE CODE
    Serial.print(F("HTTP Status Code: "));
    Serial.println(code);*/
    
    //RESET
    db.urlQuery_reset();

    //delay process - 5 seconds for demo purposes
    //delay(3000);
}
