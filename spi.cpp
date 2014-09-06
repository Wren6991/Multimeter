#include "spi.h"

#define LCD_CS 0x02
#define PORT_LCD_CS PORTA
#define DDR_LCD_CS DDRA
#define FLASH_CS 0x80
#define FLASH_WP 0x40
#define FLASH_HOLD 0x20
#define PORT_FLASH_CS PORTD
#define DDR_FLASH_CS DDRD

#define PORT_SPI PORTB
#define DDR_SPI DDRB
#define SPI_MOSI 0x20
#define SPI_MISO 0x40
#define SPI_SCK 0x80
#define SPI_SS 0x10
#define SPI_ALL (SPI_MOSI | SPI_MISO | SPI_SCK | SPI_SS)

void spi_init()
{
    spi_deassert();
    DDR_SPI = (DDR_SPI & ~SPI_ALL) | (SPI_MOSI | SPI_SCK | SPI_SS);
    SPCR0 = 0x51;    // no interrupt, SPI enabled, MSB first, master, divide clock by four (on top of four phases)
    SPSR0 = 0x01;    // SPI2X enabled (gives effective SPI clock of FCPU/8 = 1MHz)
}

void spi_deassert()
{
    //DDR_LCD_CS &= ~LCD_CS;
    PORT_LCD_CS |= LCD_CS;
    //DDR_FLASH_CS &= ~FLASH_CS;
    PORT_FLASH_CS |= FLASH_CS;
}

void spi_assert(uint8_t device)
{
    switch (device)
    {
    case spi_assert_flash:
        PORT_FLASH_CS = (PORT_FLASH_CS & ~FLASH_CS) | FLASH_HOLD;
        DDR_FLASH_CS |= FLASH_CS | FLASH_HOLD;
        break;
    case spi_assert_lcd:
        PORT_LCD_CS &= ~LCD_CS;
        DDR_LCD_CS |= LCD_CS;
        break;
    }
}

void spi_send_byte(uint8_t x)
{
    SPDR0 = x;
    while (!(SPSR0 & 0x80));
}

void spi_send(uint8_t *message, uint8_t length)
{
    while (length--)
        spi_send_byte(*message++);
}

void spi_receive();
