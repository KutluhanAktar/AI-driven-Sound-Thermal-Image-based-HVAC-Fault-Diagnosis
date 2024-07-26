         /////////////////////////////////////////////  
        //  AI-driven Sound & Thermal Image-based  //
       //         HVAC Fault Diagnosis            //
      //           ----------------              //
     //             (Arduino Nano)              //           
    //             by Kutluhan Aktar           // 
   //                                         //
  /////////////////////////////////////////////

//
// Identify the faulty components via anomalous sound detection and diagnose ensuing cooling malfunctions via thermal visual anomaly detection.
//
// For more information:
// https://www.hackster.io/kutluhan-aktar
//
//
// Connections
// Arduino Nano :
//                                28BYJ-48 Stepper Motor (w/ ULN2003)
// D8      ------------------------ IN1
// D9      ------------------------ IN2
// D10     ------------------------ IN3
// D11     ------------------------ IN4
//                                Micro Switch with Pulley (JL024-2-026)
// D12     ------------------------ +
//                                Home Button
// D7     ------------------------ +
//                                5mm Common Anode RGB LED
// D3      ------------------------ R
// D5      ------------------------ G
// D6      ------------------------ B
//                                XIAO ESP32C6
// RX (D2) ------------------------ TX (D6) 
// TX (D4) ------------------------ RX (D7)


// Include the required libraries:
#include <SoftwareSerial.h>
#include <Stepper.h>

// Define the CNC (28BYJ-48) stepper motor configurations and initialize the stepper object. 
int stepsPerRevolution = 2038, max_motor_RPM = 10, step_delay = 500, CNC_go_to_switch = 15, CNC_switch_to_home = 1;
Stepper CNC_motor(stepsPerRevolution, 8, 10, 9, 11); // IN1-IN3-IN2-IN4

// Define the software serial port — XIAO.
const int rxPin = 2, txPin = 4;
SoftwareSerial XIAO(/*RX=*/rxPin, /*TX=*/txPin);

// Define the CNC stop micro switch pin.
#define CNC_stop_switch 12

// Define the CNC home button pin.
#define CNC_home_button 7

// Define RGB LED pins.
#define red_pin    3
#define green_pin  5
#define blue_pin   6

// Define the required CNC configurations by creating a struct — _CNC.
struct _CNC{
  String pos_command[5] = {"1", "2", "3", "4", "h"};
  int step_number[4] = {1, 1, 3, 1};
  int pos_delay = 5000;
};

// Define the data holders:
struct _CNC _CNC;
String data_packet = "";

void setup(){
  Serial.begin(115200);

  // Initiate the serial communication between Arduino Nano and XIAO ESP32C6.
  XIAO.begin(115200);

  // Register pin configurations.
  pinMode(CNC_stop_switch, INPUT_PULLUP);
  pinMode(CNC_home_button, INPUT_PULLUP);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  adjustColor(0,0,0);

}

void loop(){
  // Obtain the data packet transferred by XIAO ESP32C6 via serial communication.
  if(XIAO.available() > 0){
    data_packet = XIAO.readString();
  }

  if(data_packet != ""){
    Serial.print("Received Data Packet => "); Serial.println(data_packet);
    // Depending on the received coordinate update command from XIAO ESP32C6, change the thermal camera container head position via the stepper motor.
    if(data_packet.indexOf(_CNC.pos_command[0]) > -1){
      adjustColor(255,0,0);
      CNC_motor_move(_CNC.step_number[0], "CW");
      adjustColor(255,255,0);
      delay(_CNC.pos_delay);
      // Transfer (reply) the coordinate update confirmation message to XIAO ESP32C6 via serial communication.
      XIAO.print("CNC_OK");
      delay(1000);
      adjustColor(0,255,0);
    }else if(data_packet.indexOf(_CNC.pos_command[1]) > -1){
      adjustColor(255,0,0);
      CNC_motor_move(_CNC.step_number[1], "CW");
      adjustColor(255,255,0);
      delay(_CNC.pos_delay);
      // Transfer (reply) the coordinate update confirmation message to XIAO ESP32C6 via serial communication.
      XIAO.print("CNC_OK");
      delay(1000);
      adjustColor(0,255,0);
    }else if(data_packet.indexOf(_CNC.pos_command[2]) > -1){
      adjustColor(255,0,0);
      CNC_motor_move(_CNC.step_number[2], "CW");
      adjustColor(255,255,0);
      delay(_CNC.pos_delay);
      // Transfer (reply) the coordinate update confirmation message to XIAO ESP32C6 via serial communication.
      XIAO.print("CNC_OK");
      delay(1000);
      adjustColor(0,255,0);
    }else if(data_packet.indexOf(_CNC.pos_command[3]) > -1){
      adjustColor(255,0,0);
      CNC_motor_move(_CNC.step_number[3], "CW");
      adjustColor(255,255,0);
      delay(_CNC.pos_delay);
      // Transfer (reply) the coordinate update confirmation message to XIAO ESP32C6 via serial communication.
      XIAO.print("CNC_OK");
      delay(1000);
      adjustColor(0,255,0);
    }else if(data_packet.indexOf(_CNC.pos_command[4]) > -1){
      // If requested, after going through four coordinate updates, return the thermal camera container head to the starting point (zeroing).
      int zeroing = 0;
      for(int i=0; i<4; i++) zeroing+=_CNC.step_number[i];
      Serial.print("Zeroing the container head for "); Serial.print(zeroing); Serial.println(" steps!\n");
      adjustColor(255,0,0);
      CNC_motor_move(zeroing, "CCW");
      adjustColor(255,255,0);
      delay(_CNC.pos_delay);
      // Transfer (reply) the coordinate update confirmation message to XIAO ESP32C6 via serial communication.
      XIAO.print("CNC_OK");
      delay(1000);
      adjustColor(255,0,255);
    }
    // Clear the received data packet.
    data_packet = "";
  }

  // If the home button is pressed, initiate the container head homing sequence.
  if(!digitalRead(CNC_home_button)){
    Serial.println("\nHoming sequence activated!\n");
    adjustColor(0,0,255);
    CNC_motor_move(CNC_go_to_switch, "CCW");
  }
}

int CNC_motor_move(int step_number, String _direction){
  int revs = 0;
  // Move the CNC stepper motor according to the passed step number and the direction.
  // CW:  Clockwise
  // CCW: Counter-clockwise
  if(_direction == "CW"){
    for(int i=0; i<step_number; i++){
      CNC_motor.setSpeed(max_motor_RPM/2);
      CNC_motor.step(stepsPerRevolution/8);
      delay(step_delay);
      revs++;
    }
  }else if(_direction == "CCW"){
    for(int i=0; i<step_number; i++){
      CNC_motor.setSpeed(max_motor_RPM/2);
      CNC_motor.step(-stepsPerRevolution/4);
      delay(step_delay);
      revs++;
      // If the thermal camera container head triggers the stop micro switch by colliding, force the container head to return to the home position.
      if(digitalRead(CNC_stop_switch)){
        if(CNC_position_home()) adjustColor(255,255,255);
        break;      
      }      
    }
  }
  // Return the total revolution number.
  return revs;
}

bool CNC_position_home(){
  // Return the thermal camera container head to the home position — 0.
  for(int i=0; i<CNC_switch_to_home; i++){
    CNC_motor.setSpeed(max_motor_RPM);
    CNC_motor.step(stepsPerRevolution/8);
    delay(step_delay);
  }
  return true;
}

void adjustColor(int r, int g, int b){
  analogWrite(red_pin, (255-r));
  analogWrite(green_pin, (255-g));
  analogWrite(blue_pin, (255-b));
}
