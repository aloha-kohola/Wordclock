#include "espressif/esp_common.h"
#include "esp8266.h"
#include "esp/slc.h"
#include "esp/slc_regs.h"
#include "FreeRTOS.h"
#include "task.h"
#include "driver/ws2812b.h"

//defines for i2s clock
extern int sdk_rom_i2c_writeReg_Mask(int block, int host_id, int reg_add, int Msb, int Lsb, int indata);
//extern void sdk_rom_i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb);

#ifndef i2c_bbpll
#define i2c_bbpll                                 0x67
#define i2c_bbpll_en_audio_clock_out            4
#define i2c_bbpll_en_audio_clock_out_msb        7
#define i2c_bbpll_en_audio_clock_out_lsb        7
#define i2c_bbpll_hostid                           4

#define i2c_writeReg_Mask(block, host_id, reg_add, Msb, Lsb, indata)  sdk_rom_i2c_writeReg_Mask(block, host_id, reg_add, Msb, Lsb, indata)
//#define i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb)  sdk_rom_i2c_readReg_Mask(block, host_id, reg_add, Msb, Lsb)
#define i2c_writeReg_Mask_def(block, reg_add, indata) \
      i2c_writeReg_Mask(block, block##_hostid,  reg_add,  reg_add##_msb,  reg_add##_lsb,  indata)
/*#define i2c_readReg_Mask_def(block, reg_add) \
      i2c_readReg_Mask(block, block##_hostid,  reg_add,  reg_add##_msb,  reg_add##_lsb)*/
#endif

#define WSBUFFER_SIZE 342

static uint32_t ws2812b_buffer[WSBUFFER_SIZE];
static uint32_t zero_buffer[32];

static struct SLC_REGS* slc = (struct SLC_REGS*)SLC_BASE;

struct SLCDescriptor i2sDesc;
struct SLCDescriptor zeroDesc;

// static void dma_finished_isr(void) {
//     uint32_t int_status = READ_PERI_REG(&(slc->INT_STATUS));
//     printf("DMA finished!\n");

//     printf("%X\n", int_status);

//     if(int_status & SLC_INT_STATUS_RX_DONE){
//         printf("DMA stopped!\n");
//         SET_PERI_REG_MASK(&(slc->RX_LINK), SLC_RX_LINK_STOP);
//     }
//     SET_PERI_REG_MASK(&(slc->INT_CLEAR), 0xFFFFFFFF);
// }

void ws2812b_init(void)
{
    //printf("Start init\n");

    //printf("Reset DMA\n");
    //Reset DMA
    SET_PERI_REG_MASK(&(slc->CONF0), SLC_CONF0_RX_LINK_RESET);
    CLEAR_PERI_REG_MASK(&(slc->CONF0), SLC_CONF0_RX_LINK_RESET);

    //printf("Clear DMA Interrupt Flags\n");
    //Clear DMA Interrupt Flags
    SET_PERI_REG_MASK(&(slc->INT_CLEAR), 0xFFFFFFFF);
    CLEAR_PERI_REG_MASK(&(slc->INT_CLEAR), 0xFFFFFFFF);

    //printf("Enable and configure DMA\n");
    //Enable and configure DMA
    CLEAR_PERI_REG_MASK(&(slc->CONF0), (SLC_CONF0_MODE_M << SLC_CONF0_MODE_S));
    SET_PERI_REG_MASK(&(slc->CONF0), (1 << SLC_CONF0_MODE_S));
    SET_PERI_REG_MASK(&(slc->RX_DESCRIPTOR_CONF), (SLC_RX_DESCRIPTOR_CONF_INFOR_NO_REPLACE | SLC_RX_DESCRIPTOR_CONF_TOKEN_NO_REPLACE));
    CLEAR_PERI_REG_MASK(&(slc->RX_DESCRIPTOR_CONF), (SLC_RX_DESCRIPTOR_CONF_RX_FILL_MODE | SLC_RX_DESCRIPTOR_CONF_RX_EOF_MODE | SLC_RX_DESCRIPTOR_CONF_RX_FILL_ENABLE));

    //printf("Zero buffer\n");
    for(uint32_t i = 0; i < WSBUFFER_SIZE; i++) {
        ws2812b_buffer[i] = 0;
    }

    for(uint32_t i = 0; i < 32; i++) {
        zero_buffer[i] = 0;
    }

    //printf("Init SLC Descriptor\n");
    //SLC_DESCRIPTOR_FLAGS(blocksize,datalen,sub_sof,eof,owner)
    i2sDesc.flags = SLC_DESCRIPTOR_FLAGS(WSBUFFER_SIZE, WSBUFFER_SIZE * 4, 0, 1, 1);
    i2sDesc.buf_ptr = (uint32_t)&ws2812b_buffer[0];
    i2sDesc.next_link_ptr = (uint32_t)&zeroDesc;

    zeroDesc.flags = SLC_DESCRIPTOR_FLAGS(32, 32 * 4, 0, 1, 1);
    zeroDesc.buf_ptr = (uint32_t)&zero_buffer[0];
    zeroDesc.next_link_ptr = (uint32_t)&i2sDesc;

    //printf("Set descriptor address\n");
    //Set descriptor address
    CLEAR_PERI_REG_MASK(&(slc->RX_LINK), SLC_RX_LINK_DESCRIPTOR_ADDR_M);
    SET_PERI_REG_MASK(&(slc->RX_LINK), ((uint32_t)&i2sDesc) & SLC_RX_LINK_DESCRIPTOR_ADDR_M);

    // _xt_isr_attach(INUM_SLC, dma_finished_isr);
    // SET_PERI_REG_MASK(&(slc->INT_ENABLE), SLC_INT_ENABLE_RX_DONE);
    // CLEAR_PERI_REG_MASK(&(slc->INT_CLEAR), 0xFFFFFFFF);
    // _xt_isr_unmask(1 << INUM_SLC);
    _xt_isr_mask(1 << INUM_SLC);


    //printf("Start DMA\n");
    //Start DMA
    SET_PERI_REG_MASK(&(slc->RX_LINK), SLC_RX_LINK_START);

    //printf("Set UART Recieve pin to I2S-Output-Data-Pin\n");
    //Set UART Recieve pin to I2S-Output-Data-Pin
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0RXD_U, FUNC_I2SO_DATA);
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_I2SO_WS);

    struct I2S_REGS* i2s = (struct I2S_REGS*)I2S_BASE;

    //printf("Set I2S Clock\n");
    //Set I2S Clock?
    i2c_writeReg_Mask_def(i2c_bbpll, i2c_bbpll_en_audio_clock_out, 1);

    //printf("Reset I2S\n");
    //Reset I2S
    CLEAR_PERI_REG_MASK(&(i2s->CONF), I2S_CONF_RESET_MASK);
    SET_PERI_REG_MASK(&(i2s->CONF), I2S_CONF_RESET_MASK);
    CLEAR_PERI_REG_MASK(&(i2s->CONF), I2S_CONF_RESET_MASK);

    //printf("Configre I2S\n");
    //Configure I2S
    CLEAR_PERI_REG_MASK(&(i2s->FIFO_CONF), (I2S_FIFO_CONF_DESCRIPTOR_ENABLE | /*(I2S_FIFO_CONF_RX_FIFO_MOD_M << I2S_FIFO_CONF_RX_FIFO_MOD_S) |*/ (I2S_FIFO_CONF_TX_FIFO_MOD_M << I2S_FIFO_CONF_TX_FIFO_MOD_S)));
    SET_PERI_REG_MASK(&(i2s->FIFO_CONF), I2S_FIFO_CONF_DESCRIPTOR_ENABLE);

    CLEAR_PERI_REG_MASK(&(i2s->CONF), (I2S_CONF_TX_SLAVE_MOD | (I2S_CONF_BITS_MOD_M << I2S_CONF_BITS_MOD_S) | (I2S_CONF_BCK_DIV_M << I2S_CONF_BCK_DIV_S) | (I2S_CONF_CLKM_DIV_M << I2S_CONF_CLKM_DIV_S)));
    SET_PERI_REG_MASK(&(i2s->CONF), (I2S_CONF_RIGHT_FIRST | I2S_CONF_MSB_RIGHT /*| I2S_CONF_RX_SLAVE_MOD | I2S_CONF_RX_MSB_SHIFT */ | I2S_CONF_TX_MSB_SHIFT | ((16 & I2S_CONF_BCK_DIV_M) << I2S_CONF_BCK_DIV_S) | ((3 & I2S_CONF_CLKM_DIV_M) << I2S_CONF_CLKM_DIV_S)));

    CLEAR_PERI_REG_MASK(&(i2s->CONF_CHANNELS), I2S_CONF_CHANNELS_TX_CHANNEL_MOD_M << I2S_CONF_CHANNELS_TX_CHANNEL_MOD_S);
    //SET_PERI_REG_MASK(&(i2s->CONF_CHANNELS), (0x01 << I2S_CONF_CHANNELS_TX_CHANNEL_MOD_S));

    //printf("Start I2S\n");
    //Start I2S
    SET_PERI_REG_MASK(&(i2s->CONF), I2S_CONF_TX_START);

}

