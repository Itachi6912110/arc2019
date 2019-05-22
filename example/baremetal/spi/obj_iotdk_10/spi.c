#include "spi.h"
/*
int spiInit(uint8_t devId) {
    int ercd = E_SYS;
    spi_flash = spi_get_dev(devId);
    CHECK_EXP_NOERCD(spi_flash != NULL, error_exit);

    ercd = spi_flash->spi_open(DEV_MASTER_MODE, SPI_SPEED_STANDARD);
    //ercd = spi_flash->spi_control(SPI_CMD_MST_SEL_DEV, (void*)(OV7670_SCCB_ADDR));

error_exit:
    return ercd;
};

bool spiWrite(uint8_t regAddr, uint8_t data) {
    // static int cnt = 0;
    int ercd = 0;
    while(spi_flash->spi_control(SPI_CMD_MST_SEL_DEV, (void*)(WRSR)) != E_OK) {};
    spi_flash->spi_write(SPI_CMD_MST_SEL_DEV, (void*)(WRSR));

    uint8_t seq[2] = {regAddr, data};
    ov7670_sccb->iic_control(IIC_CMD_MST_SET_NEXT_COND, (void*)(IIC_MODE_STOP));
    ercd = ov7670_sccb->iic_write(seq, 2);
    // EMBARC_PRINTF("%d\n", ++cnt);
    board_delay_ms(1, 0);
    return (ercd == 2)? true: false;
};

uint8_t spiRead(uint8_t regAddr) {
    int ercd = 0;
    uint8_t data;
    ov7670_sccb->iic_control(IIC_CMD_MST_SET_NEXT_COND, (void*)(IIC_MODE_STOP));
    ercd = ov7670_sccb->iic_write(&regAddr, 1);
    ercd = ov7670_sccb->iic_read(&data, 1);
    board_delay_ms(1, 0);
    return data;
};*/