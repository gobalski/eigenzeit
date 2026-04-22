#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include <string.h>
#include "libpp.h"

static const int DELAY = 800;

static i2c_inst_t *I2C = i2c0;
static const uint8_t FRAM_ADDR = 0x50;
static const char UPTIME_ADDR[2] = {0x0, 0x0};


#ifndef PROD
char flipLED(char state) {
  if(state==1){
    gpio_put(25,1);
    return 0;
  }
  else {
    gpio_put(25,0);
    return 1;
  }
}
#endif

int init_i2c_bus() {
    const uint sda_pin = 16;
    const uint scl_pin = 17;

    //Initialize I2C port at 400 kHz
    i2c_init(I2C, 400 * 1000);

    // Initialize I2C pins
    gpio_set_function(sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(scl_pin, GPIO_FUNC_I2C);

    // Check if Device sends ACK after sending address
}

int writeTime(int64_t uptime) {
  uint8_t* uptime_data = (uint8_t*) &uptime;
  uint8_t data[10];
  for (int i=0; i<2; i++){
    data[i] = UPTIME_ADDR[i];
  }
  for (int i=2; i<10; i++){
    data[i] = uptime_data[i-2];
  }
  int bytes_written;
   
  bytes_written = i2c_write_blocking(I2C, FRAM_ADDR, data, 10, false);

  LOG_DEBUG("Wrote %d bytes to i2c address: %x\n", bytes_written, FRAM_ADDR);
  LOG_DEBUG("Wrote %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x \n", \
      data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9]);
  return 0;
}

int64_t readTime() {
  int bytes_read;
  uint8_t data[8];

  i2c_write_blocking(I2C, FRAM_ADDR, UPTIME_ADDR, 2, true);
  bytes_read = i2c_read_blocking(I2C, FRAM_ADDR, data, 8, false);
  LOG_DEBUG("read %d from address: %x\n", bytes_read, FRAM_ADDR);
  LOG_DEBUG("Read %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x \n", \
      data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);

  int64_t uptime;
  for (int i=0; i<8; i++) uptime = (uptime << 8) | data[i-7];
  return uptime;
}

int updateLCD(int64_t uptime) {
  return 0;
}

bool dead(int64_t lifetime) {
  if (lifetime > 0){
    return 1;
  }
  return 0;
}

int main() {
#ifndef PROD
  stdio_init_all();
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  char state = 0;
#endif
  int64_t uptime;

  init_i2c_bus();

  sleep_ms(5000);

  while (true) {
#ifndef PROD
    state = flipLED(state);
#endif
    sleep_ms(DELAY);

    int64_t lifetime = readTime();
    //lifetime = 0;
    LOG_DEBUG("lifetime: %lld sec\n", lifetime);
    if(dead(lifetime)){
      LOG_DEBUG("dead\n");
    } else {
      uptime = to_ms_since_boot(get_absolute_time()) / 1000;
      LOG_DEBUG("up-time: %d sec\n", uptime);
      writeTime(uptime);
    }


  }
}

