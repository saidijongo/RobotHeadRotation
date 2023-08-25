#define CW 5   
#define CCW 6  
#define RIGHT_SENSOR_PIN 11   // Right photomicrosensor pin
#define LEFT_SENSOR_PIN 10    // Left photomicrosensor pin

int stepDelay = 10000;
bool motorStopped = true;
bool cw_ccw = true; // Flag to track the direction of rotation

void rotate(int angleDegrees, bool isClockwise) {
  int steps = angleDegrees / 0.72;
  int motorPin = isClockwise ? CW : CCW;
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(motorPin, HIGH);
    delayMicroseconds(stepDelay); 
    digitalWrite(motorPin, LOW);
    delayMicroseconds(stepDelay);
  }
}

void setup() {
  pinMode(CW, OUTPUT);
  pinMode(CCW, OUTPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LEFT_SENSOR_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void stopMotor() {
  digitalWrite(CW, LOW);
  digitalWrite(CCW, LOW);
  motorStopped = true;
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command == "CW") {
      rotate(90, true); // Rotate 90 degrees clockwise
      motorStopped = true;
      Serial.println("CW : 90");
      stopMotor();
      cw_ccw = true;
    } else if (command == "CCW") {
      while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
        rotate(8, true); // Rotate 8 degrees clockwise until right sensor is interrupted
      }
      stopMotor();
      delay(1000); // delay for stability
      Serial.println("CCW : 8");
      cw_ccw = true;
    } else if (command == "STOP") {
      stopMotor();
      Serial.println("STOPPED");
    } else if (command == "RESET") {
      while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
        rotate(5, false); // Rotating CCW until right sensor is interrupted
      }
      delay(1000); // delay for stability

      while (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
        rotate(8, true);
        delay(2000); // Delay for stability
        stopMotor();
        delay(1000); // Delay before checking again
      }
      cw_ccw = true;
    } else if (command == "MOVE") {
      while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
        rotate(5, true); // Rotating CW until left sensor is interrupted
      }
      delay(1000); // delay for stability

      while (digitalRead(LEFT_SENSOR_PIN) == LOW) {
        rotate(8, false);
        delay(2000); // Delay for stability
        stopMotor();
        delay(1000); // Delay before checking again
      }
      cw_ccw = false;
    } else if (command == "STATUS") {
      Serial.println(motorStopped ? "Stopped" : "Moving");
    }
  }
}
