#include <Wire.h>
#include <SoftwareSerial.h>

// Misc Program Variables
int currentX = 0;
int currentY = 0;
String currentCardinalDirection = "E";  // Starting facing "East", aka toward the X direction

bool intersectionDetected = false;
String lastKnownDirection = "Forward";

bool isTurning = false;
String isTurningDirection = "";
unsigned long turningStartTime = 0;  // Variable to hold the time when turning started

// Add a new variable to hold the time when the robot first loses sight of the line
unsigned long lineLostStartTime = 0;


// IR Sensor Connections
#define IR1 A3
#define IR2 A2
#define IR3 A1
#define IR4 A0

// Motor A connections
#define MOTOR_A1 3
#define MOTOR_A2 5
// Motor B connections
#define MOTOR_B1 10
#define MOTOR_B2 11

// Motor Speed Variables
int motorSpeed = 255;           // Overall Motor speed (0-255)
int motorSpeedAMultiplier = 1;  // Adjust this multiplier for Motor A speed to correct for differences.
int motorSpeedBMultiplier = 1;  // Adjust this multiplier for Motor B speed to correct for differences.

// Software Serial Setup
SoftwareSerial mySerial(12, 13);

// Variables for asynchronous NodeMCU ESP messages.
unsigned long previousMillis = 0;
const long interval = 1000;  // 1000 milliseconds (1 second) interval for sending message

void setup() {

  // Setup Serial communication
  Serial.begin(115200);
  mySerial.begin(9600);

  // Setup IR sensor Pins
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);

  // Define motor control pins as outputs
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);

  // Set the motors to initial stopped state
  digitalWrite(MOTOR_A1, LOW);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, LOW);
  digitalWrite(MOTOR_B2, LOW);
}

void loop() {

  /****************
  * IR PIN SYNTHESIS
  ****************/

  // Read IR pins
  int IR1Output = digitalRead(IR1);
  int IR2Output = digitalRead(IR2);
  int IR3Output = digitalRead(IR3);
  int IR4Output = digitalRead(IR4);

  // Serial Print IR outputs for debugging
  Serial.print(IR1Output);
  Serial.print(IR2Output);
  Serial.print(IR3Output);
  Serial.print(IR4Output);

  /****************
 * Motor Control Logic
 ****************/
  if (isTurning == true) {
    if (turningStartTime == 0) {    // If turning just started
      turningStartTime = millis();  // Record the start time of turning
    }

    Serial.println("ISTURNING TRUE");
    controlMotor(-90, 90);  // Turn left

    unsigned long elapsedTurningTime = millis() - turningStartTime;

    if (elapsedTurningTime >= 150) {
      int IR2Output = digitalRead(IR2);
      int IR3Output = digitalRead(IR3);
      if (IR2Output == HIGH || IR3Output == HIGH) {
        controlMotor(0, 0);  // stop;
        delay(500);
        isTurning = false;
        turningStartTime = 0;  // Reset the turning start time
      }
    }

    if (elapsedTurningTime >= 2000) {
      controlMotor(0, 0);  // stop;
      delay(1000);
      isTurning = false;
      turningStartTime = 0;  // Reset the turning start time
    }

  } else
    // Handle Intersection Crossings
    if (IR1Output == HIGH && IR2Output == HIGH && IR3Output == HIGH && IR4Output == HIGH) {  // All sensors on a black line
      controlMotor(200, 200);                                                                // Both motors forward at full speed
      Serial.println(" Crossing Internal Intersection");
    } else if (IR1Output == HIGH && IR2Output == HIGH && IR3Output == HIGH && IR4Output == LOW) {  // All sensors on a black line except for rightmost
      controlMotor(200, 200);                                                                      // Both motors forward at full speed
      Serial.println(" Crossing Internal Intersection On Edge (Right)");
    } else if (IR1Output == LOW && IR2Output == HIGH && IR3Output == HIGH && IR4Output == HIGH) {  // All sensors on a black line except for leftmost
      controlMotor(200, 200);                                                                      // Both motors forward at full speed
      Serial.println(" Crossing Internal Intersection On Edge (Left)");

      // Keep the robot on the line
    } else if (IR2Output == HIGH && IR3Output == HIGH) {  // Both center sensors on a black line
      controlMotor(200, 200);                             // Both motors forward at full speed
      Serial.println("Going Straight");
      lastKnownDirection = "Forward";
    } else if (IR2Output == LOW && IR3Output == HIGH) {  // Left sensor only on a black line
      controlMotor(100, 200);
      Serial.println("Turn Right");
      lastKnownDirection = "Right";
    } else if (IR2Output == HIGH && IR3Output == LOW) {  // Right sensor only on a black line
      controlMotor(200, 100);                            // Motor 1 backward, Motor 2 forward (turn left)
      Serial.println("Turn Left");
      lastKnownDirection = "Left";

      // Other cases
    }

    // else if (IR2Output == LOW && IR3Output == LOW) {  // Both center sensors are high - we lost the line. Turn the robot in the last known direction of the line
    //   controlMotor(255, 255);                           // Both motors forward at full speed

    //   // Serial.print("Finding the line - turning ");
    //   // Serial.println(lastKnownDirection);

    //   // if (lastKnownDirection == "Left") {  // Turn toward the line hopefully
    //   //   motorLeft();
    //   // } else if (lastKnownDirection == "Right") {  // Turn toward the line hopefully
    //   //   motorRight();
    //   // } else if (lastKnownDirection == "Forward") {  // Keep on keeping on
    //   //   motorForward();
    //   // }

    //}
    else {
      controlMotor(0, 0);  // Stop both motors
      Serial.println("No Lines Detected");

      // Record the time when the line is first lost
      if (lineLostStartTime == 0) {
        lineLostStartTime = millis();
      }

      // Check if the line has been lost for 300ms or more
      if (millis() - lineLostStartTime >= 300) {
        isTurning = true;
        lineLostStartTime = 0;  // Reset the line lost start time
      }
    }

  // After processing IR sensor output, reset lineLostStartTime if a line is detected
  if (IR2Output == HIGH || IR3Output == HIGH) {
    lineLostStartTime = 0;
  }
  // Are we on an intersection currently? Trip the intersectionDetected boolean to avoid double counting intersections.

  if (IR1Output == HIGH && IR2Output == HIGH && IR3Output == HIGH && IR4Output == HIGH) {  // All sensors on a black line
    intersectionDetected = true;
  } else if (IR1Output == HIGH && IR2Output == HIGH && IR3Output == HIGH && IR4Output == LOW) {  // All sensors on a black line except for rightmost
    intersectionDetected = true;
  } else if (IR1Output == LOW && IR2Output == HIGH && IR3Output == HIGH && IR4Output == HIGH) {  // All sensors on a black line except for rightmost
    intersectionDetected = true;
  } else {
    intersectionDetected = false;
  }

  /****************
 * Motor Testing
 ****************/

  // motorForward();
  // delay(1000);
  // motorBackward();
  // delay(1000);
  // motorLeft();
  // delay(1000);
  // motorRight();
  // delay(1000);
  // motorStop();
  // delay(1000);

  /****************
 * Communicate With ESP8266 Testing
 ****************/

  // unsigned long currentMillis = millis();

  // if (currentMillis - previousMillis >= interval) {
  //   previousMillis = currentMillis;

  //   //Report Location (Command, X, Y, Direction)
  //   mySerial.println("POS,2,3,N");

  //   // Report Puck Location & Color (Command, X, Y, Color)
  //   // Color is Red, Green or Blue
  //   // mySerial.print("COL,4,3,Red");
  // }
}
