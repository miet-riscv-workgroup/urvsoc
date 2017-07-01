/* boot.c - a trivial serial port bootloader for LM32.

   Public domain.

   Awful code below. Be warned. */

#include <stdint.h>

#include "../common/board.h"
#include "../common/uart/uart.h"

#define USER_START 0x800

#define GPIO_CODR 0x0
#define GPIO_SODR 0x4

#define BASE_UART 0x80000000
#define BASE_GPIO 0x80001000

const char hexchars[] = "0123456789abcdef";

void dump_int(uint32_t v)
{
    int i;
    for(i=7;i>=0;i--)
    {
	uart_write_byte(hexchars[(v>>(i*4)) & 0xf]);
    }
uart_write_byte('\n');
uart_write_byte('\r');
}

void delay(int v)
{
    volatile int i;

    for(i=0;i<v;i++);
}

main()
{
	int i;
	uint32_t dword;
	
	int len, boot_active = 0;
	uint32_t *ptr;
	uint32_t size;
	
	uart_init_hw();
	len = 0;
	boot_active = 0;

	uart_write_byte('B');
	uart_write_byte('o');
	uart_write_byte('o');
	uart_write_byte('t');
	uart_write_byte('?');
	
	//for(i=0;i<500000;i++)
	while(1)
		if(uart_read_byte () == 'Y')
		{
			boot_active = 1;
			break;
		}
	if(boot_active)
	{
		uart_write_byte('O');
		uart_write_byte('K');

		len = (uint32_t)uart_read_byte();
		len <<=8;
		len |= (uint32_t)uart_read_byte();
		len <<=8;
		len |= (uint32_t)uart_read_byte();		
		len <<=8;
		len |= (uint32_t)uart_read_byte();
		
		size = len;
		// read code
		for (ptr= (uint32_t *) USER_START; ptr < (uint32_t *) (USER_START+size); ptr=ptr+1) {
			dword  = (uint32_t)uart_read_byte();
			dword |= (uint32_t)(uart_read_byte() << 8);
			dword |= (uint32_t)(uart_read_byte() << 16);
			dword |= (uint32_t)(uart_read_byte() << 24);
			*ptr   = dword;
		}

		uart_write_byte('G');
		uart_write_byte('o');
		uart_write_byte('!');

		((int(*)(void))(USER_START))();
		
	} 
	/*else {
		uart_write_byte('T');
		uart_write_byte('o');
		uart_write_byte('u');
		uart_write_byte('t');


    	//	void (*f)() = USER_START;	
		//f();
		((int(*)(void))(USER_START))();
//		goto again;
	}*/

	
}
