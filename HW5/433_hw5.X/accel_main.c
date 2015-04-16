/*
 * File:   display_main.c
 * Author: tgdiriba
 *
 * Created on April 13, 2015, 10:47 PM
 */

#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro
#include "i2c_display.h"
#include "accel.h"

// DEVCFGs here
// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // not boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 40MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
#pragma config UPLLIDIV = DIV_1 // divide clock after FPLLMUL
#pragma config UPLLEN = ON // USB clock on
#pragma config FPLLODIV = DIV_2 // divide clock by 1 to output on pin

// DEVCFG3
#pragma config USERID = 0 // some 16bit userid
#pragma config PMDL1WAY = ON // not multiple reconfiguration, check this
#pragma config IOL1WAY = ON // not multimple reconfiguration, check this
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // controlled by USB module

/*
 *
 */
int main() {
  // startup
  __builtin_disable_interrupts();
  // set the CP0 CONFIG register to indicate that
  // kseg0 is cacheable (0x3) or uncacheable (0x2)
  // see Chapter 2 "CPU for Devices with M4K Core"
  // of the PIC32 reference manual
  __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
  // no cache on this chip!
  // 0 data RAM access wait states
  BMXCONbits.BMXWSDRM = 0x0;
  // enable multi vector interrupts
  INTCONbits.MVEC = 0x1;
  // disable JTAG to be able to use TDI, TDO, TCK, TMS as digital
  DDPCONbits.JTAGEN = 0;
  __builtin_enable_interrupts();

  display_init();
  acc_setup();

  short accels[3]; // accelerations for the 3 axes
  short mags[3];   // magnetometer readings for the 3 axes
  short temp;
  char buffer[100];
  int pixels_x;
  int pixels_y;

  while(1) {
      acc_read_register(OUT_X_L_A, (unsigned char *) accels, 6);
      acc_read_register(OUT_X_L_M, (unsigned char *) mags, 6);
      acc_read_register(TEMP_OUT_L, (unsigned char *) &temp, 2);

      display_clear();

      pixels_x = (int)(((float)((WIDTH-1)*((int)accels[0])))/((float)(1<<15)));
      pixels_y = (int)(((float)((HEIGHT-1)*((int)accels[1])))/((float)(1<<15)));

      sprintf(buffer, "X:%d", accels[0]);
      display_write_text(0, 0, buffer);
      sprintf(buffer, "Y:%d", accels[1]);
      display_write_text(8, 0, buffer);
      sprintf(buffer, "Z:%d", accels[2]);
      display_write_text(16, 0, buffer);

      display_draw_vbar(HEIGHT/2, WIDTH/2, signum(accels[0]), pixels_y, 6);
      display_draw_hbar(HEIGHT/2, WIDTH/2, signum(accels[1]), pixels_x, 6);

      display_draw();
  }

}
