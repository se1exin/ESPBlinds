#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include "constants.h"
#include "EasyDriver.h"

// Outputs
const int PIN_STEP = 12;
const int PIN_DIR = 13;
const int PIN_MS1 = 4;
const int PIN_MS2 = 5;
const int PIN_ENABLE = 14;

EasyDriver stepper = EasyDriver(PIN_STEP, PIN_DIR, PIN_MS1, PIN_MS2, PIN_ENABLE);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Down = forwards == close
// Up   = Reverse == open
const int DIRECTION_CLOSE = EASYDRIVER_DIRECTION_FORWARDS;
const int DIRECTION_OPEN = EASYDRIVER_DIRECTION_REVERSE;

const bool MQTT_SEND_STEPS = false;

int STEPS_VERTICAL = 3000; // Number of full steps required to complete open/close sequence

int DELAY_CLOSE = 600; // In micros
int MODE_CLOSE = EASYDRIVER_MODE_QUARTER_STEP;

int DELAY_OPEN = 600; // In micros
int MODE_OPEN = EASYDRIVER_MODE_QUARTER_STEP;

// Locals
int currentStep = 0;
int currentMode = MODE_OPEN;
int stepDirection = EASYDRIVER_DIRECTION_FORWARDS;
bool stepperEnabled = true;
bool isOpening = false;
bool isClosing = false;

void setup() {
  stepper.reset();

  Serial.begin(115200);
  while (! Serial);

  setupWifi();
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(mqttCallback);
  if (!mqttClient.connected()) {
    mqttReconnect();
  }

  mqttPublish(MQTT_TOPIC_ENABLED, 0);
  mqttPublish(MQTT_TOPIC_STATE, "closed");
  mqttPublish(MQTT_TOPIC_STEPS, currentStep);

  // Topic for controlling the stepper remotely
  mqttClient.subscribe(MQTT_TOPIC_CONTROL_ENABLED);
  mqttClient.subscribe(MQTT_TOPIC_CONTROL_DIRECTION);
  mqttClient.subscribe(MQTT_TOPIC_CONTROL_STEPFOR);
  mqttClient.subscribe(MQTT_TOPIC_CONTROL_BLINDS);
}


/**
 * MAIN LOOP
 */
void loop() {
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();
  ArduinoOTA.handle();
}


void closeBlinds() {
  isClosing = true;
  mqttPublish(MQTT_TOPIC_STATE, "closing");
  setStepperDirection(DIRECTION_CLOSE);
  setStepperMode(MODE_CLOSE);
  stepFor(STEPS_VERTICAL);
  mqttPublish(MQTT_TOPIC_STATE, "closed");
  isClosing = false;
}

void openBlinds() {
  isOpening = true;
  mqttPublish(MQTT_TOPIC_STATE, "opening");
  setStepperDirection(DIRECTION_OPEN);
  setStepperMode(MODE_OPEN);
  stepFor(STEPS_VERTICAL); 
  mqttPublish(MQTT_TOPIC_STATE, "opened");
  isOpening = false;
}


/**
 * STEPPER WRAPPERS
 */
int lastYield = 0;
void stepFor(int steps) {
  setStepperEnabled(true);
  for (int i = 0; i < steps; i++) {
    // Stepping mode is effectively a multiplier on the desired number of steps
    // E.g. a quarter step mode, to achieve a full step we need to do 4x stepper steps
    for (int x = 0; x < currentMode; x++) {
      stepper.step();

      if (millis() - lastYield > 80) {
        yield(); // Don't block the CPU or ESP8266 will crash
        lastYield = millis();
      }
    }

    currentStep += stepDirection;
    if (!stepperEnabled) {
      break;
    }
    
    if (MQTT_SEND_STEPS) {
      if (currentStep % 250 == 0) {
        mqttPublish(MQTT_TOPIC_STEPS, currentStep);
      }
    }
    mqttClient.loop();
  }
  setStepperEnabled(false);
}

void setStepperMode(int mode) {
  stepper.setMode(mode);
  currentMode = mode;
  mqttPublish(MQTT_TOPIC_MODE, mode);
}

void setStepperEnabled(bool enabled) {
  stepper.enable(enabled);
  
  if (enabled) {
    mqttPublish(MQTT_TOPIC_ENABLED, 1); 
  } else {
    mqttPublish(MQTT_TOPIC_ENABLED, 0); 
  }
  stepperEnabled = enabled;
}

void setStepperDirection(int direction) {
  stepper.setDirection(direction);
  stepDirection = direction;
  if (direction == EASYDRIVER_DIRECTION_FORWARDS) {
    stepper.setDelay(DELAY_CLOSE);
    mqttPublish(MQTT_TOPIC_DIRECTION, "forwards");
  } else {
    stepper.setDelay(DELAY_OPEN);
    mqttPublish(MQTT_TOPIC_DIRECTION, "reverse");
  }
}


/**
 * WIFI / MQTT HELPERS
 */
void setupWifi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  ArduinoOTA.setHostname(MQTT_CLIENT_ID);
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    ESP.reset();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

  ArduinoOTA.begin();
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String topicStr = topic;
  String value = "";
  for (int i = 0; i < length; i++) {
      value += (char)payload[i];
  }

  Serial.println("mqttCallback !");
  Serial.println(topicStr);
  Serial.println(value);

  if (topicStr == MQTT_TOPIC_CONTROL_ENABLED) {
    if (value.equals("1")) {
      setStepperEnabled(true);
    } else if (value.equals("0")) {
      setStepperEnabled(false);
    }
  }

  if (topicStr == MQTT_TOPIC_CONTROL_DIRECTION) {
    if (value.equals("1")) {
      setStepperDirection(DIRECTION_CLOSE);
    } else if (value.equals("0")) {
      setStepperDirection(DIRECTION_OPEN);
    }
  }

  if (topicStr == MQTT_TOPIC_CONTROL_STEPFOR) {
    int steps = value.toInt();
    stepFor(steps);
  }

  if (topicStr == MQTT_TOPIC_CONTROL_BLINDS) {
    if (value.equals("opened") && !isOpening) {
    // if (value.equals("opened")) {
      openBlinds();
    } else if (value.equals("closed") && !isClosing) {
    // } else if (value.equals("closed")) {
      closeBlinds();
    } else if (value.equals("stop")) {
      setStepperEnabled(false);
    }
  }
}

void mqttReconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD, MQTT_TOPIC_STATUS, 1, true, "disconnected", false)) {
      Serial.println("connected");

      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_TOPIC_STATUS, "connected", true);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPublish(char *topic, int payload) {
  // Serial.print(topic);
  // Serial.print(": ");
  // Serial.println(payload);

  mqttClient.publish(topic, String(payload).c_str(), true);
}

void mqttPublish(char *topic, String payload) {
  // Serial.print(topic);
  // Serial.print(": ");
  // Serial.println(payload);

  mqttClient.publish(topic, payload.c_str(), true);
}
