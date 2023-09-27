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

#define MQTT_CLIENT_ID "esp8266_blinds_01"
