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
   if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
Serial1 << "c 0" << " " << 0<< "\n";
  while (!Serial) ; /*wait for Teensy Serial Monitor -5.16695  -0.229677  -0.03272  1.0219*/

  Serial1 << "w axis0.controller.config.control_mode 1\n";
  
  Serial1 << "sc" << "\n";
  oldtime = millis();
progtime = millis();
  /*closed loop control*/
//if (!odrive.run_state(0, 8, false /*don't wait*/)) return;
  /*axis state idle*/
  // if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
  //Serial << Serial1.readString() << "\n";
  Serial1.clear();
  u= 0;


}
/*helper for getting the offset*/
int i = 0;
/**/
float   offset1;
float   offset2 ;

/*Control gains       */
//static float l1 = -8.77003;  /*Bryson*/     
//static float l2 = 0.975133;
//static float l3 = 0.982186;
//static float l4 = 0.393173;
//
static float l1 = -1.61576;/*Q=10 R=1**/
static float l2 = -0.143523;
static float l3 = -0.00623429 ;
static float l4 = 0.324385;

static float l1 = -19.7332;/*same maximal*/
static float l2 = 7.69556;
static float l3 = 4.84155 ;
static float l4 = 1.30125;
//static float l1 = -2.29589;
//static float l2 = -0.165354;
//static float l3 = -0.0122479 ;
//static float l4 = 0.457796;

//static float l1 = -3.88;       
//static float l2 = 0.863;
//static float l3 = 0.847;
//static float l4 = 0.174;  
    
//static float l1 = -6.38852;Bryson's
//static float l2 =1.59481;
//static float l3 = 1.4832;
//static float l4 = 0.284831;

//-1.63859  0.0445174  0.0280187  0.322441

//static float l1 = -1.63735 ;/*Q = I, R = 10*/
//static float l2 =0.0420334 ;
//static float l3 = 0.0279742;
//static float l4 = 0.322487;

//static float l1 = -25.01;
//static float l2 =1.84 ;
//static float l3 = 3.64;
//static float l4 = 0.53;
static float l1 = -12.173;/*same maximal weighta*/
static float l2 =9.13344;
static float l3 =5.5163;
static float l4 =0.805554;
void loop()
{
if(!Serial){
   if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
        Serial1 << "c 0" << " " << 0<< "\n";
  }
 if ((millis() - progtime) < 60000) {
  if ( (millis() - oldtime) > 10) {

    if (i == 0) {/*take the offset at the start of the loop and send a  zero torque*/
      oldtime = millis();

      Serial1 << "f 0\n";

      String c1 = Serial1.readString();
 //      Serial << "f0  " << c1 << "\n";

      String placholder = "!";

      c1  = placholder.concat(c1);
      int myspace = 0;
      for (int i = 8; i < 11; i++) {
        if (c1.charAt(i) == ' ') {
          myspace = i;
          break;
        }
      }

      offset1 = (c1.substring(1, myspace)).toFloat();
      endstopvel  = (c1.substring(myspace + 1)).toFloat();

      //      Serial << "p " << offset1 << "\n";

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

      offset2 = (c2.substring(1, myspace)).toFloat();
      endstopvel  = (c2.substring(myspace + 1)).toFloat();

      //      Serial << "p1 " << offset2 << "\n";
      i++;
      Serial1 << "c 0 0" << "\n";
      //      Serial << "The Acrobot is ready!";
    }
    else {
      oldtime = millis();

      Serial1 << "f 0\n";

      String c1 = Serial1.readString();
      //      Serial << "f0  " << c1 << "\n";

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
      Serial.print(q,4);
      Serial.print(',');
      Serial.print(dq,4);
      //      Serial << "p " << q << "\n";
      //      Serial << "v  " << dq << "\n";

      Serial1 << "f 1 \n";
      myspace = 0;

      String c2 = Serial1.readString();
      //Serial << "f1  " << c2 << "\n";

      String placholder2 = "!";
      c2  = placholder2.concat(c2);
      for (int i = 8; i < 11; i++) {
        if (c2.charAt(i) == ' ') {
          myspace = i;
          break;
        }
      }

      q0 = (c2.substring(1, myspace)).toFloat();
      q0 = get_angle(offset2, q0);
      dq0  = ((c2.substring(myspace + 1)).toFloat()) * 2 * PI;
      Serial.print(',');
      Serial.print(q0,4);
      Serial.print(',');
      Serial.print(dq0,4);
      /*Calculate the torque and send it to the odrive*/
      u = (l1 * q0 + l2 * q + l3 * dq0 + l4 * dq) * -0.2;
//      if (u > 1.8 || u < -1.8) {
//        if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
//        Serial1 << "c 0" << " " << 0<< "\n";
//      }
  // Serial1 << "c 0" << " " << u << "\n";
      //
      //      Serial << "p1 " << q0 << "\n";
      //      Serial << "v1  " << dq0 << "\n";
      //      Serial << "Torque: " << u << "\n";
      Serial.print(',');
      Serial.println(u,4);

    }

  }
 }
 else{
    if (!odrive.run_state(0, 1, false /*don't wait*/)) return;
        Serial1 << "c 0" << " " << 0<< "\n";
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
