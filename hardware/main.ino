//Requirements
#include "DHT.h"
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESP32_Supabase.h> 
#include "ArduinoJson.h"

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
const char* ssid = "Josu Laptop";
const char* wifi_password = "12345678901";

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
String anon_key = "";

// Supabase credentials
const String email = "hello@sproutscouts.com";
const String password = "test1234";

//Setup for JSON Document
DynamicJsonDocument doc(192);

void setup() {
    //Establish wifi connection
    Serial.begin(115200);
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

    // Beginning Supabase Connection
    db.begin(supabase_url, anon_key);

    // Logging in with your account you made in Supabase
    db.login_email(email, password);

    // READ FROM DB
    //query 
    String read = db.from("plants").select("*").limit(1).doSelect();
    //.eq("name", "value").order("name", "asc", true).limit(1).doSelect();
    Serial.println(read); //read from DB
    
    //CREATE JSON 
    /*"created_at": "",
    "name": "plant2",
    "owner_id": "",
    "temperature": 6.5,
    "humidity": 7.5,
    "soil_moisture": 10,
    "light": 15*/
    doc["created_at"] = "";
    doc["name"] = "Plant One";
    doc["owner_id"] = "";
    doc["temperature"] = fahr;
    doc["humidity"] = hif;
    doc["soil_moisture"] = moisture;
    doc["light"] = lightVal;
    serializeJson(doc, JSON);
    // This prints:
    // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

    //WRITE TO DB
    table = "plants"; //set table you wanto write to
    //JSON = "";  //Put your JSON that you want to insert rows
    code = db.insert(table, JSON, upsert); //write to db
    Serial.println(code);

    //delay process
    delay(30000);
}
