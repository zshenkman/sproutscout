 n#include "Forecast.h"
#include <string.h>


String httpGET(String server) {
  HTTPClient http;
  
  http.begin(server.c_str());
  
  int responseNum = http.GET();
  
  String payload = "{}";
  
  if (responseNum > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(responseNum);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(responseNum);
  }
  http.end();
  
  return payload;
}

Forecast Deserialize(String jsonPayload, String tim) {
  DynamicJsonDocument doc(26670);
  DeserializationError error = deserializeJson(doc, jsonPayload);
  
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }

  unsigned int day = doc["current"]["is_day"];
  unsigned int rain;
  String currentTime = doc["location"]["localtime"];
  currentTime = currentTime.substring(11,13);
  if(tim == "X") {
    rain = doc["forecast"]["forecastday"][0]["day"]["daily_will_it_rain"];
  }
  else {
    rain = doc["forecast"]["forecastday"][0]["hour"][0]["will_it_rain"];
  }
  //Serial.println(day);
  //Serial.println(rain);
  Forecast currentForecast(day, rain, currentTime);
  return currentForecast;
}

Forecast getForecast(String location, String host, String key, String tim) {
  if(tim == "X") {
    String path = host + "forecast.json?key=" + key + "&q=" + location + "&days=1";
    String getted = httpGET(path);
    
    Forecast currentForecast = Deserialize(getted, tim);
    return currentForecast;
  }
  else {
    String path = host + "forecast.json?key=" + key + "&q=" + location + "&days=1&hour=" + tim;
    String getted = httpGET(path);
    
    Forecast currentForecast = Deserialize(getted, tim);
    return currentForecast;
  }
}

boolean willRain(String location, String host, String key, String tim) {
  if(tim == "X") {
    Forecast currentForecast = getForecast("Gainesville", host, key, "X");
    return currentForecast.willRain();
  }
  else {
    Forecast currentForecast = getForecast("Gainesville", host, key, tim);
    return currentForecast.willRain();
  }
}

String getCurrentTime(String location, String host, String key) {
    Forecast currentForecast = getForecast("Gainesville", host, key, "X");
    return currentForecast.getTime();
}

boolean isDay(String location, String host, String key) {
  Forecast currentForecast = getForecast("Gainesville", host, key, "X");
  return currentForecast.isDay();
}