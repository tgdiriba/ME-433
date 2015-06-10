#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hidapi.h"
#include <time.h>
#include <sys/time.h>
#include <iostream>

#define MAX_STR 255
#define MAX_SAMPLES 1000

using namespace std;

long int millis()
{
  // Taken from stack overflow
  static struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

int main(int argc, char* argv[])
{
  int res;
  unsigned char buf[65];
  char c_buf[100];
  int data_buffer[3][MAX_SAMPLES];
  time_t prev_timer, curr_timer;
  long int prev_millis, curr_millis, period_millis = 10; 
  hid_device *handle;
  char message[26];
  int row = 0;

  // Initialize the hidapi library
  res = hid_init();
  
  // Open the device using the VID, PID,
  // and optionally the Serial number.
  handle = hid_open(0x4d8, 0x3f, NULL);
  
  memset(buf, 0, 65);
  printf("Enter the message that you would like to display (no spaces!): \n");
  scanf("%25s", message);
  printf("Input the row number to display the message: \n");
  scanf("%d", &row);
  printf("Sending %s on row %d\n", message, row);
  
  // Send out the initial data
  buf[0] = 0;
  buf[1] = 1;
  buf[2] = (unsigned char) row;
  for(int i = 0; i < 26; i++) buf[i+3] = (unsigned char) message[i];
  
  // send out the data
  res = hid_write(handle, buf, 65);
  
  // Send out data in order to recieve a response
  prev_millis = millis();
  int i = 0;
  int command, accel_x, accel_y, accel_z;
  for(int i = 0; i < MAX_SAMPLES; i++) {
    // Send command to PIC
    memset(buf, 0, 65);
    buf[0] = 2;

    while((curr_millis = millis()) < (prev_millis + period_millis));
    
    // Send out packet to prompt the PIC
    res = hid_write(handle, buf, 65);
    res = hid_read(handle, buf, 65);
    
    for(int j = 0; j < 65; j++) c_buf[j] = (char)buf[j];

    printf("Time: %ld ms\n\n", curr_millis);
    sscanf(c_buf, "%d %d %d", &accel_x, &accel_y, &accel_z);
    data_buffer[0][i] = accel_x;
    data_buffer[1][i] = accel_y;
    data_buffer[2][i] = accel_z;
    printf("%d %d %d\n\n", accel_x, accel_y, accel_z);
    
    for(int j = 0; j < 65; j++)
      printf("%d:%d ", buf[j], c_buf[j]);
    printf("\n\n************************************\n\n");
    
    prev_millis += period_millis;
  }
  
  // Finalize the hidapi library
  res = hid_exit();

  printf("Writing data buffer to file.\n\n");

  FILE* data_output = fopen("accelerations.dat", "w+");
  for(int i = 0; i < MAX_SAMPLES; i++)
    fprintf(data_output, "%d %d %d\n", data_buffer[0][i], data_buffer[1][i], data_buffer[2][i]);
  fclose(data_output);
  
  return 0;
}
