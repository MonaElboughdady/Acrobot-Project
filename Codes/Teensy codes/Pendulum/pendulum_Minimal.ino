#include <HardwareSerial.h>
#include <ODriveArduino.h>
#include <math.h>
#include <avr/io.h>

volatile float u, q0, q, dq0, dq, endstopPos, endstopvel;
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
void setup()
{ Serial1.begin(921600);
  Serial.begin(921600);
  Serial1.setTimeout(1);
    Serial1 << "w axis0.controller.config.control_mode 1\n";
  if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
  Serial1 << "c 0" << " " << 0 << "\n";
  while (!Serial) ; /*wait for Teensy Serial Monitor -5.16695  -0.229677  -0.03272  1.0219*/
/*closed loop control*/
if (!odrive.run_state(0, 8, false /*don't wait*/)) return;
  u = 0;
  Serial1.clear();
  oldtime = millis();
  progtime = millis();
}
/*helper for getting the offset*/
int i = 0;
/**/
float   offset1 =  -0.669649;/* -0.173067*/
/*Control gains       */
static float l1 = 2.96944;//downward gains q11= 10
static float l2 = 1.01374;
//static float l1 = 1.21873;
//static float l2 = 1.00566;

void loop()
{
  if (!Serial) {
    if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
    Serial1 << "c 0" << " " << 0 << "\n";
  }
  if ((millis() - progtime) < 30000) {
    if ( (millis() - oldtime) > 10) {

      if (i <3) {/*take the offset at the start of the loop and send a  zero torque*/
        oldtime = millis();
String placholder = "!";
        Serial1 << "f 0\n";

        String c1 = Serial1.readString();

        

        c1  = placholder.concat(c1);
        int myspace = 0;
        for (int i = 8; i < 11; i++) {
          if (c1.charAt(i) == ' ') {
            myspace = i;
            break;
          }
        }

        offset1 = (c1.substring(1, myspace)).toFloat();
        Serial.println(offset1);
        endstopvel  = (c1.substring(myspace + 1)).toFloat();
        i++;
        Serial1 << "c 0 0" << "\n";
        
      }
      else {
        oldtime = millis();

        Serial1 << "f 0\n";
        String c1 = Serial1.readString();
        String placholder = "!";
        c1  = placholder.concat(c1);
        int myspace = 0;
        for (int i = 8; i < 11; i++) {
          if (c1.charAt(i) == ' ') {
            myspace = i;
            break;
          }
        }

        q = (c1.substring(1, myspace)).toFloat();
        q = get_angle(offset1, q);
        dq  = ((c1.substring(myspace + 1)).toFloat()) * 2 * PI;
        Serial.print(',');
        Serial.print(q, 4);
        Serial.print(',');
        Serial.print(dq, 4);
        /*Calculate the torque and send it to the odrive*/
        u = (l1 * q + l2 * dq) * -1;

   Serial1 << "c 0" << " " << u << "\n";
        Serial.print(',');
        Serial.println(u, 4);

      }

    }
  }
  else {
    if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
    Serial1 << "c 0" << " " << 0 << "\n";
  }
}
/*Adjusting the angle*/
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
