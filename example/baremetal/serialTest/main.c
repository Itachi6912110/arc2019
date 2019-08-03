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

#define BOARD_CONSOLE_UART_BAUD 1000000 //(iotdk.h)
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "embARC.h"
#include "embARC_debug.h"

void waitStart();


int main(void)
{

	//set_baud(uart_context0, BOARD_CONSOLE_UART_BAUD);

	int32_t i, j;
	uint8_t pixel;
	unsigned char* p = (unsigned char*)&pixel;

	size_t size = 65536;
	waitStart();
	for(j = 0; j < size; j = j + 1)
	{
		pixel = j % 256;
		//putchar(p[0]);
		//fflush(stdout);
		xprintf("%c", pixel);
	}

	waitStart();
	for(j = size-1; j >= 0; j = j - 1)
	{
		pixel = j % 256;
		xprintf("%c", pixel);
	}

	waitStart();
	for(j = 0; j < size; j = j + 1)
	{
		pixel = j % 256;
		xprintf("%c", pixel);
	}
	//printf("END!!!\n");
}

void waitStart()
{
	while(1)
	{
		char *start = malloc(sizeof(char));
		*start = getchar();
		//EMBARC_PRINTF("Start: %d\n", *start);
		if(start != NULL) break;
		//scanf("abc%d", &debug);
		free(start);
	}
	//fflush(stdin);
}