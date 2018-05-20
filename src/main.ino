/*
Based on the Bruh Automation sensor hardware and original code. Modified, heavily :), by Shawn Corey.

Original code is available at: https://github.com/bruhautomation/ESP-MQTT-JSON-Multisensor


To use this code you will need the following dependancies:sendstate

- Support for the ESP8266 boards.
    - You can add it to the board manager by going to File -> Preference and pasting http://arduino.esp8266.com/stable/package_esp8266com_index.json into the Additional Board Managers URL field.
    - Next, download the ESP8266 dependancies by going to Tools -> Board -> Board Manager and searching for ESP8266 and installing it.

- You will also need to download the follow libraries by going to Sketch -> Include Libraries -> Manage Libraries
    - DHT sensor library
    - Adfruit unified sensor
    - PubSubClient
    - ArduinoJSON

*/

// The packet for the LED discovery registration packet is really big so need to make the max packet size bigger.
// If it still fails then edit the PubSubClient.h file directly,
// had problems without changing the .h file so look there if it fails to register the LED
#define MQTT_MAX_PACKET_SIZE 768

#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"
#include "user_config.h"






WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);


void setup() {
    Serial.begin(115200);

    pinMode(PIRPIN, INPUT);
    pinMode(DHTPIN, INPUT);
    pinMode(LDRPIN, INPUT);

    Serial.begin(115200);
    delay(10);

    Serial.print("calibrating sensor ");
    for (int i = 0; i < calibrationTime; i++) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("Starting Node named " SENSORNAME);

    setup_wifi();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    ArduinoOTA.setPort(OTAport);
    ArduinoOTA.setHostname(SENSORNAME);
    ArduinoOTA.setPassword((const char *)OTApassword);

    ArduinoOTA.onStart([]() {
        Serial.println("Starting");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        switch(error){
            case OTA_AUTH_ERROR: Serial.println("Auth Failed"); break;
            case OTA_BEGIN_ERROR: Serial.println("Begin Failed"); break;
            case OTA_CONNECT_ERROR: Serial.println("Connect Failed"); break;
            case OTA_RECEIVE_ERROR: Serial.println("Receive Failed"); break;
            case OTA_END_ERROR: Serial.println("End Failed"); break;
            default: Serial.println("Unknown Error"); break;
        }
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    configColor(red, green, blue);
}


void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);

    WiFi.mode(WIFI_STA);
    WiFi.hostname(SENSORNAME);
    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}


bool sendState(const char* topic, const char* message, bool retain = true) {
    return client.publish(topic, message, retain);
}


void sendAllState() {
    // Send LED status
    sprintf(message_buff, "%s", (stateOn) ? MQTT_ON_CMD : MQTT_OFF_CMD);
    sendState(DEVICE_LED_STATE_TOPIC, message_buff);

    // Send LED RGB values
    sprintf(message_buff, "%d,%d,%d", red, green, blue);
    sendState(DEVICE_LED_RGB_STATE_TOPIC, message_buff);

    // Send LED Brighness value
    sprintf(message_buff, "%d", brightness);
    sendState(DEVICE_LED_BRIGHTNESS_STATE_TOPIC, message_buff);

    // Send Humidity value
    dtostrf(humValue, 4, 2, str_buff);
    sprintf(message_buff, "%s", str_buff);
    sendState(DEVICE_HUMIDITY_STATE_TOPIC, message_buff);

    // Send Temperature value
    dtostrf(tempValue, 4, 2, str_buff);
    sprintf(message_buff, "%s", str_buff);
    sendState(DEVICE_TEMP_STATE_TOPIC, message_buff);

    // Send Light sensor value
    sprintf(message_buff, "%d", LDR);
    sendState(DEVICE_LDR_STATE_TOPIC, message_buff);

    // Send Motion status
    sprintf(message_buff, "%s", (motionStatus) ? MQTT_ON_CMD : MQTT_OFF_CMD);
    sendState(DEVICE_PIR_STATE_TOPIC, message_buff);
}


void registerSensors(bool forceRegister = false){
    sendState(DEVICE_PIR_DISCOVERY_TOPIC, DEVICE_PIR_DISCOVERY_REGISTER_MESSAGE);
    sendState(DEVICE_TEMP_DISCOVERY_TOPIC, DEVICE_TEMP_DISCOVERY_REGISTER_MESSAGE);
    sendState(DEVICE_HUMIDITY_DISCOVERY_TOPIC, DEVICE_HUMIDITY_DISCOVERY_REGISTER_MESSAGE);
    sendState(DEVICE_LDR_DISCOVERY_TOPIC, DEVICE_LDR_DISCOVERY_REGISTER_MESSAGE);
    sendState(DEVICE_LED_DISCOVERY_TOPIC, DEVICE_LED_DISCOVERY_REGISTER_MESSAGE);
}

void unregisterSensors(){
    if(!sendState(DEVICE_PIR_DISCOVERY_TOPIC, ""))
        Serial.println("Failed to unregister motion sensor");

    if(!sendState(DEVICE_TEMP_DISCOVERY_TOPIC, ""))
        Serial.println("Failed to unregister temperature sensor");

    if(!sendState(DEVICE_HUMIDITY_DISCOVERY_TOPIC, ""))
        Serial.println("Failed to unregister humidity sensor");

    if(!sendState(DEVICE_LDR_DISCOVERY_TOPIC, ""))
        Serial.println("Failed to unregister LDR sensor");

    if(!sendState(DEVICE_LED_DISCOVERY_TOPIC, ""))
        Serial.println("Failed to unregister LED");
}

