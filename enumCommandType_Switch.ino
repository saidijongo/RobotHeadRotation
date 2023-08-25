#define CW 5   
#define CCW 6  
#define RIGHT_SENSOR_PIN 11   // Right photomicrosensor pin
#define LEFT_SENSOR_PIN 10   // Left photomicrosensor pin

int stepDelay = 10000;
bool whenReset = true;
bool motorStopped = false;

enum CommandType {
  NONE,
  CW_ROTATE,
  CCW_ROTATE,
  STOP,
  RESET,
  MOVE,
  STATUS
};

CommandType currentCommand = NONE;

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

void rotateCCW(int angleDegrees) {
  rotate(angleDegrees, false);
}

void rotateCW(int angleDegrees) {
  rotate(angleDegrees, true);
}

void stopMotor() {
  digitalWrite(CW, LOW);
  digitalWrite(CCW, LOW);
}

void processCommand(String command) {
  command.trim();
  
  if (command == "CW") {
    currentCommand = CW_ROTATE;
  } else if (command == "CCW") {
    currentCommand = CCW_ROTATE;
  } else if (command == "STOP") {
    currentCommand = STOP;
  } else if (command == "RESET") {
    currentCommand = RESET;
  } else if (command == "MOVE") {
    currentCommand = MOVE;
  } else if (command == "STATUS") {
    Serial.println(motorStopped ? "Stopped" : "Moving");
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
    processCommand(Serial.readStringUntil('\n'));
  }

  switch (currentCommand) {
    case CW_ROTATE:
      rotateCW(90); // Rotate 90 degrees clockwise
      motorStopped = true;
      Serial.println("CW : 90");
      currentCommand = NONE;
      break;
      
    case CCW_ROTATE:
      whenReset = false;
      rotateCCW(90); // Rotate 90 degrees counterclockwise
      motorStopped = true;
      Serial.println("CCW : 90");
      currentCommand = NONE;
      break;
      
    case STOP:
      stopMotor();
      Serial.println("STOPPED");
      currentCommand = NONE;
      break;
      
    case RESET:
      while (digitalRead(RIGHT_SENSOR_PIN) == HIGH) {
        rotateCCW(5); // Rotating CCW until right sensor is interrupted
      }
      delay(1000); // delay for stability

      while (digitalRead(RIGHT_SENSOR_PIN) == LOW) {
        rotateCW(8);
        delay(2000); // Delay for stability
        stopMotor();
        delay(1000); // Delay before checking again
      }
      currentCommand = NONE;
      break;
      
    case MOVE:
      while (digitalRead(LEFT_SENSOR_PIN) == HIGH) {
        rotateCW(5); // Rotating CW until left sensor is interrupted
      }
      delay(1000); // delay for stability

      while (digitalRead(LEFT_SENSOR_PIN) == LOW) {
        rotateCCW(8);
        delay(2000); // Delay for stability
        stopMotor();
        delay(1000); // Delay before checking again
      }
      currentCommand = NONE;
      break;
  }
}
