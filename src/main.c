#include "pico/stdlib.h"
#include <stdio.h>

int main() {
    stdio_init_all();
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    int i = 0;
    
    while (true) {
        gpio_put(25, 1);
        sleep_ms(500);
        gpio_put(25, 0);
	printf("%d\n", i);
	i = i + 1;
        sleep_ms(500);
    }
}

