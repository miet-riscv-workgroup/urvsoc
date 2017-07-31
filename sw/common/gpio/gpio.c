#include "gpio.h"
#include <stdbool.h>

void gpio_set(int pin, bool value) {
  if (value) *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<pin);
  else       *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<pin);
}
