#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "constants.h"
#include "EasyDriver.h"

// Outputs
const int PIN_STEP = 2;
const int PIN_DIR = 4;
const int PIN_MS1 = 5;
const int PIN_MS2 = 12;
const int PIN_ENABLE = 16;

// Inputs
const int PIN_CUTOFF_CLOSE = 13;
const int PIN_CUTOFF_OPEN = 14;

EasyDriver stepper = EasyDriver(PIN_STEP, PIN_DIR, PIN_MS1, PIN_MS2, PIN_ENABLE);

WiFiClient espClient;
PubSubClient mqttClient(espClient);

u_int64_t currentStep = 0;
int stepDirection = EASYDRIVER_DIRECTION_FORWARDS;
bool stepperEnabled = true;

// Down = forwards == close
// Up   = Reverse == open
// Down = EASYDRIVER_MODE_FULL_STEP
// Up   = EASYDRIVER_MODE_QUARTER_STEP
const int MODE_CLOSE = EASYDRIVER_MODE_HALF_STEP;
const int MODE_OPEN = EASYDRIVER_MODE_EIGHTH_STEP;
const int DIRECTION_CLOSE = EASYDRIVER_DIRECTION_FORWARDS;
const int DIRECTION_OPEN = EASYDRIVER_DIRECTION_REVERSE;

const int STEPS_VERTICAL = 6500;
const int STEPS_TO_CLOSE = STEPS_VERTICAL * MODE_CLOSE;
const int STEPS_TO_OPEN = STEPS_VERTICAL * MODE_OPEN;

void setup() {
  pinMode(PIN_CUTOFF_CLOSE, INPUT_PULLUP);
  pinMode(PIN_CUTOFF_OPEN, INPUT_PULLUP);

  stepper.reset();
  stepper.setDelay(800);

  Serial.begin(115200);
  while (! Serial);

  setupWifi();
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(mqttCallback);
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttPublish(MQTT_TOPIC_STEPS, currentStep);

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
}


void closeBlinds() {
  setStepperDirection(DIRECTION_CLOSE);
  setStepperMode(MODE_CLOSE);
  stepFor(STEPS_TO_CLOSE);
}

void openBlinds() {
  setStepperDirection(DIRECTION_OPEN);
  setStepperMode(MODE_OPEN);
  stepFor(STEPS_TO_OPEN); 
}


/**
 * STEPPER WRAPPERS
 */
void stepFor(int steps) {
  setStepperEnabled(true);
  for (int i = 0; i < steps; i++) {
    if (stepDirection == DIRECTION_CLOSE && digitalRead(PIN_CUTOFF_CLOSE) == LOW) {
      Serial.println("ABORT CLOSE");
      break;
    }
    if (stepDirection == DIRECTION_OPEN && digitalRead(PIN_CUTOFF_OPEN) == LOW) {
      Serial.println("ABORT OPEN");
      break;
    }
    stepper.step();
    currentStep += stepDirection;
    mqttClient.loop();
  }
  setStepperEnabled(false);
}

void setStepperMode(int mode) {
  stepper.setMode(mode);
  mqttPublish(MQTT_TOPIC_MODE, mode);
}

void setStepperEnabled(bool enabled) {
  stepper.enable(enabled);
  
  if (enabled && !stepperEnabled) {
    mqttPublish(MQTT_TOPIC_ENABLED, 1); 
  } else if (!enabled && stepperEnabled) {
    mqttPublish(MQTT_TOPIC_ENABLED, 0); 
  }
  stepperEnabled = enabled;
}

void setStepperDirection(int direction) {
  stepper.setDirection(direction);
  stepDirection = direction;
  if (direction == EASYDRIVER_DIRECTION_FORWARDS) {
    mqttPublish(MQTT_TOPIC_DIRECTION, "forwards");
  } else {
    mqttPublish(MQTT_TOPIC_DIRECTION, "reverse");
  }
}


/**
 * WIFI / MQTT HELPERS
 */
void setupWifi() {
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

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
    if (stepDirection == DIRECTION_CLOSE) {
      steps = steps * MODE_CLOSE;
    } else {
      steps = steps * MODE_OPEN;
    }
    stepFor(steps);
  }

  if (topicStr == MQTT_TOPIC_CONTROL_BLINDS) {
    if (value.equals("open")) {
      openBlinds();
    } else if (value.equals("close")) {
      closeBlinds();
    }
  }

  
  if (value.equals("1")) {
    setStepperEnabled(true);
  } else if (value.equals("0")) {
    setStepperEnabled(false);
  } else if (value.equals("open")) {
    openBlinds();
  } else if (value.equals("close")) {
    closeBlinds();
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
