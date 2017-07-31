#include <stdint.h>
#include "spi.h"
#include "../board.h"

volatile struct SPI_WB *spi;

uint32_t calc_baud(uint32_t baudrate) {
  return ((BASE_CLOCK / (baudrate * 2)) - 1);
}

void spi_init(SPI_CONFIG_WB_Type *spi_config)
{
  spi = (volatile struct SPI_WB *)BASE_SPI;

  spi->CTRL = spi_config->CS_GEN | spi_config->IE_ENABLE | spi_config->LSB_ORDER \
    | spi_config->TX_EDGE | spi_config->RX_EDGE | (spi_config->TRANSFER_LEN & SPI_TRANSFER_LEN_MASK);
    
  spi->DIVIDER = calc_baud(spi_config->BAUDRATE);
}

uint32_t spi_transaction(uint32_t data, uint32_t slave_num)
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

void spi_send(uint32_t *data_send, uint32_t len, uint32_t slave_num)
{
  while(len > 0){
    spi_transaction(*data_send, slave_num);
    data_send++;
    len--;
  }
}

void spi_exchange(uint32_t *data_send, uint32_t *data_receive, uint32_t len, uint32_t slave_num)
{
  while(len > 0){
    *data_receive = spi_transaction(*data_send, slave_num);
    data_receive++;
    data_send++;
    len--;
  }
}

void spi_read(uint32_t *data_receive, uint32_t len, uint32_t slave_num)
{
  while(len > 0){
    *data_receive = spi_transaction(0, slave_num);
    data_receive++;
    len--;
  }
}


