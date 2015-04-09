#ifndef I2C_H
#define I2C_H

typedef struct {
  int baud_rate;
  int destination_addr;
  int source_addr;
  int state;
} i2ccon_t;

void init_i2c(i2ccon_t* config);
void write_i2c(char output);
char read_i2c();

#endif // I2C_H
