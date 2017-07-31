#include "../common/board.h"
#include "../common/uart/uart.h"
#include "../common/spi/spi.h"
#include "../common/irq/irq_c.h"

#define BASE_CLOCK 100000000 // Xtal frequency

#define GPIO_CODR 0x0
#define GPIO_SODR 0x4

#define BASE_UART 0x80000000
#define BASE_GPIO 0x80001000
#define BASE_WDAC 0x80002000

void gpio_set(int pin, int value)
{
    if(value)
	*(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<pin);
    else
	*(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<pin);
}

void delay(int v)
{
    volatile int i;

    for(i=0;i<v;i++);
}

SPI_CONFIG_WB_Type spi_config;

void irq_0_handler()
{
    
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x400\ncsrc mip, t0"); // clear irq_0
}

void irq_1_handler()
{
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x800\ncsrc mip, t0"); // clear irq_1
}

void irq_2_handler()
{
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x1000\ncsrc mip, t0"); // clear irq_2
}

void irq_3_handler()
{
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x2000\ncsrc mip, t0"); // clear irq_3
}

void irq_4_handler()
{
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x4000\ncsrc mip, t0"); // clear irq_4
}

void irq_5_handler()
{
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x8000\ncsrc mip, t0"); // clear irq_5
}

void irq_6_handler()
{
    
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<3);
    asm volatile ("li t0, 0x10000\ncsrc mip, t0"); // clear irq_6
}

void irq_7_handler()
{
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<0);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<1);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_CODR ) = (1<<2);
    *(volatile unsigned *) ( BASE_GPIO + GPIO_SODR ) = (1<<3);
    asm volatile ("li t0, 0x20000\ncsrc mip, t0"); // clear irq_7
}



main()
{
  //uart_init_hw();
  irq_enable_all();
  irq_set_mask(0x01);
  //disable_irq_0();

  spi_config.CS_GEN       = SPI_CS_GEN_AUTO;
  spi_config.IE_ENABLE    = SPI_IE_ENABLE_OFF;
  spi_config.LSB_ORDER    = SPI_LSB_ORDER_LAST;
  spi_config.TX_EDGE      = SPI_TX_EDGE_NEGEDGE;
  spi_config.RX_EDGE      = SPI_RX_EDGE_POSEDGE;
  spi_config.BAUDRATE     = 1000000;
  spi_config.TRANSFER_LEN = 8;

  spi_init(&spi_config);

  uint32_t *ptr_r, *ptr_s;   
  int a[] = {0x56, 0x43, 0x35, 0x27, 0x18, 0x6, 0x67, 0x48, 0x99};
  int b[] = {0x56, 0x43, 0x35, 0x27, 0x18, 0x6, 0x67, 0x48, 0x99};
  ptr_s = &a;
  ptr_r = &b;
  
  for(;;)
    {

    //low_byte =uart_read_byte();
	  //high_byte =uart_read_byte();
	  //uart_write_byte(low_byte);
	  //uart_write_byte(high_byte);

    //spi_send(0x8C, 0x1);
    //spi_send(0x8B, 0x2);
    //spi_send(0x8D, 0x4);
    //spi_send(0x8E, 0x8);
    //spi_send_burst(ptr_s, 9, 0x1);

	  gpio_set(0, 1);
    gpio_set(1, 1);
    gpio_set(2, 1);
    gpio_set(3, 1);
    delay(100);

    gpio_set(0, 0);
    gpio_set(1, 0);
    gpio_set(2, 0);
    gpio_set(3, 0);
    delay(100);
    
    }
}
