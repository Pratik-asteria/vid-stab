// C library headers
#include <stdio.h>
#include <string.h>
#include <stdint.h>
//#include <date.h>
//#include <cstdlib>
#include <chrono>
#include <iostream>
#include <thread>
// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <math.h>

using namespace std;
using namespace std::chrono;
//using namespace date;

#define ANGLE_BUFFER 20

int ROLL_IMU_ANGLE; //global declaration of angles
int PITCH_IMU_ANGLE;
int YAW_IMU_ANGLE;

struct angles {
  	int time_index;
  	float actual_roll_angle[ANGLE_BUFFER];
  	float actual_pitch_angle[ANGLE_BUFFER];
  	float actual_yaw_angle[ANGLE_BUFFER];
  };


int get_angles(uint8_t *buffer){
  //Total bytes: 59, 
  //5+6 : ROLL_IMU_ANGLE (16 bits) ---> units: 0.02197265625degree
  //7+8 : ROLL_RC_ANGLE (16 bits)
  //9+10+11+12 : ROLL_STATOR_REL_ANGLE (32 bits) camera actual euler angle
  //
  //23+24 : PITCH_IMU_ANGLE (16 bits)  ---> units: 0.02197265625degree
  //25+26 : PITCH_RC_TARGET_ANGLE (16 bits)
  //27+28+29+30 : PITCH_STATOR_REL_ANGLE (32 bits)
  //
  //41+42 : YAW_IMU_ANGLE (16 bits)  ---> units: 0.02197265625degree
  //43+44 : YAW_RC_TARGET_ANGLE (16 bits)
  //45+46+47+48 : YAW_STATOR_REL_ANGLE (32 bits)

  int ROLL_IMU_ANGLE= (int)(((uint16_t)buffer[4] << 8) + buffer[5]);
  int16_t ROLL_RC_ANGLE= ((int16_t)buffer[6] << 8) + buffer[7];
  int32_t ROLL_STATOR_REL_ANGLE= (buffer[8] << 24) + (buffer[9] << 16) + (buffer[10] << 8) + buffer[11];

  int PITCH_IMU_ANGLE= (int)(((uint16_t)buffer[22] << 8) + buffer[23]);
  int16_t PITCH_RC_ANGLE= (buffer[25] << 8) + buffer[26];
  int32_t PITCH_STATOR_REL_ANGLE= (buffer[27] << 24) + (buffer[28] << 16) + (buffer[29] << 8) + buffer[30];

  int YAW_IMU_ANGLE= (int)(((uint16_t)buffer[40] << 8) + buffer[41]);
  int16_t YAW_RC_ANGLE= (buffer[43] << 8) + buffer[44];
  int32_t YAW_STATOR_REL_ANGLE= (buffer[45] << 24) + (buffer[46] << 16) + (buffer[47] << 8) + buffer[48];

  printf("%d \n", ROLL_IMU_ANGLE);
  float actual_roll_angle= fmod((ROLL_IMU_ANGLE*(0.02197265625)) , 360.0);
  printf("%d \n", PITCH_IMU_ANGLE);
  float actual_pitch_angle= fmod((PITCH_IMU_ANGLE*(0.02197265625)) , 360.0);
  printf("%d \n", YAW_IMU_ANGLE);
  float actual_yaw_angle= fmod((YAW_IMU_ANGLE*(0.02197265625)) , 360.0);

  printf("%f\n", actual_roll_angle);
  printf("%f\n", actual_pitch_angle);
  printf("%f\n", actual_yaw_angle);

  return 0;
}


int main() {
  // Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device)
  int serial_port = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);

  // Create new termios struc, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B115200);
  cfsetospeed(&tty, B115200);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Write to serial port
  //uint8_t msg[20] = {0xFF, 0x01, 0x0F, 0x10, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x16};
  //uint8_t msg[6] = {0x3e, 0x45, 0x01, 0x46, 0x12, 0x12};
  int8_t msg[5]= {0x3e, 0x3d, 0x00, 0x3d, 0x00};

  write(serial_port, msg, sizeof(msg));

  // Allocate memory for read buffer, set size according to your needs
  uint8_t read_buf[59];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // ASCII data for this example, we'll set everything to 0 so we can
  // call printf() easily.
  memset(&read_buf, '\0', sizeof(read_buf));

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME

  int num_bytes=0;
  while(num_bytes<59){
  	num_bytes += read(serial_port, &read_buf[num_bytes], sizeof(read_buf));
  	printf("%d\n", num_bytes);
  }

  
  //int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));


  // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
  if (num_bytes < 0) {
      //printf("Error reading: %s", strerror(errno));
      cout << "Error reading: " << strerror(errno) << endl;
      return 1;
  }

  // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
  // print it to the screen like this!)
  printf("Read %i bytes. Received message: ", num_bytes);

  for(int i=0; i< num_bytes; i++){
  	printf("%x ", read_buf[i]);
  } //print the 59 byte feedback data

  printf("\n");

  // while(true){
  // 	get_angles(read_buf);
  // }

  auto next = steady_clock::now();
  auto prev = next - 300ms;
    while (true)
    {
        get_angles(read_buf);  // do stuff
        auto now = steady_clock::now();
        //std::cout << round<milliseconds>(now - prev) << '\n';
        prev = now;

        // delay until time to iterate again
        next += 200ms;
        std::this_thread::sleep_until(next);
    }
  
  close(serial_port);
  printf("\n");
  return 0; // success
}
