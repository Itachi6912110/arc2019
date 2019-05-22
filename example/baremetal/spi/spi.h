#ifndef SPI_H
#define SPI_H
#include "embARC.h"
#include "embARC_debug.h"
#include "embARC_error.h"

static DEV_SPI_PTR spi_flash = NULL;

#define RDID	0x9F	/*!<read chip ID */
#define RDSR	0x05	/*!< read status register */
#define WRSR	0x01	/*!< write status register */
#define WREN	0x06	/*!< write enablewaitDeviceReady */
#define WRDI	0x04	/*!< write disable */
#define READ	0x03	/*!< read data bytes */
#define SE		0x20	/*!< sector erase */
#define PP	    0x02	/*!< page program */

int spiInit(uint8_t devId);

bool spiWrite(uint8_t regAddr, uint8_t data);

uint8_t spiRead(uint8_t regAddr);

#endif