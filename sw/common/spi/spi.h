
#ifndef __SPI_H
#define __SPI_H

#include <inttypes.h>

#if defined( __GNUC__)
#define PACKED __attribute__ ((packed))
#else
#error "Unsupported compiler?"
#endif

#ifndef __WBGEN2_MACROS_DEFINED__
#define __WBGEN2_MACROS_DEFINED__
#define WBGEN2_GEN_MASK(offset, size) (((1<<(size))-1) << (offset))
#define WBGEN2_GEN_WRITE(value, offset, size) (((value) & ((1<<(size))-1)) << (offset))
#define WBGEN2_GEN_READ(reg, offset, size) (((reg) >> (offset)) & ((1<<(size))-1))
#define WBGEN2_SIGN_EXTEND(value, bits) (((value) & (1<<bits) ? ~((1<<(bits))-1): 0 ) | (value))
#endif

/* definitions for register: Status Register */
#define SPI_CS_GEN_AUTO                       WBGEN2_GEN_MASK(13, 1)
#define SPI_CS_GEN_PROGRAM                    0x00000000
#define SPI_IE_ENABLE_ON                      WBGEN2_GEN_MASK(12, 1)
#define SPI_IE_ENABLE_OFF                     0x00000000
#define SPI_LSB_ORDER_FIRST                   WBGEN2_GEN_MASK(11, 1)
#define SPI_LSB_ORDER_LAST                    0x00000000
#define SPI_TX_EDGE_NEGEDGE                   WBGEN2_GEN_MASK(10, 1)
#define SPI_TX_EDGE_POSEDGE                   0x00000000
#define SPI_RX_EDGE_NEGEDGE                   WBGEN2_GEN_MASK( 9, 1)
#define SPI_RX_EDGE_POSEDGE                   0x00000000
#define SPI_TRANSFER_LEN_MASK                 WBGEN2_GEN_MASK(0, 7)
#define SPI_CRTL_BUSY                         WBGEN2_GEN_MASK(8, 1)
#define SPI_START                             WBGEN2_GEN_MASK(8, 1)

/* [0x0]: REG Transmit data regsiter */
#define SPI_REG_TX      0x00000000
/* [0x10]: REG Control and status register */
#define SPI_REG_CTRL    0x00000010
/* [0x14]: REG Clock devider register */
#define SPI_REG_DIVIDER 0x00000014
/* [0x18]: REG Slave select regsiter */
#define SPI_REG_SS      0x00000018

PACKED struct SPI_WB {
  /* [0x0]: REG Transmit data regsiter */
  uint32_t TX_0;
  /* [0x4]: REG Transmit data regsiter */
  uint32_t TX_1;
  /* [0x8]: REG Transmit data regsiter */
  uint32_t TX_2;
  /* [0xc]: REG Transmit data regsiter */
  uint32_t TX_3;
  /* [0x10]: REG Control and status register */
  uint32_t CTRL;
  /* [0x14]: REG Clock devider register */
  uint32_t DIVIDER;
  /* [0x18]: REG CS signal control */
  uint32_t SS;
};

typedef struct {
  /* CS autogeneration  */
  uint16_t  CS_GEN;
  /* Interrupt enable */
  uint16_t  IE_ENABLE;
  /* Bit order */
  uint16_t  LSB_ORDER;
  /* Edge Tx data */
  uint16_t  TX_EDGE;
  /* Edge RX data */
  uint16_t  RX_EDGE;
  /* Baudrate */
  uint32_t  BAUDRATE;
  /* Number bits in package */
  uint16_t  TRANSFER_LEN;
}SPI_CONFIG_WB_Type;

/* function declaration*/
void spi_init(SPI_CONFIG_WB_Type *spi_config);
uint32_t spi_transaction(uint32_t data, uint32_t slave_num);
void spi_send(uint32_t *data_send, uint32_t len, uint32_t slave_num);
void spi_exchange(uint32_t *data_send, uint32_t *data_receive, uint32_t len, uint32_t slave_num);
void spi_read(uint32_t *data_receive, uint32_t len, uint32_t slave_num);

#endif
