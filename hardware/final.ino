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
const char* ssid = "Josu Laptop";
const char* wifi_password = "12345678901";

//const char* ssid = "nabilsphone"; 
//const char* wifi_password = "12345678";*/

/*#define ssid "ufdevice"
#define wifi_password "gogators"*/

//Pin Definitions
#define waterRelay 21
#define moistSensor 36 // NEEDS TO BE 36
#define lightSensor 32 
#define lightRelay 23
#define DHTPIN 4    // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT11   // DHT 11


//Variables for light values
int lightVal;

//Variables for moisture
int moisture;

//DHT sensor
DHT dht(DHTPIN, DHTTYPE);
float humidity;
float cel;
float fahr;

//Variables for heat index
float hif;
float hic;

// Plant 1 ID - 55e3045c-ce3e-4b1c-99cc-b3041edc3dce
// Plant 2 ID - 40bd666e-436e-4544-bdee-f81cb45ff0ba
String plantID = "55e3045c-ce3e-4b1c-99cc-b3041edc3dce";

//Supabase API Setup
String supabase_url = "https://xzissfgunvpzgocavmvi.supabase.co";
String anon_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Inh6aXNzZmd1bnZwemdvY2F2bXZpIiwicm9sZSI6ImFub24iLCJpYXQiOjE2OTkwMzY5MjYsImV4cCI6MjAxNDYxMjkyNn0.yimNI3OEgNYs5dvwwcdO4Lr41yGEj-FE1c-tzofI4AM";

// Supabase credentials
const String email = "hello@sproutscouts.com";
const String password = "test1234";
HTTPClient http;

//Setup for JSON
DynamicJsonDocument readDoc(192);
DynamicJsonDocument writeDoc(192);

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

    //we will take a single reading from the light sensor and store it in the lightCal        
    //variable. This will give us a prelinary value to compare against in the loop
    digitalWrite(waterRelay, LOW);
    digitalWrite(lightRelay, LOW);
    dht.begin();

    // Beginning Supabase Connection
    db.begin(supabase_url, anon_key);

    // Logging in with your account you made in Supabase
    db.login_email(email, password);
}

void loop() {
    Serial.println("**************SENSOR VALUES***************");
    //Read information from moisture sensor
    moisture = analogRead(moistSensor); // lower is wetter, higher is dryer about 1000-3000
    Serial.print("Moisture Sensor: ");
    Serial.println(moisture);
  
    //Read information from light sensor
    lightVal = analogRead(lightSensor);
    Serial.print("Light Sensor Val: ");
    Serial.println(lightVal);
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    /*float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    // Compute heat index in Fahrenheit (the default)
    hif = dht.computeHeatIndex(fahr, humidity);
    // Compute heat index in Celsius (isFahreheit = false)
    hic = dht.computeHeatIndex(cel, humidity, false); 

    //Print DHT data
    Serial.print(F("Humidity Sensor: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(cel);
    Serial.print(F("°C "));
    Serial.print(fahr);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F")); */

    // READ LIGHT VALUES FROM DB
    Serial.println("**************READ LIGHT WATER VALUES FROM DB***************");
    String light = db.from("plants").select("light_enabled").eq("id", plantID).order("id", "asc", true).limit(1).doSelect();
    Serial.println("Light enabled for plant: "); //read values for plant from DB
    //Serial.print(light); //read values for plant from DB

    // Remove brackets 
    light = light.substring(1);
    light = light.substring(0, light.length() - 1);
        
    //Deserialize 
    deserializeJson(readDoc, light);
    bool lightEnabled = readDoc["light_enabled"];
    Serial.println(lightEnabled);

    // Reset Your Query before doing everything else
    db.urlQuery_reset(); 

      
    // READ WATER VALUES FROM DB
    Serial.println("**************READING WATER VALUES FROM DB***************");
    String water = db.from("plants").select("water_enabled").eq("id", plantID).order("id", "asc", true).limit(1).doSelect();
    Serial.println("Water enabled for plant: "); //read values for plant from DB
    //Serial.print(water); //read values for plant from DB
        
    // Remove brackets 
    water = water.substring(1);
    water = water.substring(0, water.length() - 1);
        
    // Deserialize the JSON document
    deserializeJson(readDoc, water);
    bool waterEnabled = readDoc["water_enabled"];
    Serial.println(waterEnabled);

    // Reset Your Query before doing everything else
    db.urlQuery_reset();


    //ACTIVATE WATER RELAY
    if (waterEnabled || moisture > 2000) {
      digitalWrite(waterRelay, HIGH); // Water pump ON
    }
    else {
      digitalWrite(waterRelay, LOW); // Water pump OFF
    }

    //ACTIVATE LIGHT RELAY
    if (lightEnabled || lightVal < 500) {
      digitalWrite(lightRelay, HIGH); // Lamp ON
    }
    else {
      digitalWrite(lightRelay, LOW); // Lamp OFF
    }
    
        //CREATE JSON 
    Serial.println("**************WRITING TO DB***************");
    JSON = "";  //Put your JSON that you want to insert rows

    //writeDoc["temperature"] = fahr;
    //writeDoc["humidity"] = humidity;
    writeDoc["soil_moisture"] = moisture;
    writeDoc["light"] = lightVal;
    if (waterEnabled) {
      writeDoc["water_enabled"] = false;
    }
    serializeJson(writeDoc, JSON);
    Serial.print(F("JSON to DB: "));
    Serial.println(JSON);

    // WRITE TO DB
    table = "plants"; //set table you want to write to
    int code = db.update(table).eq("id", plantID).doUpdate(JSON);
    
    //PRINT RESPONSE CODE
    Serial.print(F("HTTP Status Code: "));
    Serial.println(code);
    
    //RESET
    db.urlQuery_reset();

    //delay process - 3 seconds for demo purposes
    delay(3000);
}
