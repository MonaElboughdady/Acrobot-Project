#include <HardwareSerial.h>
#include <ODriveArduino.h>
#include <avr/io.h>
int count = 0;

//initialize the checksum lOOKUP array
unsigned short crctable16[256] =
{ 0x0000, 0x3D65, 0x7ACA, 0x47AF, 0xF594, 0xC8F1, 0x8F5E, 0xB23B, 0xD64D,
  0xEB28, 0xAC87, 0x91E2, 0x23D9, 0x1EBC, 0x5913, 0x6476, 0x91FF,
  0xAC9A, 0xEB35, 0xD650, 0x646B, 0x590E, 0x1EA1, 0x23C4, 0x47B2,
  0x7AD7, 0x3D78, 0x001D, 0xB226, 0x8F43, 0xC8EC, 0xF589, 0x1E9B,
  0x23FE, 0x6451, 0x5934, 0xEB0F, 0xD66A, 0x91C5, 0xACA0, 0xC8D6,
  0xF5B3, 0xB21C, 0x8F79, 0x3D42, 0x0027, 0x4788, 0x7AED, 0x8F64,
  0xB201, 0xF5AE, 0xC8CB, 0x7AF0, 0x4795, 0x003A, 0x3D5F, 0x5929,
  0x644C, 0x23E3, 0x1E86, 0xACBD, 0x91D8, 0xD677, 0xEB12, 0x3D36,
  0x0053, 0x47FC, 0x7A99, 0xC8A2, 0xF5C7, 0xB268, 0x8F0D, 0xEB7B,
  0xD61E, 0x91B1, 0xACD4, 0x1EEF, 0x238A, 0x6425, 0x5940, 0xACC9,
  0x91AC, 0xD603, 0xEB66, 0x595D, 0x6438, 0x2397, 0x1EF2, 0x7A84,
  0x47E1, 0x004E, 0x3D2B, 0x8F10, 0xB275, 0xF5DA, 0xC8BF, 0x23AD,
  0x1EC8, 0x5967, 0x6402, 0xD639, 0xEB5C, 0xACF3, 0x9196, 0xF5E0,
  0xC885, 0x8F2A, 0xB24F, 0x0074, 0x3D11, 0x7ABE, 0x47DB, 0xB252,
  0x8F37, 0xC898, 0xF5FD, 0x47C6, 0x7AA3, 0x3D0C, 0x0069, 0x641F,
  0x597A, 0x1ED5, 0x23B0, 0x918B, 0xACEE, 0xEB41, 0xD624, 0x7A6C,
  0x4709, 0x00A6, 0x3DC3, 0x8FF8, 0xB29D, 0xF532, 0xC857, 0xAC21,
  0x9144, 0xD6EB, 0xEB8E, 0x59B5, 0x64D0, 0x237F, 0x1E1A, 0xEB93,
  0xD6F6, 0x9159, 0xAC3C, 0x1E07, 0x2362, 0x64CD, 0x59A8, 0x3DDE,
  0x00BB, 0x4714, 0x7A71, 0xC84A, 0xF52F, 0xB280, 0x8FE5, 0x64F7,
  0x5992, 0x1E3D, 0x2358, 0x9163, 0xAC06, 0xEBA9, 0xD6CC, 0xB2BA,
  0x8FDF, 0xC870, 0xF515, 0x472E, 0x7A4B, 0x3DE4, 0x0081, 0xF508,
  0xC86D, 0x8FC2, 0xB2A7, 0x009C, 0x3DF9, 0x7A56, 0x4733, 0x2345,
  0x1E20, 0x598F, 0x64EA, 0xD6D1, 0xEBB4, 0xAC1B, 0x917E, 0x475A,
  0x7A3F, 0x3D90, 0x00F5, 0xB2CE, 0x8FAB, 0xC804, 0xF561, 0x9117,
  0xAC72, 0xEBDD, 0xD6B8, 0x6483, 0x59E6, 0x1E49, 0x232C, 0xD6A5,
  0xEBC0, 0xAC6F, 0x910A, 0x2331, 0x1E54, 0x59FB, 0x649E, 0x00E8,
  0x3D8D, 0x7A22, 0x4747, 0xF57C, 0xC819, 0x8FB6, 0xB2D3, 0x59C1,
  0x64A4, 0x230B, 0x1E6E, 0xAC55, 0x9130, 0xD69F, 0xEBFA, 0x8F8C,
  0xB2E9, 0xF546, 0xC823, 0x7A18, 0x477D, 0x00D2, 0x3DB7, 0xC83E,
  0xF55B, 0xB2F4, 0x8F91, 0x3DAA, 0x00CF, 0x4760, 0x7A05, 0x1E73,
  0x2316, 0x64B9, 0x59DC, 0xEBE7, 0xD682, 0x912D, 0xAC48
};

