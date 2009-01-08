#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include "display-driver.h"
uint8_t display_frame_buffer[8] = {255,129,129,129,129,129,129,255};
static uint8_t display_current_scan_line;

void display_set_frame_buffer(uint8_t *fb) {
    int i;
    for (i=0; i<8; i++) display_frame_buffer[i] = fb[i];
}
#define CATHODES PORTC
#define ANODES PORTA
void display_show() {
    CATHODES = 0;
    ANODES = ~(1 << display_current_scan_line);
    CATHODES = display_frame_buffer[display_current_scan_line];

    display_current_scan_line++;
    if (display_current_scan_line >= 8) display_current_scan_line = 0;
}

