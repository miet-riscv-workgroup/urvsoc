
#include <stdint.h>

#include "wb_spi.h"
#include "../board.h"

#define CALC_BAUD(baudrate)  ((BASE_CLOCK  / (((unsigned int)baudrate) * 2)) - 1)

volatile struct SPI_WB *spi;

void spi_init(SPI_CONFIG_WB_Type *spi_config)
{
  spi = (volatile struct SPI_WB *)BASE_SPI;

  spi->CTRL = spi_config->CS_GEN | spi_config->IE_ENABLE | spi_config->LSB_ORDER \
    | spi_config->TX_EDGE | spi_config->RX_EDGE | (spi_config->TRANSFER_LEN & SPI_TRANSFER_LEN_MASK);
    
	spi->DIVIDER = CALC_BAUD(spi_config->BAUDRATE);
}

uint32_t spi_send(uint32_t data, uint32_t slave_num)
{
	while (spi->CTRL & SPI_CRTL_BUSY)
		;

	spi->SS   = slave_num;
  spi->TX_0  = data;
  spi->CTRL = spi->CTRL | SPI_START;

  while (spi->CTRL & SPI_CRTL_BUSY)
    ;

  return spi->TX_0;
}

void spi_send_burst(uint32_t *data_send, uint32_t len, uint32_t slave_num)
{
  while(len > 0){
    spi_send(*data_send, slave_num);
    data_send++;
    len--;
  }
}

void spi_receive_send_burst(uint32_t *data_send, uint32_t *data_receive, uint32_t len, uint32_t slave_num)
{
  while(len > 0){
    *data_receive = spi_send(*data_send, slave_num);
    data_receive++;
    data_send++;
    len--;
  }
}


