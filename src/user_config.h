#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H
/************ WIFI and MQTT INFORMATION (CHANGE THESE FOR YOUR SETUP) ******************/
#define wifi_ssid            "WIFI_NAME"
#define wifi_password        "WIFI_PASSWORD"
#define mqtt_server          "MQTT_SERVER_ADDRESS"
#define mqtt_user            "MQTT_USERNAME"
#define mqtt_password        "MQTT_PASSWORD"
#define mqtt_port            1883


#define DEVICE_NAME          "MyDeviceName"
#define DEVICE_FRIENDLY_NAME "My Friendly Device Name"

// Sensor name is used for OTA and WiFi hostnames, can be different from DEVICE_NAME if you want
// to have a difference between your MQTT topic names and the OTA/WiFi hostnames
#define SENSORNAME DEVICE_NAME

#define OTApassword "OTA_PASSWORD"
int OTAport = 8266;


#endif