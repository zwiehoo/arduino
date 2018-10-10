#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid     = "zwierznet";
const char* password = "forfiter123";

#define CW false
#define CCW true

int step1 = 0;
int step2 = 0;
boolean dir = CW;
int count = 0;

int led = LOW;
boolean spin = false;

ESP8266WebServer server(80);

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  Serial.begin(115200);
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  digitalWrite(D4, LOW);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(D4, HIGH);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    if (led == LOW) {
      led = HIGH;
      dir = CCW;
    } else {
      led = LOW;
      dir = CW;
    }
    sendMainPage();
  });
  server.on("/on", []() {
    spin = true;
    sendMainPage();
  });
  server.on("/off", []() {
    spin = false;
    sendMainPage();
  });
  server.onNotFound([]() {
    sendMainPage();
  });
  server.begin();

  Serial.println("Setup complete");
  //  ESP.wdtDisable();
}

void sendMainPage() {
  server.send(200, "text/html", "<!doctype html><html lang=\"en\"><head><meta charset=\"utf-8\">"
  "<title>Trzykoder</title><meta name=\"description\" content=\"Digital Homing Device\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>"
  "<h1>Trzykoder:</h1>"
  "<p><a href=\"/on\">Engine start!</a></p>"
  "<p><a href=\"/off\">Engine stop!</a></p>"
  "<p><a href=\"/\">Change direction</a><p>"
  "</body></html>"
  );
}

int halfStep(int currentStep, boolean dir, int pin1, int pin2, int pin3, int pin4) {
  switch (currentStep) {
    case 0:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, HIGH);
      break;
    case 1:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, HIGH);
      break;
    case 2:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, LOW);
      break;
    case 3:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, LOW);
      break;
    case 4:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
    case 5:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
    case 6:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
    case 7:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, HIGH);
      break;
    default:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
  }

  if (dir) {
    currentStep++;
  } else {
    currentStep--;
  }
  if (currentStep > 7) {
    currentStep = 0;
  }
  if (currentStep < 0) {
    currentStep = 7;
  }
  return currentStep;
}

int fullStep(int currentStep, boolean dir, int pin1, int pin2, int pin3, int pin4) {
  switch (currentStep) {
    case 0:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, HIGH);
      break;
    case 1:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, HIGH);
      digitalWrite(pin4, LOW);
      break;
    case 2:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, HIGH);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
    case 3:
      digitalWrite(pin1, HIGH);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
    default:
      digitalWrite(pin1, LOW);
      digitalWrite(pin2, LOW);
      digitalWrite(pin3, LOW);
      digitalWrite(pin4, LOW);
      break;
  }

  if (dir) {
    currentStep++;
  } else {
    currentStep--;
  }
  if (currentStep > 3) {
    currentStep = 0;
  }
  if (currentStep < 0) {
    currentStep = 3;
  }
  return currentStep;
}

void motorOff(int pin1, int pin2, int pin3, int pin4) {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
}

void loop() {
  digitalWrite(D4, HIGH);
  if (spin) {
    step1 = fullStep(step1, dir, D3, D4, D7, D8);
    step2 = fullStep(step2, !dir, D1, D2, D5, D6);
  } else {
    motorOff(D1, D2, D5, D6);
    motorOff(D3, D4, D7, D8);
    step1 = 0;
    step2 = 0;
  }
  delay(3);
  server.handleClient();
}
