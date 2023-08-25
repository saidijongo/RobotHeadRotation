#define CW 5   
#define CCW 6  
#define RIGHT_SENSOR_PIN 11   // Right photomicrosensor pin
#define LEFT_SENSOR_PIN 10   // Left photomicrosensor pin

int stepDelay = 10000;
bool whenReset = true;
bool motorStopped = false;
bool commandInProgress = false;

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

void reset() {
  if (whenReset) {
    while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
      if (Serial.available() > 0 && Serial.readStringUntil('\n') == "STOP") {
        stopMotor();
        return;
      }
      rotateCCW(5); // Rotating CCW until right sensor is interrupted
    }
    delay(1000); // delay for stability

    while (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
      if (Serial.available() > 0 && Serial.readStringUntil('\n') == "STOP") {
        stopMotor();
        return;
      }
      rotateCW(8);
      delay(2000); // Delay for stability
      stopMotor();
      delay(1000); // Delay before checking again
    }
  }
}

void robotMovingStart() {
  if (!motorStopped) {
    while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
      if (Serial.available() > 0 && Serial.readStringUntil('\n') == "STOP") {
        stopMotor();
        return;
      }
      rotateCW(5); // Rotating CW until left sensor is interrupted
    }
    delay(1000); // delay for stability

    while (digitalRead(LEFT_SENSOR_PIN) == LOW) {
      if (Serial.available() > 0 && Serial.readStringUntil('\n') == "STOP") {
        stopMotor();
        return;
      }
      rotateCCW(8);
      delay(2000); // Delay for stability
      stopMotor();
      delay(1000); // Delay before checking again
    }
  }
}

void setup() {
  pinMode(CW, OUTPUT);
  pinMode(CCW, OUTPUT);
  pinMode(RIGHT_SENSOR_PIN, INPUT_PULLUP);
  pinMode(LEFT_SENSOR_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (!commandInProgress) {
      if (command == "CW" || command == "CCW" || command == "STOP" || command == "RESET" || command == "MOVE" || command == "STATUS") {
        commandInProgress = true;
        
        // Reset sensors before executing any command
        digitalWrite(CW, LOW);
        digitalWrite(CCW, LOW);
        
        if (command == "CW") {
          rotateCW(90); // Rotate 90 degrees clockwise
          motorStopped = true;
          Serial.println("CW : 90");
        } else if (command == "CCW") {
          whenReset = false;
          rotateCCW(90); // Rotate 90 degrees counterclockwise
          motorStopped = true;
          Serial.println("CCW : 90");
        } else if (command == "STOP") {
          stopMotor(); 
          Serial.println("STOPPED");
        } else if (command == "STATUS") {
          Serial.println(motorStopped ? "Stopped" : "Moving");
        } else if (command == "RESET") {
          reset();
          Serial.println("HOME BASE");
        } else if (command == "MOVE") {
          robotMovingStart();
          Serial.println("STARTED MOVING");
        }
        commandInProgress = false;
      }
    }
  }
}
