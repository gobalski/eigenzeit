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

static const uint8_t LCD_ADDR = 0x7C;
static const uint8_t LCD_INIT[] = {
    0x00, // Control byte: CO = 0 (only data follows), RS = 0 (Instruction write operation)
    0x34, // Function set DL=1 (8bit Bus), N=0 (1 Line), DH=1 (double height), IS=0 (normal instructions)
    0x35, // Function set DL=1 (8bit Bus), N=0 (1 Line), DH=1 (double height), IS=1 (extend instructions)
    0x14, // Internal oscillator / frequency related setting
    0x78, // Contrast set / related setting 
    0x5E, // Power/ICON/contrast control related setting 
    0x6D, // Follower control / power stabilization related setting 
    0x0C, // Display ON/OFF control: D=1 (Display ON), C=0 (Cursor OFF), B=0 (Cursor Blink OFF)
    0x01, // Clear display
    0x06  // Entry mode set I/D=1 (address increases after write), S=0 (no display shift)
};

static const uint8_t HELLO_WORLD[] = {
  0x80, // control byte, but another will follow
  0x02, // return home
  0x40, // last control byte, RS=1 for writing to DDRAM
  // NOTE: its ASCII!!
  0x48, // H
  0x45, // E
  0x4C, // L
  0x4C, // L
  0x4F, // O
}

// NOTE: its ASCII!!
static const uint8_t DIGITS[] = {
  0x30, // 0
  0x31, // 1
  0x32, // 2
  0x33, // 3
  0x34, // 4
  0x35, // 5
  0x36, // 6
  0x37, // 7
  0x38, // 8
  0x39  // 9
}

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
  if (bytes_written != 10){
    LOG_DEBUG("Write failed: bytes_written=%d\n", bytes_written);
    return -1;
  } else {
    //LOG_DEBUG("Wrote %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x \n", \
               data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9]);
    //LOG_DEBUG("write success\n");
    return 0;
  }
}

int64_t readTime() {
  int bytes_read;
  uint8_t data[8];

  i2c_write_blocking(I2C, FRAM_ADDR, UPTIME_ADDR, 2, true);
  bytes_read = i2c_read_blocking(I2C, FRAM_ADDR, data, 8, false);
  if (bytes_read < 0){
    LOG_DEBUG("Read failed.\n");
    return -1;
  } else {
    LOG_DEBUG("Read %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x \n", \
               data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
  }
  // prepare output
  int64_t uptime;
  for (int i=0; i<8; i++) uptime = (uptime << 8) | data[i-7];
  return uptime;
}

int updateLCD(int64_t uptime) {
  char uptime_str[16];
  snprintf(buf, sizeof(uptime_str), "%d", uptime);
  // OPTIONAL: check what bytes need to be uptdated
  // prepare data byte sequences
  // send i2c comands
  return 0;
}

int initLCD() {
  return 0;
}

bool dead(int64_t lifetime) {
  if (lifetime > 0){
    return 1;
  }
  return 0;
}

int reset(){
  int ret;
  sleep_ms(4000);
  uptime = 0;
  ret = writeTime(uptime);
  if (ret < 0){
    LOG_DEBUG("reset failed\n");
    return 1;
  }
  LOG_DEBUG("wrote reset to FRAM\n");
  flipLED(1);
  sleep_ms(200);
  flipLED(0);
  sleep_ms(200);
  flipLED(1);
  sleep_ms(200);
  return 0;
}

int main() {
// init dev stuff
#ifndef PROD
  stdio_init_all();
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  char state = 0;

  // reset eigenzeit in DEV
  reset();
#endif

  int64_t uptime;
  init_i2c_bus();

  int64_t lifetime = readTime();
  LOG_DEBUG("lifetime: %lld sec\n", lifetime);

  while (true) {
#ifndef PROD
    state = flipLED(state);
#endif
    sleep_ms(DELAY);

    if(dead(lifetime)){
      LOG_DEBUG("dead\n");
    } else {
      uptime = to_ms_since_boot(get_absolute_time()) / 1000;
      LOG_DEBUG("up-time: %d sec\r\n", uptime);
      writeTime(uptime);
    }

  }
}