void ws2812b_show(struct rgb* buffer, uint8_t num_leds)
{

    if((num_leds * 3) > WSBUFFER_SIZE) {
        return;
    }

    const uint16_t ws_nibbles[16] = {   0b1000100010001000, 0b1000100010001110, 0b1000100011101000, 0b1000100011101110,
                                        0b1000111010001000, 0b1000111010001110, 0b1000111011101000, 0b1000111011101110,
                                        0b1110100010001000, 0b1110100010001110, 0b1110100011101000, 0b1110100011101110,
                                        0b1110111010001000, 0b1110111010001110, 0b1110111011101000, 0b1110111011101110
                                    };

    for(uint32_t led = 0; led < num_leds; led++) {
        ws2812b_buffer[3 * led] = (((ws_nibbles[((buffer[led].green) & 0xF0) >> 4]) << 16) | (ws_nibbles[(buffer[led].green) & 0x0F]));
        ws2812b_buffer[3 * led + 1] = (uint32_t)(((ws_nibbles[((buffer[led].red) & 0xF0) >> 4]) << 16) | (ws_nibbles[(buffer[led].red) & 0x0F]));
        ws2812b_buffer[3 * led + 2] = (uint32_t)(((ws_nibbles[((buffer[led].blue) & 0xF0) >> 4]) << 16) | (ws_nibbles[(buffer[led].blue) & 0x0F]));
    }

    //SET_PERI_REG_MASK(&(slc->RX_LINK), SLC_RX_LINK_START);
}