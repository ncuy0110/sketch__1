#include <PS2X_lib.h>
#include <math.h>
// PS2 Tank by Igor Fonseca @2019
// Controls a robotic tank using a PS2 joystick, using D-pad buttons
// based on an example using the PS2X library by Bill Porter 2011
// All text above must be included in any redistribution.

// include libraries


// These are used to set the direction of the bridge driver.

#define PS2_DAT  13
#define PS2_CMD  11
#define PS2_ATT 10
#define PS2_CLK 12

#define MOTORA_1 7 //IN3
#define MOTORA_2 8 //IN4
#define MOTORB_3 4 //IN1
#define MOTORB_4 5 //IN2

#define PWM_MOTOR_1 9
#define PWM_MOTOR_2 6

int LX = 0;
int LY = 0;
int RX = 0;
int RY = 0;

int speed = 135;
PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning
//you must always either restart your Arduino after you conect the controller,
//or call config_gamepad(pins) again after connecting the controller.
int error = 0;
byte type = 0;
byte vibrate = 0;

void setup() {

  // Configure output pins

  pinMode(MOTORA_1, OUTPUT);
  pinMode(MOTORA_2, OUTPUT);

  pinMode(MOTORB_3, OUTPUT);
  pinMode(MOTORB_4, OUTPUT);

  pinMode(PWM_MOTOR_1, OUTPUT);
  pinMode(PWM_MOTOR_2, OUTPUT);



  // Disable both motors


  // Start serial communication
  Serial.begin(9600);

  //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, false, false);

  // Check for error
  if (error == 0) {
    Serial.println("Đã tìm thấy Bộ điều khiển, đã định cấu hình thành công");
  }

  else if (error == 1)
    Serial.println("Không tìm thấy bộ điều khiển, hãy kiểm tra hệ thống dây điện hoặc đặt lại Arduino");

  else if (error == 2)
    Serial.println("Đã tìm thấy bộ điều khiển nhưng không chấp nhận lệnh");

  else if (error == 3)
    Serial.println("Bộ điều khiển từ chối vào chế độ Pressures, có thể không hỗ trợ nó.");

  // Check for the type of controller
  type = ps2x.readType();
  switch (type) {
    case 0:
      Serial.println("Loại bộ điều khiển không xác định");
      break;
    case 1:
      Serial.println("Đã tìm thấy bộ điều khiển DualShock");
      break;
    case 2:
      Serial.println("Bộ điều khiển GuitarHero");
      break;
  }
}

// Main loop
void loop() {



  if (error == 1) //skip loop if no controller found
    return;

  else { //DualShock Controller

    ps2x.read_gamepad(false, vibrate); // disable vibration of the controller
    delay(50);
    if (ps2x.Button(PSB_L1))
    {
      LY = ps2x.Analog(PSS_LY);                     //receive values from p22 joystick
      LX = ps2x.Analog(PSS_LX);

      RY = ps2x.Analog(PSS_RY);
      RX = ps2x.Analog(PSS_RX);
      //LX, LY, RX, RY

      //di thang
      if (LY > 200 || RY > 200)                       //check if the joystick pushed up side
      {
        forward((LY > 200) ? LY : RY);
      }

      //di lui
      if (LY < 100 || RY < 100)
      {
        REV((LY < 100) ? LY : RY);
      }

      //re trai
      if (LX < 100 || RX < 100)
      {
        left((LX < 100) ? LX : RX);
      }

      //re phai
      if (LX > 200 || RX > 200)
      {
        right((LX > 200) ? LX : RX);
      }
      //Tang toc
      if (ps2x.Button(PSB_GREEN)) if (speed < 255) speed = speed + 30;

      //Giam toc
      if (ps2x.Button(PSB_BLUE)) if (speed > 135) speed = speed - 30;

      //set ve muc 1
      if (ps2x.Button(PSB_PINK)) speed = 135;

      //set ve muc 5
      if (ps2x.Button(PSB_RED)) speed = 255;
      //dung
      if (LX == 128 && LY == 128 && RX == 128 && RY == 128)
      {
        stop();
      }
      LY = LX = 128;         //return to default vlaues
      RY = RX = 128;         //return to default values
    } else {
      // Perform movements based on D-pad buttons

      // re phai
      if (ps2x.Button(PSB_PAD_RIGHT)) right(128);

      // re trai
      if (ps2x.Button(PSB_PAD_LEFT)) left(128);

      // chay thang
      if (ps2x.Button(PSB_PAD_UP)) REV(128);

      // chay lui
      if (ps2x.Button(PSB_PAD_DOWN)) forward(128);

      //Tang toc
      if (ps2x.Button(PSB_GREEN)) if (speed < 255) speed = speed + 30;

      //Giam toc
      if (ps2x.Button(PSB_BLUE)) if (speed > 135) speed = speed - 30;

      //set ve muc 1
      if (ps2x.Button(PSB_PINK)) speed = 135;

      //set ve muc 5
      if (ps2x.Button(PSB_RED)) speed = 255;

      //Stop
      if (ps2x.Button(PSB_PAD_UP) == LOW && ps2x.Button(PSB_PAD_DOWN) == LOW && ps2x.Button(PSB_PAD_RIGHT) == LOW && ps2x.Button(PSB_PAD_LEFT) == LOW) stop();
    }





  }

}


//ham set toc do dong co theo do nhan, va theo nut nhan
void setSpeed(int k) {
  if (k != 128) {
    analogWrite(PWM_MOTOR_1, (abs(128 - k) / 127)*speed );
    analogWrite(PWM_MOTOR_2, (abs(128 - k) / 127)*speed );
  } else {
    analogWrite(PWM_MOTOR_1, speed);
    analogWrite(PWM_MOTOR_2, speed);
  }
}
//ham chay thang
void forward(int k) {
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);
  digitalWrite(MOTORB_3, LOW);
  digitalWrite(MOTORB_4, HIGH);
  setSpeed(k);

  Serial.println("Turn REV");
}

//ham chay lui
void REV(int k) {
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, HIGH);
  digitalWrite(MOTORB_3, HIGH);
  digitalWrite(MOTORB_4, LOW);
  setSpeed(k);

  Serial.println("Turn forward");
}

//ham re trai
void left(int k) {
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, HIGH);
  digitalWrite(MOTORB_3, LOW);
  digitalWrite(MOTORB_4, HIGH);
  analogWrite(PWM_MOTOR_1, 135);
  analogWrite(PWM_MOTOR_2, 135);

  Serial.println("Move left");
}

//ham re phai
void right(int k) {
  digitalWrite(MOTORA_1, HIGH);
  digitalWrite(MOTORA_2, LOW);
  digitalWrite(MOTORB_3, HIGH);
  digitalWrite(MOTORB_4, LOW);
  analogWrite(PWM_MOTOR_1, 135);
  analogWrite(PWM_MOTOR_2, 135);

  Serial.println("Move right");
}

//ham dung
void stop() {
  digitalWrite(MOTORA_1, LOW);
  digitalWrite(MOTORA_2, LOW);
  digitalWrite(MOTORB_3, LOW);
  digitalWrite(MOTORB_4, LOW);
  analogWrite(PWM_MOTOR_1, 0);
  analogWrite(PWM_MOTOR_2, 0);
}
