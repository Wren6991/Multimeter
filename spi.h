#ifndef _MM_SPI_H_
#define _MM_SPI_H_

#include <avr/io.h>

enum spi_assert_enum {
    spi_assert_lcd,
    spi_assert_flash
};


void spi_init();
void spi_deassert();
void spi_assert(uint8_t device);
void spi_send_byte(uint8_t x);
void spi_send(uint8_t *message, uint8_t length);

#endif // _MM_SPI_H_
