#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/i2c.h"
#include <stdio.h>
#include "libpp.h"

#define DELAY 800

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

int writeTime(int64_t uptime) {
  return 0;
}

int64_t readTime() {
  return 0;
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

  while (true) {
#ifndef PROD
    state = flipLED(state);
#endif
    sleep_ms(DELAY);

    int64_t lifetime = readTime();
    if(dead(lifetime)){
      LOG_DEBUG("dead\n");
    } else {
      uptime = to_ms_since_boot(get_absolute_time()) / 1000;
      LOG_DEBUG("up-time: %d sec\n", uptime);
      writeTime(uptime);
    }


  }
}

