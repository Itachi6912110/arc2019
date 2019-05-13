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

#include "embARC.h"
#include "embARC_debug.h"
#include "sccb.h"
#include "ov7670.h"

#define PIN_VSYNC 0
#define PIN_PCLK 1
#define PIN_DATA 0xff

DEV_GPIO_PTR clk_ptr = NULL;
DEV_GPIO_PTR data_ptr = NULL;

void captureImg(uint16_t width, uint16_t height);

int main(void) {
    // config SCCB
    sccbInit(0);
    // EMBARC_PRINTF("SSCCB init done\n");

    // config PWM
    io_arduino_config(ARDUINO_PIN_3, ARDUINO_PWM, IO_PINMUX_ENABLE);//pwm timer ch0
	DEV_PWM_TIMER_CFG ch0_pwm_cfg;
	ch0_pwm_cfg.mode = DEV_PWM_TIMER_MODE_PWM;
	ch0_pwm_cfg.count_high = 3;
	ch0_pwm_cfg.count_low = 3;
	ch0_pwm_cfg.isr_hander = NULL;
	DEV_PWM_TIMER_PTR pwm_timer_test = pwm_timer_get_dev(DW_PWM_TIMER_0_ID);
	pwm_timer_test->pwm_timer_open();
	pwm_timer_test->pwm_timer_control(0, PWM_TIMER_CMD_SET_CFG, (void *)(&ch0_pwm_cfg));
	// EMBARC_PRINTF("PWM init done\n");
    board_delay_ms(3000, 1);

    // other pins
    io_arduino_config(ARDUINO_PIN_0, ARDUINO_GPIO, IO_PINMUX_ENABLE); // vsync
    clk_ptr = gpio_get_dev(DFSS_GPIO_4B2_ID);
	if (clk_ptr->gpio_open((1 << PIN_VSYNC)) == E_OPNED) {
		clk_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT,
								(void *)(1 << PIN_VSYNC));
	}

    io_arduino_config(ARDUINO_PIN_1, ARDUINO_GPIO, IO_PINMUX_ENABLE); // pclk
	if (clk_ptr->gpio_open((1 << PIN_PCLK)) == E_OPNED) {
		clk_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT,
								(void *)(1 << PIN_PCLK));
	}

    io_arduino_config(ARDUINO_PIN_4, ARDUINO_GPIO, IO_PINMUX_ENABLE);  // d0
    io_arduino_config(ARDUINO_PIN_5, ARDUINO_GPIO, IO_PINMUX_ENABLE);  // d1
    io_arduino_config(ARDUINO_PIN_6, ARDUINO_GPIO, IO_PINMUX_ENABLE);  // d2
    io_arduino_config(ARDUINO_PIN_7, ARDUINO_GPIO, IO_PINMUX_ENABLE);  // d3
    io_arduino_config(ARDUINO_PIN_8, ARDUINO_GPIO, IO_PINMUX_ENABLE);  // d4
    io_arduino_config(ARDUINO_PIN_9, ARDUINO_GPIO, IO_PINMUX_ENABLE);  // d5
    io_arduino_config(ARDUINO_PIN_10, ARDUINO_GPIO, IO_PINMUX_ENABLE); // d6
    io_arduino_config(ARDUINO_PIN_11, ARDUINO_GPIO, IO_PINMUX_ENABLE); // d7
    data_ptr = gpio_get_dev(DFSS_GPIO_8B2_ID);
	if (data_ptr->gpio_open(PIN_DATA) == E_OPNED) {
		data_ptr->gpio_control(GPIO_CMD_SET_BIT_DIR_INPUT,
								(void *)(PIN_DATA));
	}

    camInit();
    setRes(QQVGA);
    setColorSpace(RGB565);

    // EMBARC_PRINTF("Cam init done\n");

    writeReg(0x11, 63);

    while(1) {
        captureImg(160*2, 120);
    }
}

inline bool vsyncLow() {
    uint32_t vsync_sig;
    clk_ptr->gpio_read(&vsync_sig, 1 << PIN_VSYNC);
    return vsync_sig == 0;
}

inline bool vsyncHigh() {
    uint32_t vsync_sig;
    clk_ptr->gpio_read(&vsync_sig, 1 << PIN_VSYNC);
    return vsync_sig != 0;
}

inline bool pclkHigh() {
    uint32_t pclk_sig;
    clk_ptr->gpio_read(&pclk_sig, 1 << PIN_PCLK);
    return pclk_sig != 0;
}

inline bool pclkLow() {
    uint32_t pclk_sig;
    clk_ptr->gpio_read(&pclk_sig, 1 << PIN_PCLK);
    return pclk_sig == 0;
}

void captureImg(uint16_t width, uint16_t height) {
    uint8_t buf[320];
    EMBARC_PRINTF("RDY");
    while(vsyncLow());
    while(vsyncHigh());

    while(height--) {
        // EMBARC_PRINTF("I");
        uint8_t* readPtr = buf;
        uint8_t* writePtr = buf;

        int readTime = 64;
        int writeTime = 320 - 64;
        // EMBARC_PRINTF("%d", readTime);
        while(readTime--) {
            while(pclkHigh());
            data_ptr->gpio_read((uint32_t *)readPtr++, PIN_DATA);
            while(pclkLow());
            while(pclkHigh());
            data_ptr->gpio_read((uint32_t *)readPtr++, PIN_DATA);
            while(pclkLow());
            while(pclkHigh());
            data_ptr->gpio_read((uint32_t *)readPtr++, PIN_DATA);
            while(pclkLow());
            while(pclkHigh());
            data_ptr->gpio_read((uint32_t *)readPtr++, PIN_DATA);
            while(pclkLow());
            while(pclkHigh());
            data_ptr->gpio_read((uint32_t *)readPtr++, PIN_DATA);
            EMBARC_PRINTF("%c", *writePtr++);
            while(pclkLow());
        }
        while(writeTime--) {
            EMBARC_PRINTF("%c", *writePtr++);
        }
    }
    // EMBARC_PRINTF("DONE");
}