void callback(char* topic, byte* payload, int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    char message[length + 1];
    for (int i = 0; i < length; i++) {
        message[i] = (char)payload[i];
    }
    message[length] = '\0';
    Serial.println(message);

    if(strcmp(topic,DEVICE_LED_COMMAND_TOPIC)==0){
        if(strcmp(message, MQTT_ON_CMD)==0){
            stateOn = true;
            configColor(red, green, blue);
        } else {
            stateOn = false;
            configColor(red, green, blue);
        }
        // Send LED status
        sendState(DEVICE_LED_STATE_TOPIC, (stateOn) ? MQTT_ON_CMD : MQTT_OFF_CMD);
    } else if(strcmp(topic, DEVICE_LED_BRIGHTNESS_COMMAND_TOPIC)==0){
        brightness = atoi(message);
        configColor(red, green, blue);
        // Send LED Brighness value
        sprintf(message_buff, "%d", brightness);
        sendState(DEVICE_LED_BRIGHTNESS_STATE_TOPIC, message_buff);
    } else if(strcmp(topic, DEVICE_LED_RGB_COMMAND_TOPIC)==0){
        char* color = strtok(message, ",");
        red = atoi(color);
        color = strtok(0, ",");
        green = atoi(color);
        color = strtok(0, ",");
        blue = atoi(color);
        configColor(red, green, blue);
        // Send LED RGB values
        sprintf(message_buff, "%d,%d,%d", red, green, blue);
        sendState(DEVICE_LED_RGB_STATE_TOPIC, message_buff);
    } else if(strcmp(topic, DEVICE_DEVICE_COMMAND_TOPIC)==0){
        if(strcmp(message, MQTT_RESET_CMD)==0){
            Serial.println("Restarting device");
            ESP.restart();
        } else if(strcmp(message, MQTT_STAT_CMD)==0){
            Serial.println("Sending all sensor state");
            sendAllState();
        } else if(strcmp(message, MQTT_REGISTER_CMD)==0){
            Serial.println("Forcing registration of sensor");
            registerSensors(true);
            sendAllState();
        } else if(strcmp(message, MQTT_UNREGISTER_CMD)==0){
            Serial.println("Forcing unregistration of sensor");
            unregisterSensors();
        }
    }
}


void configColor(int inR, int inG, int inB) {
    realRed = map(red, 0, 255, 0, brightness);
    realGreen = map(green, 0, 255, 0, brightness);
    realBlue = map(blue, 0, 255, 0, brightness);
    if(stateOn){
        setColor(realRed, realGreen, realBlue);
    } else {
        setColor(0,0,0);
    }
}


void setColor(int inR, int inG, int inB) {
    analogWrite(redPin, inR);
    analogWrite(greenPin, inG);
    analogWrite(bluePin, inB);

    Serial.println("Setting LEDs:");
    Serial.print("r: ");
    Serial.print(inR);
    Serial.print(", g: ");
    Serial.print(inG);
    Serial.print(", b: ");
    Serial.println(inB);
}


void reconnect() {
    // Loop until we're reconnected
    int failedCnt = 0;
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(SENSORNAME, mqtt_user, mqtt_password)) {
            Serial.println("connected");
            //client.subscribe(light_set_topic);
            client.subscribe(DEVICE_LED_COMMAND_TOPIC);
            client.subscribe(DEVICE_LED_BRIGHTNESS_COMMAND_TOPIC);
            client.subscribe(DEVICE_LED_RGB_COMMAND_TOPIC);
            client.subscribe(DEVICE_DEVICE_COMMAND_TOPIC);
            setColor(0, 0, 0);
            registerSensors();
            sendAllState();
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            failedCnt++;
            if(failedCnt > 10){ // if failed to connect more than 10 times reset device to try and fix
            	Serial.println("Restarting device");
            	ESP.restart();
            }
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}


bool checkBoundSensor(float newValue, float prevValue, float maxDiff) {
    return newValue < prevValue - maxDiff || newValue > prevValue + maxDiff;
}


void loop() {
    ArduinoOTA.handle();

    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    //PIR CODE
    long now = millis();
    pirValue = digitalRead(PIRPIN); //read state of the
    if (pirOldValue != pirValue) {
    	// Wait for 2 full triggers before sending a detect to try and debounce.
    	// For some reason there are a lot of false triggers and the default trigger
    	// time is 2000ms for the RCWL modules so wait for 2 triggers to reduce false reports
        if (now - pirTimer > 4100) {
            if (pirValue == LOW) {
                motionStatus = false;
            } else {
                motionStatus = true;
            }
            sprintf(message_buff, "%s", (motionStatus) ? MQTT_ON_CMD : MQTT_OFF_CMD);
            sendState(DEVICE_PIR_STATE_TOPIC, message_buff);
            pirTimer = now;
            pirOldValue = pirValue;
        }
    } else {
        pirTimer = now;
    }

    float newTempValue = dht.readTemperature(true);
    float newHumValue = dht.readHumidity();

    if (checkBoundSensor(newTempValue, tempValue, diffTEMP)) {
        tempValue = newTempValue;
        dtostrf(tempValue, 4, 2, str_buff);
        sprintf(message_buff, "%s", str_buff);
        sendState(DEVICE_TEMP_STATE_TOPIC, message_buff);
    }

    if (checkBoundSensor(newHumValue, humValue, diffHUM)) {
        humValue = newHumValue;
        dtostrf(humValue, 4, 2, str_buff);
        sprintf(message_buff, "%s", str_buff);
        sendState(DEVICE_HUMIDITY_STATE_TOPIC, message_buff);
    }

    int newLDR = analogRead(LDRPIN);

    if (checkBoundSensor(newLDR, LDR, diffLDR)) {
        LDR = newLDR;
        sprintf(message_buff, "%d", LDR);
        sendState(DEVICE_LDR_STATE_TOPIC, message_buff);
    }
    delay(100);
}