//Adjust the angle feedback from the incremental encoders, by subtracting from the offset and limit the angle values(-0.5pi:0.5pi)
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
//Compute CRC16 for the stream native commands the odrive
unsigned short Compute_CRC16 (unsigned char bytes[], int size)
{
  unsigned short crc = 0x1337;
  int i;
  for (i = 3; i < size - 2; i++)
  {
    unsigned char b = bytes[i];
    /* XOR-in next input byte into MSB of crc, that's our new intermediate divident */
    unsigned char pos = (unsigned char) ((crc >> 8) ^ b);  /* equal: ((crc ^ (b << 8)) >> 8) */
    /* Shift out the MSB used for division per lookuptable and XOR with the remainder */
    crc = (unsigned short) ((crc << 8) ^ (unsigned short) (crctable16[pos]));
  }
  return crc;
}

//the idle axis Native command
byte msgidle[] = {0xAA, 0x09, 0x0A, 0xF2, 0x03, 0x82, 0x00, 0x01, 0x00, 0x01, 0x53, 0x5E, 0x64, 0xA8};
//closed loop control loop command
byte msgClControl[] = {0xAA, 0x09, 0x0A, 0xF3, 0x03, 0x82, 0x00, 0x01, 0x00, 0x08, 0x53, 0x5E, 0x8E, 0xB9};
/*TODO add the control mode torque*/
//The input torque command initially the input torque is zero(inputTorque[9] to inputTorque[12]), CRC16(For the packrt is inputTorque[15] to inputTorque[16])
byte inputTorque[] = {0xAA, 0xC, 0xE1, 0xF3, 0x03, 0xf9, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x5E, 0xB5, 0xC4};/*needs CRC16 check*/
//the command for getting the position for encoder0(Link 2)
byte reqPos2[] = {0xAA, 0x08, 0x3D, 0x8C, 0x03, 0x37, 0x81, 0x04, 0x00, 0x53, 0x5E, 0xF0, 0x09};
//the command for getting the velocity for encoder0(Link 2)
byte reqVel2[] = {0xAA, 0x08, 0x3D, 0xD5, 0x03, 0x3D, 0x81, 0x04, 0x00, 0x53, 0x5E, 0xF7, 0x9B};
//the command for getting the position for encoder1(Link 1)
byte reqPos1[] = {0xAA, 0x08, 0x3D, 0xF0, 0x03, 0x69, 0x82, 0x04, 0x00, 0x53, 0x5E, 0xC1, 0x57};
//the command for getting the velocity for encoder1(Link 1)
byte reqVel1[] = {0xAA, 0x08, 0x3D, 0xF4, 0x03, 0x6F, 0x82, 0x04, 0x00, 0x53, 0x5E, 0x73, 0xB7};

volatile float u_t, qp, q, dq, endstopPos, endstopvel;
static uint32_t oldtime;
float offset;
// 3.19521  1.00249
float L1 = 3.12969; float L2 = 1.00244;
union u_tag {
  byte b[4];
  float fval;
} u;

void setup()
{ Serial.begin(115200);
  Serial1.begin(115200);
  Serial1.setTimeout(1);
  while (!Serial);
  //input torque = 0
 
  Serial1.write(reqPos2, sizeof(reqPos2));
  //we are only interested in the position value, hence ignore the rest of the message
  /*TODO: check if the package is valid and not corrupted*/
  while (Serial1.available() > 0) {
    byte c = Serial1.read();
    Serial.println(c,HEX);
    if (count > 4 && count < 9) {
      u.b[count - 5] = c;
    }
    count++;
  }
  offset = u.fval;
  endstopPos = u.fval;
  q = get_angle(offset, endstopPos);
  oldtime = millis();
}

void loop()
{ if ( (millis() - oldtime) > 1000) {
    oldtime = millis();
    //odrive_serial << "r axis0.encoder.pos_estimate\n";
    Serial1.write(reqPos2, sizeof(reqPos2));
    while (Serial1.available() > 0) {
      byte c = Serial1.read();
      Serial.println(c,HEX);
      if (count > 4 && count < 9) {
        u.b[count - 5] = c;
      }
      count++;
    }
    //endstopPos = odrive_serial.readString().toFloat();
    endstopPos = u.fval;
    q = get_angle(offset, endstopPos);
    Serial.println("position");
    Serial.println(u.b[0], HEX);
    Serial.println(u.b[1], HEX);
    Serial.println(u.b[2], HEX);
    Serial.println(u.b[3], HEX);
    Serial.println(u.fval);
    //odrive_serial << "r axis0.encoder.vel_estimate\n";

  }
}
