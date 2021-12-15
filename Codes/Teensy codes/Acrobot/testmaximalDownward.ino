#include <HardwareSerial.h>
#include <ODriveArduino.h>
#include <math.h>
#include <avr/io.h>

volatile float u, q1, q2, dq1, dq2;
volatile float px1, px2, py2;
volatile float vx1, vx2, vy1, vy2;
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
//0.0  -260.321  0.0  0.0  0.0669964  0.0  85.6119  0.0  0.0  0.0467555  0.0  0.0  0.0  -25.4533  0.0  0.0  2.62058  0.0  7.38782  0.0  0.0  0.0835332  0.0  0.0
//0.0  -161.513  0.0  0.0  6.46237  0.0  -1.0467  0.0  0.0  0.0794028  0.0  0.0  0.0  32.5986  0.0  0.0  9.28337  0.0  8.98766  0.0  0.0  0.143635  0.0  0.0
//static float ly1 = -161.513; static float lz1 = 0.0; static float lvy1 = 6.46237  ; static float lvz1 =  0.0; static float l1/*ltx1*/ =  -1.0467; static float l3/*lwx1*/ = 0.0794028;
//static float ly2 = 32.5986; static float lz2 = 0.0; static float lvy2 = 9.28337; static float lvz2 = 0.0; static float l2/*ltx2*/ = 8.98766; static float l4/*lwx2*/ = 0.143635;
//static float ly1 =-260.321; static float lz1 = 0.0; static float lvy1 =0.0669964  ; static float lvz1 =  0.0; static float l1/*ltx1*/ = 85.6119 ; static float l3/*lwx1*/ =  0.0467555;
//static float ly2 = -25.4533 ; static float lz2 = 0.0; static float lvy2 = 2.62058; static float lvz2 = 0.0; static float l2/*ltx2*/ = 7.38782; static float l4/*lwx2*/ =   0.0835332;
//static float ly1 =-99.4308; static float lz1 = 0.0; static float lvy1 =1.72413 ; static float lvz1 =  0.0; static float l1/*ltx1*/ = 36.2608 ; static float l3/*lwx1*/ =  0.0427897;
//static float ly2 = 13.0616 ; static float lz2 = 0.0; static float lvy2 = 2.43281; static float lvz2 = 0.0; static float l2/*ltx2*/ = 0.33327; static float l4/*lwx2*/ = 0.0487164;
static float ly1 =-105.41; static float lz1 = 0.0; static float lvy1 =8.64147 ; static float lvz1 =  0.0; static float l1/*ltx1*/ = -1.834 ; static float l3/*lwx1*/ =  0.117364;
static float ly2 = 58.7459 ; static float lz2 = 0.0; static float lvy2 = 7.18558; static float lvz2 = 0.0; static float l2/*ltx2*/ = 5.26996; static float l4/*lwx2*/ = 0.101116;
float ref1 = -0.1625;
float ref2 = -(0.325 + 0.1);

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(921600);
  Serial.begin(921600);
  Serial1.setTimeout(1);
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
  if (!odrive.run_state(0, 8, false /*don't wait*/)) return;
  Serial1.clear();
}
/*helper for getting the offsets*/
int i = 0;
/*define the offsets*/
float   offset1 = 0.0303650;//these are the index offsets of the encoders, the readings are done agaain in the loop as we cant make that the first encoder return to the index offset as axis 1 is not actuated
float   offset2 = -0.001007;
void loop() {
  // put your main code here, to run repeatedly:
   if ((millis() - progtime) < 60000) {
  if (!Serial) {
    /*when existing the serial monitor the acrobot stops*/
    Serial1 << "c 0 0" << "\n";
    if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
  }
  else {
    
    if ( (millis() - oldtime) > 10) {
      if (i == 0) {
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

        offset2 = (c1.substring(1, myspace)).toFloat();

        Serial1 << "f 1 \n";
        myspace = 0;
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
        px2 = 0.0;
        py1 = 0.1625;
        py2 = 0.425;

      }
      else {
        oldtime = millis();

        Serial1 << "f 1 \n";
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
        px1 =  0.1625 * sinf(q1);
        //py1 = -0.1625 * cosf(q1);
        vx1 =  0.1625 * cosf(q1) * dq1;
        //vy1 =  0.1625 * sinf(q1) * dq1;
        Serial.print(',');
        Serial.print(q1);
        Serial.print(',');
        Serial.print(dq1);
        Serial.print(',');
        Serial1 << "f 0\n";

        String c1 = Serial1.readString();
        String placholder = "!";
        c1  = placholder.concat(c1);
        myspace = 0;

        for (int i = 8; i < 11; i++) {
          if (c1.charAt(i) == ' ') {
            myspace = i;
            break;
          }
        }

        q2 = (c1.substring(1, myspace)).toFloat();
        q2 = get_angle(offset2, q2);
        dq2  = ((c1.substring(myspace + 1)).toFloat()) * 2 * PI;
        px2 =  0.10 * sinf(q1 + q2) + 0.325 * sinf(q1);
        //py2 = -0.10 * cosf(q1 + q2) - 0.325 * cosf(q1);
        vx2 =  0.10 * cosf(q1 + q2) * (dq1 + dq2) + 0.325 * cosf(q1) * dq1;
        // vy2 =  0.10 * sinf(q1 + q2) * (dq1 + dq2) + 0.325 * sinf(q1) * dq1;

        Serial.print(q2);
        Serial.print(',');
        Serial.print(dq2);
        Serial.print(',');
        // u = (l1 * q1 + l2 * q2 + l3 * dq1 + l4 * dq2 + ly1 * (px1) + ly2 * (px2) + lz1 * (py1 + 0.1625) + lz2 * (py2 + 0.425) + lvy1 * (vx1) + lvy2 * (vx2) + lvz1 * (vy1) + lvz2 * (vy2)) * -1;
       u = (l1 * q1 + l2 * q2 + l3 * dq1 + l4 * dq2 + ly1 * (px1) + ly2 * (px2) + lvy1 * (vx1) + lvy2 * (vx2) ) * -0.2;
       Serial1 << "c 0" << " " << u << "\n";

        Serial.println(u);
      }

    }

  }
   }
 else{
    if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
        Serial1 << "c 0" << " " << 0<< "\n";
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
