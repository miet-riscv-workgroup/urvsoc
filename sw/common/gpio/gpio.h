#include "../board.h"
#include <stdbool.h>

#define GPIO_CODR 0x0
#define GPIO_SODR 0x4

void gpio_set(int pin, bool value);

