#include <HardwareSerial.h>
#include <ODriveArduino.h>
#include <math.h>
#include <avr/io.h>
volatile float u, q1, dq1;
volatile float px1;
volatile float vx1, vy1;
volatile float py1;

static uint32_t oldtime;
static uint32_t progtime;

template<class T> inline Print& operator <<(Print &obj,     T arg) {
  obj.print(arg);
  return obj;
}
template<>        inline Print& operator <<(Print &obj, float arg) {
  obj.print(arg, 4);
  return obj;
}

ODriveArduino odrive(Serial1);
//lx1 = 6.09365, lvx1 = 5.0283, lq = 0.609365, ldq = 0.50283
//down gains//static float lx1 = 14.8472; static float ly1 = 0.0; static float lvx1 =5.0687; static float lvy1 =  0.0; static float lq/*ltx1*/ =  1.48472  ; static float ldq/*lwx1*/ =0.50687 ;
//static float lx1 = -2016.22; static float ly1 = 0.0; static float lvx1 =-8.42255; static float lvy1 =  0.0; static float lq/*ltx1*/ = -401.159   ; static float ldq/*lwx1*/ =-0.0112297 ;
static float lx1 =6.09365; static float ly1 = 0.0; static float lvx1 =5.0283; static float lvy1 =  0.0; static float lq/*ltx1*/ =  0.609365  ; static float ldq/*lwx1*/ =0.50283 ;
float ref1 = 0.1;
void setup() {
  // put your setup code here, to run once:
  Serial1.begin(921600);
  Serial.begin(921600);
  Serial1.setTimeout(1);
  Serial1 << "sc" << "\n";
  Serial1 << "c 0 0" << "\n";
  if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
  Serial1 << "sc" << "\n";
  while (!Serial) ; /*wait for Teensy Serial Monitor*/
  /*torque control mode*/
  Serial1 << "w axis0.controller.config.control_mode 1\n";
  /*clear errors*/
  Serial1 << "sc" << "\n";
  oldtime = millis();
  progtime = millis();
  /*closed loop control*/
    Serial1 << "sc" << "\n";
  if (!odrive.run_state(0, 8, false /*don't wait*/)) return;
  Serial1.clear();
}
/*helper for getting the offsets*/
int i = 0;
/*define the offsets*/
float   offset1 = -0.173067;//these are the index offsets of the encoders, the readings are done agaain in the loop
void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - progtime) < 600000) {
    if (!Serial) {
      /*when existing the serial monitor the acrobot stops*/
      Serial1 << "c 0 0" << "\n";
      if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
    }
    else {
      if ( (millis() - oldtime) > 10) {
        if (i<4) {
          oldtime = millis();
          Serial1 << "f 0 \n";
          int myspace = 0;
          String c2 = Serial1.readString();
          String placholder2 = "!";
          c2  = placholder2.concat(c2);

          for (int i = 8; i < 11; i++) {
            if (c2.charAt(i) == ' ') {
              myspace = i;
              break;
            }
          }
          offset1 = (c2.substring(1, myspace)).toFloat();
          i++;
          u = 0;
          Serial1 << "c 0 0" << "\n";
          px1 = 0.0;
          py1 = 0.10;
        }
        else {
          oldtime = millis();
          Serial1 << "f 0 \n";
          int myspace = 0;
          String c2 = Serial1.readString();
          String placholder2 = "!";
          c2  = placholder2.concat(c2);

          for (int i = 8; i < 11; i++) {
            if (c2.charAt(i) == ' ') {
              myspace = i;
              break;
            }
          }
          q1 = (c2.substring(1, myspace)).toFloat();
          q1 = get_angle(offset1, q1);
          dq1  = ((c2.substring(myspace + 1)).toFloat()) * 2 * PI;
          px1 = 0.1 * sinf(q1);
          //py1 = -0.1 * cosf(q1);
          vx1 =  0.1 * cosf(q1) * dq1;
          //vy1 =  0.1 * sinf(q1) * dq1;
          Serial.print(',');
          Serial.print(px1);
          Serial.print(',');
          Serial.print(vx1);
          Serial.print(',');
          Serial.print(q1);
          Serial.print(',');
          Serial.print(dq1);
          Serial.print(',');
          u = (lq * q1 + ldq * dq1 + lx1 * px1 + lvx1 *vx1) * -1;
         Serial1 << "c 0" << " " << u << "\n";
          Serial.println(u);
        }
      }
    }
  }
  else {
    if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
   Serial1 << "c 0" << " " << 0 << "\n";
  }
}
float get_angle(float ref, float val)
{
  ref = ref - trunc(ref);
  val = val - trunc(val);
  if (ref < 0) {
    ref = ref + 1.0;
  }
  if (val < 0) {
    val = val + 1.0;
  }
  float diff = val - ref;
  if (diff > 0.5) {
    diff = diff - 1.0;
  }
  else if (diff < -0.5) {
    diff = diff + 1.0;
  }
  return diff * 2 * PI;
}
