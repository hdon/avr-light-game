#include <avr/io.h>
#include "snes.h"

uint8_t snes_buttons[6] = {0,0,0,0,0,0};
uint8_t snes_changes[6] = {0,0,0,0,0,0};

uint8_t snes_pump() {
    static uint8_t count = 0;
    static uint8_t incoming[6] = {0,0,0,0,0,0};

    /* Step 0: Release latch and clock (high) */
    if (count == 0) {
        SNES_PORT |= SNES_LATCH | SNES_CLOCK;
        incoming[0] = 0;
        incoming[1] = 0;
        incoming[2] = 0;
        incoming[3] = 0;
        incoming[4] = 0;
        incoming[5] = 0;
    }

    /* Step 1: Engage latch and clock (low) */
    else if (count == 1)
        SNES_PORT &= ~SNES_LATCH;

    /* Step 2 through 14: Strobe clock, collect data */
    else if (count <= 14) {
        uint8_t pin;
        
        /* Engage clock (low) */
        SNES_PORT &= ~SNES_CLOCK;

        /* Collect data */
        pin = SNES_PIN & (SNES_DATA0 | SNES_DATA1);
        if (pin & SNES_DATA0) incoming[(count-2+PLAYER0)/8] |= 1 << ((count - 2 + PLAYER0) %8);
        if (pin & SNES_DATA1) incoming[(count-2+PLAYER1)/8] |= 1 << ((count - 2 + PLAYER1) %8);

        /* Release clock (high) */
        SNES_PORT |= SNES_CLOCK;

        /* Completed sweep? */
        if (count == 14) {
            snes_changes[0] = snes_buttons[0] ^ incoming[0];
            snes_changes[1] = snes_buttons[1] ^ incoming[1];
            snes_changes[2] = snes_buttons[2] ^ incoming[2];
            snes_changes[3] = snes_buttons[3] ^ incoming[3];
            snes_changes[4] = snes_buttons[4] ^ incoming[4];
            snes_changes[5] = snes_buttons[5] ^ incoming[5];

            snes_buttons[0] = incoming[0];
            snes_buttons[1] = incoming[1];
            snes_buttons[2] = incoming[2];
            snes_buttons[3] = incoming[3];
            snes_buttons[4] = incoming[4];
            snes_buttons[5] = incoming[5];

            count = 0;
            return 1;
        }
    }
    count++;
    return 0;
}

