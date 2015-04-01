/* 
 * File:   demo.c
 * Author: tgdiriba
 *
 * Created on April 1, 2015, 3:33 PM
 */

#include<xc.h> // processor SFR definitions
#include<sys/attribs.h> // __ISR macro

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

void wait_ms(volatile int);
int readADC(void);

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

  ANSELBbits.ANSB13 = 0;
  ANSELBbits.ANSB15 = 0;

  // set up USER pin as input
  TRISBbits.TRISB13 = 1;

  // set up LED1 pin as a digital output
  RPB15Rbits.RPB15R = 0b0101;

  TRISBbits.TRISB7 = 0;
  LATBbits.LATB7 = 1;

  // set up LED2 as OC1 using Timer2 at 1kHz
  T2CONbits.TCKPS = 0;
  PR2 = 39999;
  TMR2 = 0;

  OC1CONbits.OCM = 0b110;
  OC1RS = 0;
  OC1R = 0;

  T2CONbits.ON = 1;
  OC1CONbits.ON = 1;

  // set up A0 as AN0
  ANSELAbits.ANSA0 = 1;
  AD1CON3bits.ADCS = 3;
  AD1CHSbits.CH0SA = 0;
  AD1CON1bits.ADON = 1;

  int adc_val = 0;
  int oc_val = 0;
  while (1) {
    // invert pin every 0.5s, set PWM duty cycle % to the pot voltage output %
      OC1RS = (int)((float)(readADC()*PR2)/1023.0);

      if(PORTBbits.RB13 == 0) {
          LATBbits.LATB7 = !LATBbits.LATB7;
      }
      else {
          _CP0_SET_COUNT(0);
          while(_CP0_GET_COUNT() < 20000000) OC1RS = (int)((float)(readADC()*PR2)/1023.0);
          LATBbits.LATB7 = !LATBbits.LATB7;
      }
  }
}

void wait_ms(volatile int ms) {
    for(ms *= 40000000; ms > 0; ms--);
}

int readADC(void) {
  int elapsed = 0;
  int finishtime = 0;
  int sampletime = 20;
  int a = 0;

  AD1CON1bits.SAMP = 1;
  elapsed = _CP0_GET_COUNT();
  finishtime = elapsed + sampletime;
  while (_CP0_GET_COUNT() < finishtime) {
  }
  AD1CON1bits.SAMP = 0;
  while (!AD1CON1bits.DONE) {
  }
  a = ADC1BUF0;
  return a;
}
