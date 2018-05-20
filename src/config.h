#ifndef _CONFIG_H
#define _CONFIG_H
// Should not need to set anything below this, unless you have changed the MQTT path for HA discovery


//
// MQTT TOPICS (to maintain HomeAssitant discovery ability do not edit unless you know what you are doing)
//

// Topics for general device management, this if for actions like forcing re/un-registration w/ discovery, rebooting, etc
#define DEVICE_DEVICE_COMMAND_TOPIC "homeassistant/" DEVICE_NAME "/set"

// Topics for PIR (motion sensor)
#define DEVICE_PIR_DISCOVERY_TOPIC "homeassistant/binary_sensor/" DEVICE_NAME "_sensor_pir/config"
#define DEVICE_PIR_STATE_TOPIC     "homeassistant/binary_sensor/" DEVICE_NAME "_sensor_pir/state"
#define DEVICE_PIR_DISCOVERY_REGISTER_MESSAGE "{\"name\":\"" DEVICE_FRIENDLY_NAME " Motion Sensor\",\"device_class\":\"motion\"}"

// Topics for temperature sensor
#define DEVICE_TEMP_DISCOVERY_TOPIC "homeassistant/sensor/" DEVICE_NAME "_sensor_temp/config"
#define DEVICE_TEMP_STATE_TOPIC     "homeassistant/sensor/" DEVICE_NAME "_sensor_temp/state"
#define DEVICE_TEMP_DISCOVERY_REGISTER_MESSAGE "{\"name\":\"" DEVICE_FRIENDLY_NAME " Sensor Temp\",\"unit_of_measurement\":\"°F\"}"


// Topics for humidity sensor
#define DEVICE_HUMIDITY_DISCOVERY_TOPIC "homeassistant/sensor/" DEVICE_NAME "_sensor_humidity/config"
#define DEVICE_HUMIDITY_STATE_TOPIC     "homeassistant/sensor/" DEVICE_NAME "_sensor_humidity/state"
#define DEVICE_HUMIDITY_DISCOVERY_REGISTER_MESSAGE "{\"name\":\"" DEVICE_FRIENDLY_NAME " Sensor Humidity\",\"unit_of_measurement\":\"%\"}"

// Topics for LDR (light sensor)
#define DEVICE_LDR_DISCOVERY_TOPIC "homeassistant/sensor/" DEVICE_NAME "_sensor_ldr/config"
#define DEVICE_LDR_STATE_TOPIC     "homeassistant/sensor/" DEVICE_NAME "_sensor_ldr/state"
#define DEVICE_LDR_DISCOVERY_REGISTER_MESSAGE "{\"name\":\"" DEVICE_FRIENDLY_NAME " Sensor Light Level\",\"unit_of_measurement\":\"lux\"}"


// Topics for LED. there are 3 sets because they cover on/off, brightness and RGB color seperately
#define DEVICE_LED_DISCOVERY_TOPIC          "homeassistant/light/" DEVICE_NAME "_sensor_led/config"
#define DEVICE_LED_COMMAND_TOPIC            "homeassistant/light/" DEVICE_NAME "_sensor_led/set"
#define DEVICE_LED_STATE_TOPIC              "homeassistant/light/" DEVICE_NAME "_sensor_led/state"

#define DEVICE_LED_BRIGHTNESS_COMMAND_TOPIC "homeassistant/light/" DEVICE_NAME "_sensor_led/brighness/set"
#define DEVICE_LED_BRIGHTNESS_STATE_TOPIC   "homeassistant/light/" DEVICE_NAME "_sensor_led/brighness/state"

#define DEVICE_LED_RGB_COMMAND_TOPIC        "homeassistant/light/" DEVICE_NAME "_sensor_led/rgb/set"
#define DEVICE_LED_RGB_STATE_TOPIC          "homeassistant/light/" DEVICE_NAME "_sensor_led/rgb/state"

#define DEVICE_LED_DISCOVERY_REGISTER_MESSAGE "{\"name\":\"" DEVICE_FRIENDLY_NAME " LED\",\"brightness\":true,\"flash\":true,\"rgb\":true,\"optomistic\":false,\"qos\":0,"\
"\"command_topic\":\"homeassistant/light/" DEVICE_NAME "_sensor_led/set\","\
"\"brightness_command_topic\":\"homeassistant/light/" DEVICE_NAME "_sensor_led/brighness/set\","\
"\"brightness_state_topic\":\"homeassistant/light/" DEVICE_NAME "_sensor_led/brighness/state\","\
"\"rgb_command_topic\":\"homeassistant/light/" DEVICE_NAME "_sensor_led/rgb/set\","\
"\"rgb_state_topic\":\"homeassistant/light/" DEVICE_NAME "_sensor_led/rgb/state\"}"

// Message text for LED state
#define MQTT_ON_CMD     "ON"             // command that sets relay on
#define MQTT_OFF_CMD    "OFF"            // command that sets relay off

// Message text for device commands
#define MQTT_RESET_CMD      "reset"      // command that resets the device
#define MQTT_STAT_CMD       "stat"       // command to resend all state
#define MQTT_REGISTER_CMD   "register"   // command to force reregistration
#define MQTT_UNREGISTER_CMD "unregister" // command to force unregistration

const int redPin   = D1;
const int greenPin = D2;
const int bluePin  = D3;
#define PIRPIN  D5
#define DHTPIN  D7
#define DHTTYPE DHT22
#define LDRPIN  A0

// Variables for LDR(light sensor)
float ldrValue;
int   LDR;
float calcLDR;
float diffLDR = 25;

// Variables for temp sensor
float diffTEMP = 0.2;
float tempValue;

// Variables for humidity sensor
float diffHUM = 1;
float humValue;


// Variables for PIR(motion sensor)
int  pirValue;
int  pirOldValue;
long pirTimer;
bool motionStatus = false;

// Buffers for MQTT messages
char message_buff[100];
char str_buff[100];

int calibrationTime = 0;

const int BUFFER_SIZE = 300;

// Variables for LED
byte red        = 0;
byte green      = 0;
byte blue       = 0;
byte brightness = 0;
// Variables that hold normalized variables that include color and brightness
byte realRed    = 0;
byte realGreen  = 0;
byte realBlue   = 0;

bool stateOn    = false;

#endif