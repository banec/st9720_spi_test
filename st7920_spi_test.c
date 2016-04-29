/*
 * Simple test for ST9720 LCD controller connected to Raspberry Pi
 * Uses some routines from https://github.com/dsvost/st7920fb
 * Schematic and details can be found in README
 *
 * Bane Cvetkovic - http://github.com/banec/st9720_spi_test
 */


#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "st7920_spi_test.h"

#define PIN_RST 4 // GPIO23

int main(int argc, char **argv) {
    if(wiringPiSetup() == -1) {
        printf("wiringPiSetup() failed\n");
        return EXIT_FAILURE;
    }

    // 540kHz speed - recomended by ST7920 spec
    if ((lcd_fd = wiringPiSPISetupMode(0, 540000, 0x07)) < 0) {
        printf("Can't open the SPI bus\n");
        return EXIT_FAILURE;
    }

    init_gpio();
    reset_lcd();
    init_lcd();
    
    set_extended_mode(0, 0, 0);
    set_extended_mode(0, 1, 1);

    clear_lcd();

    show_image(raspberry_pix);

    return EXIT_SUCCESS;
}

// Set PIN_RST as output
// On ST7920 reset pin is active low, 
// so we need to pull it high
void init_gpio() {
    pinMode(PIN_RST, OUTPUT);
    digitalWrite(PIN_RST, HIGH);
    delay(100);
}

// Set PIN_RST to low for a short time
// to reset controller
void reset_lcd() {
    digitalWrite(PIN_RST, LOW);
    delay(100);
    digitalWrite(PIN_RST, HIGH);
    delay(100);
}

void init_lcd() {
    delay(40); //Startup delay
    send_cmd(0b00110000); /* 8 Bit interface (DL=1), basic instruction set (RE=0) */
    send_cmd(0b00001100); /* display on, cursor & blink off */
    send_cmd(0b00011100); /* Entry mode: Cursor move to right, DDRAM address counter (AC) plus 1, no shift */
    send_cmd(0b00000011); /* disable scroll, enable CGRAM adress */
    send_cmd(0b00000001); /* clear RAM, needs 1.6 ms */
    delayMicroseconds(1600 - 72); // Delay 1.6 ms needed, 72 us done already
}

void clear_lcd() {
    int v,h;    
    for (v=0; v<32; v++) {
        set_gdram_addr(v, 0);
        for (h=0; h<32; h++) {
            send_data(0);
            delayMicroseconds(10);
        }
    }
}

// Show 128x64 1bpp image
void show_image(const uint8_t *pixmap) {
    write_buffer(pixmap, 1024, 0);
}

void write_buffer(const uint8_t *buf, uint32_t size, loff_t *ppos) {
    uint32_t offset = 0;
    int x,y;
    uint8_t val = 0;

    x=0; y=0;
    for (y=0;y<32;y++) {

        set_gdram_addr(y,0);
        for (x=0;x<16;x++) {
            offset = y*16+x;
            if (offset>size)
                break;
            val = (*(buf + offset));
            send_data(val);
        }
        for (x=0;x<16;x++) {
            offset = 512+y*16+x;
            if (offset>size)
                break;
            val = (*(buf + offset));
            send_data(val);
        }
    }

}

void set_extended_mode(uint8_t dl, uint8_t re, uint8_t gd_state) {
    send_cmd((1<<5) | (dl<<4) | (re<<2) | (gd_state<<1));
    delayMicroseconds(10);
}

void set_gdram_addr(uint8_t vaddr, uint8_t haddr) {
    send_cmd(vaddr|(1<<7));
    send_cmd(haddr|(1<<7));
}

void send_cmd(uint8_t byte) {
    cmd_lcd(0, 0, byte);
}

void send_data(uint8_t byte) {
    cmd_lcd(1, 0, byte);
}

void cmd_lcd(uint8_t rs, uint8_t rw, uint8_t param) {
    static uint8_t gspi_buf[3];
    
    if (rs == 0) {
        gspi_buf[0] = 0x0F8;
    } else {
        gspi_buf[0] = 0x0FA;
    }

    gspi_buf[1] = (param & 0x0F0);
    gspi_buf[2] = (param<<4);
    
    if (wiringPiSPIDataRW(lcd_fd, gspi_buf, 3) == -1) {
        printf ("SPI failure\n");
    }
    
    delayMicroseconds(72); // Delay for most instructions
}


