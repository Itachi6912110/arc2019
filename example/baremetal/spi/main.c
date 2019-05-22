/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

#include "stdio.h"
#include "embARC.h"
#include "embARC_debug.h"
#include "spi_flash_w25qxx.h"
//#include "spi_flash.h"
//#include "spi_flash_w25qxx.h"

#define BOARD_SFLASH_SPI_ID 0
#define BOARD_SFLASH_SPI_LIN 0

int main(void)
{
	printf("START!!\n");
	W25QXX_DEF(spi_flash, BOARD_SFLASH_SPI_ID, BOARD_SFLASH_SPI_LIN, FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE);
	while(1)
	{
		int32_t status = w25qxx_init(spi_flash, BOARD_SPI_FREQ);
		if(status == E_OK || status == E_OPNED)
		{
			EMBARC_PRINTF("status: %d\n",status);
			break;
		}
	}
	EMBARC_PRINTF("Initializaion Complete...\n");
	EMBARC_PRINTF("0x%08x\n",(void*)spi_flash);
	EMBARC_PRINTF("SPI FLASH ID: 0x%x\r\n", w25qxx_read_id(spi_flash));
	
	uint8_t test_buffer_rd[16];
	w25qxx_erase(spi_flash, 0, 16);
	w25qxx_read(spi_flash, 0, 16, test_buffer_rd);
	uint32_t i;
	for (i = 0; i < 16; i++) {
		//EMBARC_PRINTF("0x%08x\n", test_buffer_rd[i]);
		if (test_buffer_rd[i] != 0xFF) {
			EMBARC_PRINTF("SPI Erase failed:%d\n", i);
		}
	}
	EMBARC_PRINTF("Flash Erase Complete...\n");

	//while(w25qxx_wait_ready(spi_flash)) {};
	/*
	EMBARC_PRINTF("Check address 0...\n");
	uint8_t data[1];
	w25qxx_read(spi_flash, 7, 1, data);
	EMBARC_PRINTF("Read Address 0: 0x%08x\n", data[0]);
	*/
	uint32_t address = 0x0000;
	uint8_t size = 16;
	uint8_t data[16];
	for(i = 0; i < 16; i++) data[i] = i;
	w25qxx_write(spi_flash, address, size, data);
	//EMBARC_PRINTF("Write Data 0x%08x Complete...\n",*data);
	while(w25qxx_wait_ready(spi_flash)) {};
	EMBARC_PRINTF("Write Data Success...\n");
	uint8_t rx[16];
	w25qxx_read(spi_flash, address, size, rx);

	for(i = 0; i < 16; i++) EMBARC_PRINTF("%d\n",rx[i]);
	EMBARC_PRINTF("Complete\n");
}