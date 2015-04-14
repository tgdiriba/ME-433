#ifndef I2C_MASTER_INT__H__
#define I2C_MASTER_INT__H__

#define PBCLK 40000000

// buffer pointer type.  The buffer is shared by an ISR and mainline code.
// the pointer to the buffer is also shared by an ISR and mainline code.
// Hence the double volatile qualification
typedef volatile unsigned char * volatile buffer_t;

volatile enum state_t {IDLE, START, WRITE, READ, RESTART, ACK, NACK, STOP, ERROR};

typedef volatile struct {
  unsigned int baud_rate;
  unsigned int destination_addr;
  unsigned int source_addr;
  volatile state_t state;
  int module;
} i2c_t;

void i2c_master_setup(i2c_t* mod); //sets up i2c2 as a master using an interrupt

// initiate an i2c write read operation at the given address. You can optionally only read or only write by passing zero lengths for the reading or writing
// this will not return until the transaction is complete.  returns false on error

int i2c_write_read(i2c_t* mod, const buffer_t write, unsigned int wlen, const buffer_t read, unsigned int rlen );

#endif
