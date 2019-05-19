#include "embARC.h"

#define DEBUG
#include "embARC_debug.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

//#if defined(EMBARC_USE_MCUBOOT)
//#include "bootutil/bootutil.h"
//#include "bootutil/image.h"
//#include "flash_map/flash_map.h"
//#endif

#define NUM_BYTE_READ 4096

#define BUFF_SPACE 8192

#define BOOT_CFG_FILE_NAME	"boot.json"
#define BOOT_FILE_NAME		"0:\\boot.bin"


FIL fp;
//int8_t file_buff[ BUFF_SPACE ];
char file_buff[ BUFF_SPACE ];
int32_t error_num ;

/**
 * \brief       Read out File Form SD Card to DDR2 15MB buff
 *
 * \param[in]   music_name             Pointer of File Name in SD Card Need to Read Out
 *
 */
void readout_file(void)
{
	char filename[50] = "0:\\boot.json";			//the Mp3 File Should in "root"/music/
	uint32_t num_read;
	uint8_t *fbuff_p = file_buff;
	uint32_t read_sum = 0;

	error_num = f_open(&fp, filename, FA_READ);

	/* Sometimes May Open File Fail,Stop Program */
	if ( error_num != FR_OK) {
		DBG("File %s open failed!\r\nContinue!\r\n", filename);

		while (1);
	}

	DBG("Start To Read file %s !!!\r\n", filename);
	memset( file_buff, 0, sizeof(int8_t) * BUFF_SPACE );
	num_read = 1;

	while ( num_read != 0 ) {
		cpu_lock();								//No Interruption Should Happen Here
		error_num = f_read(&fp, fbuff_p, NUM_BYTE_READ, &num_read);
		cpu_unlock();

		fbuff_p += num_read;
		read_sum += num_read;
	}

	DBG("readout %d!!!\r\n", read_sum);
	DBG("File %s Read End\r\n", filename);
	DBG("error number: %d\r\n", error_num);

	f_close(&fp);
}

int main(void){
	EMBARC_PRINTF("Start Program! \n");
	board_init();
	cpu_unlock();

	readout_file();
	EMBARC_PRINTF("Finish reading! \n");
	EMBARC_PRINTF("read in buff: %s \n", file_buff);
}