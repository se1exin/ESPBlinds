// Wifi username and password
const char *WIFI_SSID = "your ssid";
const char *WIFI_PASSWORD = "your password";

// MQTT connection details
const char *MQTT_SERVER = "x.x.x.x";
const char *MQTT_USER;
const char *MQTT_PASSWORD;

#define MQTT_TOPIC_CONTROL_ENABLED "home/esp8266_blinds_01/control/enabled"
#define MQTT_TOPIC_CONTROL_DIRECTION "home/esp8266_blinds_01/control/direction"
#define MQTT_TOPIC_CONTROL_STEPFOR "home/esp8266_blinds_01/control/stepfor"
#define MQTT_TOPIC_CONTROL_BLINDS "home/esp8266_blinds_01/control/blinds"

#define MQTT_TOPIC_STATE "home/esp8266_blinds_01/state"
#define MQTT_TOPIC_STEPS "home/esp8266_blinds_01/steps"
#define MQTT_TOPIC_ENABLED "home/esp8266_blinds_01/enabled"
#define MQTT_TOPIC_DIRECTION "home/esp8266_blinds_01/direction"
#define MQTT_TOPIC_MODE "home/esp8266_blinds_01/mode"
#define MQTT_TOPIC_STATUS "home/esp8266_blinds_01/status"

// These values will come and save to from EEPROM
#define MQTT_TOPIC_MODE_OPEN "home/esp8266_blinds_01/mode/open"
#define MQTT_TOPIC_MODE_CLOSE "home/esp8266_blinds_01/mode/close"
#define MQTT_TOPIC_DELAY_OPEN "home/esp8266_blinds_01/delay/open"
#define MQTT_TOPIC_DELAY_CLOSE "home/esp8266_blinds_01/delay/close"
#define MQTT_TOPIC_STEPS_VERTICAL "home/esp8266_blinds_01/steps/vertical"
#define MQTT_TOPIC_CONTROL_MODE_OPEN "home/esp8266_blinds_01/control/mode/open"
#define MQTT_TOPIC_CONTROL_MODE_CLOSE "home/esp8266_blinds_01/control/mode/close"
#define MQTT_TOPIC_CONTROL_DELAY_OPEN "home/esp8266_blinds_01/control/delay/open"
#define MQTT_TOPIC_CONTROL_DELAY_CLOSE "home/esp8266_blinds_01/control/delay/close"
#define MQTT_TOPIC_CONTROL_STEPS_VERTICAL "home/esp8266_blinds_01/control/steps/vertical"

#define MQTT_CLIENT_ID "esp8266_blinds_01"
