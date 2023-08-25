#define CW 5   
#define CCW 6  
#define RIGHT_SENSOR_PIN 11   // Right photomicrosensor pin
#define LEFT_SENSOR_PIN 10   // Left photomicrosensor pin

int stepDelay = 10000;
bool whenReset = true;
bool motorStopped = false;

void rotateCW(int angleDegrees) {
  int steps = angleDegrees / 0.72;
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(CW, HIGH);
    delayMicroseconds(stepDelay); 
    digitalWrite(CW, LOW);
    delayMicroseconds(stepDelay);
  }
}

void rotateCCW(int angleDegrees) {
  int steps = angleDegrees / 0.72;
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(CCW, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(CCW, LOW);
    delayMicroseconds(stepDelay);
  }
}

void stopMotor() {
  digitalWrite(CW, LOW);
  digitalWrite(CCW, LOW);
}

void frame180() {
  while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
    // Rotating CCW until right sensor is interrupted
    rotateCCW(5); // Jump start it with a small angle
  }
  delay(1000); // delay for stability

  while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
    // Rotating CW until left sensor is interrupted
    rotateCW(5); // Rotating by a small angle
  }
  delay(1000); // Waiting for stability
}

void reset() {
  if (whenReset) {
    rotateCCW(5); // Rotating CCW by a small angle
  }
  if (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
    // Rotating 8 degrees CW
    whenReset = false;
    delay(2000);
    rotateCW(8);
    delay(2000); // Delay for stability
    stopMotor();
  }
}

void robotMovingStart() {
  if (!motorStopped) {
    rotateCW(5); // Rotating CW by a small angle
  }
  if (digitalRead(LEFT_SENSOR_PIN) == LOW) {
    // Rotating 8 degrees CCW
    motorStopped = false;
    delay(2000);
    rotateCCW(8);
    delay(2000); // Delay for stability
    stopMotor();
  }
}



void setup() {
  pinMode(CW, OUTPUT);
  pinMode(CCW, OUTPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LEFT_SENSOR_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}
//22/72 *8
void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == "CW" || command == "CCW" || command == "STOP" || command == "RESET" || command == "MOVE" || command == "STATUS") {
      //stopMotor();
      //delay(2000);
      if (command == "CW") {
        whenReset = false;
        rotateCW(90); // Rotate 90 degrees clockwise
        motorStopped = true;
        Serial.println("CW : 90");

      } else if (command == "CCW") {
        whenReset = false;
        rotateCCW(90); // Rotate 90 degrees counterclockwise
        motorStopped = true;
        Serial.println("CW : 90");

        } else if (command == "STOP") {
        whenReset = false;
        stopMotor(); 
        motorStopped = true;
        Serial.println("STOPPED");

        } else if (command == "STATUS") {
        whenReset = false;
        stopMotor(); 
        Serial.println("motorStopped? 'Moving':'Stopped'");

      } else if (command == "RESET") {
        whenReset = true;
        reset();
        Serial.println("HOME BASE");
      } else if (command == "STATUS") {
        Serial.println("motorStopped? 'Moving':'Stopped'");
      }
      else if (command == "MOVE") {
        whenReset = false;
        robotMovingStart();
        Serial.println("Started Moving");
    }

  }
}
}



