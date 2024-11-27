#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClient.h>  // Use WiFiClient for HTTP

#define USE_EDUROAM 1
#define WIFI_USER "zcabake@ucl.ac.uk"
#define WIFI_PASS "sny;z!8A3auX#2VZYG6qC5"
#define WIFI_SSID "eduroam"

const char* pass = WIFI_PASS;

const char* host = "glacial-lowlands-87276-2b36fa9f34ee.herokuapp.com";
const int httpPort = 80;  // Use port 80 for HTTP

WiFiClient client;  // Use WiFiClient for HTTP

#if USE_EDUROAM
#include "esp_eap_client.h"
const char* ssid = "eduroam";
const char* user = WIFI_USER;
#else
const char* ssid = WIFI_SSID;
#endif

void print_wifi_info() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi network: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Gateway address: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("Subnet mask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
  } else {
    Serial.println("WiFi not connected");
    Serial.print("MAC address: ");
    Serial.println(WiFi.macAddress());
  }
}

void wifi_connect(float timeout = 15) {
  unsigned long deadline;

  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

#if USE_EDUROAM
  Serial.printf("Connecting to eduroam as user %s\n", user);
  esp_eap_client_set_identity((uint8_t *)user, strlen(user));
  esp_eap_client_set_username((uint8_t *)user, strlen(user));
  esp_eap_client_set_password((uint8_t *)pass, strlen(pass));
  esp_wifi_sta_enterprise_enable();
  WiFi.begin(ssid);
#else
  Serial.printf("Connecting to %s\n", ssid);
  WiFi.begin(ssid, pass);
#endif
  
  deadline = millis() + (unsigned long)(timeout * 1000);

  while ((WiFi.status() != WL_CONNECTED) && (millis() < deadline)) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  print_wifi_info();
}

void fetch_api_data() {
  HTTPClient http;
  String api_url = "http://glacial-lowlands-87276-2b36fa9f34ee.herokuapp.com/current";
  
  http.begin(api_url);
  int httpCode = http.GET();
  
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println("API Response:");
    Serial.println(payload);
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload); 

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.f_str());
      return;
    }
    
    float ph = doc["ph"];
    float stirring_speed = doc["stirring_speed"];
    float temp = doc["temperature"];
    
    Serial.print("PH: ");
    Serial.println(ph);
    Serial.print("Stirring Speed: ");
    Serial.println(stirring_speed);
    Serial.print("Temperature: ");
    Serial.println(temp);
  } else {
    Serial.println("Error in HTTP request");
  }

  http.end(); 
}

void postData(float ph, float temperature, float stirring_speed) {
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed");
    return;
  }

  String url = "/update";
  String postData = "ph=" + String(ph) + "&temperature=" + String(temperature) + "&stirring_speed=" + String(stirring_speed);

  // Make a HTTP POST request
  client.println("POST " + url + " HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(postData.length());
  client.println();
  client.println(postData);

  // Read the response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
  }

  // Print the response
  String response = client.readString();
  Serial.println("Response:");
  Serial.println(response);

  client.stop();
}

void setup() {
  Serial.begin(115200);

  wifi_connect();
}

void loop() {
  float sample_temp = 20.0;
  float sample_speed = 100.0;
  float sample_ph = 9.0;
  fetch_api_data();
  delay(10000);  
  postData(sample_ph, sample_temp, sample_speed);
}