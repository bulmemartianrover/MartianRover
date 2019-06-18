#include <Arduino.h>
#include "BluetoothSerial.h"
#include "Servo.h"
#include "Stepper.h"
#include <map>

BluetoothSerial SerialBT;

#define COUNT_SERVOS 5
static const int servoPins[COUNT_SERVOS] = {2, 23, 22, 21, 19};
Servo servos[COUNT_SERVOS];

Stepper camStepper(64, 34, 35, 32, 33);

typedef String (*commandAction)(String);

std::map<String, commandAction> commandMap;

String cmd_ping(String args) {
  return "pong";
}

String cmd_forward(String args) {

  return "ok";
}

String cmd_backward(String args) {
  return "Not implemented!";
}

String cmd_left_wheel(String args) {
  return "Not implemented!";
}

String cmd_right_wheel(String args) {
  return "Not implemented!";
}

String cmd_motor_stop(String args) {
  return "Not implemented!";
}

String cmd_servo(String args) {
  int servoId = args.substring(0, 1).toInt();
  Serial.println(servoId);
  int deg = args.substring(2).toInt();
  if (servoId >= COUNT_SERVOS) return "err";
  servos[servoId].write(deg);
  return "ok";
}

String cmd_camStepper(String args) {
  bool back = args[0] == '1';
  int steps = args.substring(1).toInt();
  camStepper.step(steps);
  return "ok";
}

void setupCommandMap() {
  commandMap.insert(std::pair<String, commandAction>("ping", cmd_ping));
  commandMap.insert(std::pair<String, commandAction>("forward", cmd_forward));
  commandMap.insert(std::pair<String, commandAction>("backward", cmd_backward));
  commandMap.insert(std::pair<String, commandAction>("left_wheel", cmd_left_wheel));
  commandMap.insert(std::pair<String, commandAction>("right_wheel", cmd_right_wheel));
  commandMap.insert(std::pair<String, commandAction>("motor_stop", cmd_motor_stop));
  commandMap.insert(std::pair<String, commandAction>("servo", cmd_servo));
}

void receiveBTCommand() {
  if (!SerialBT.available()) return;
  String s = SerialBT.readStringUntil('+');
  String args = SerialBT.readStringUntil('\n');
  Serial.print("- ");
  Serial.print(s);
  Serial.print(" -> ");
  Serial.print(args);
  Serial.println(" -");
  auto result = commandMap.find(s);
  if (result == commandMap.end()) {
    SerialBT.println("notfound");
    Serial.println("Command not found!");
    return;
  }
  commandAction a = result->second;
  SerialBT.println((*a)(args));
  Serial.println("Executed " + s);
}

void setup() {
  setupCommandMap();
  // Initialise Servo library
  for(int i = 0; i < COUNT_SERVOS; ++i) {
    if(!servos[i].attach(servoPins[i])) {
        Serial.print("Servo ");
        Serial.print(i);
        Serial.println("attach error");
    }
  }
  Serial.begin(115200);
  SerialBT.begin("MartianRover");
  //SerialBT.begin("ESP32test");
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  receiveBTCommand();
  delay(20);
}