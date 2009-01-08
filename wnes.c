#include <avr/io.h>
#include <avr/interrupt.h>
#include "display-driver.h"
#include "snes.h"
#include "leveldata.c"

/*void delayms(volatile uint16_t millis) {
  while ( millis ) {
  _delay_ms(1);
  millis--;
  }
  }*/

uint8_t game_board[8] = {0,0,0,0,0,0,0,0}, random_board[8];

enum GAME_MODES {
    GAME,
    CONFIRM_RESET, 
    CONFIRM_QUIT, 
    CONFIRM_RESET_MESSAGE, 
    CONFIRM_QUIT_MESSAGE,
};
uint8_t game_mode = GAME;
int8_t pos_x = 0, pos_y = 0;
uint8_t current_level = 0;
#define RANDOM_BYTE(s) *(s++)
void load_level(int l) {
    uint8_t i;
    if (l > 4) {
        uint8_t * seed = ((uint8_t*)level_data) + l*2+11;
        uint8_t x, y;
        for (i=0; i<8; i++)
            game_board[i] = 0;
        x = RANDOM_BYTE(seed);
        pos_y = y = x >> 4;
        pos_x = x = x & 7;
        l = l * 2 + 10;
        for (i=0; i<l; i++) {
            switch (RANDOM_BYTE(seed)&3) {
                case 0: if (x>0) x--; break;
                case 1: if (x<7) x++; break;
                case 2: if (y>0) y--; break;
                case 3: if (y<7) y++;
            }
            game_board[y] ^= _BV(x);
        }
    } else {
        for (i=0; i<8; i++)
            game_board[i] = level_data[l][i];
        pos_x = level_data_home_pos[l] >> 4;
        pos_y = level_data_home_pos[l] & 7;
    }
}

uint8_t selection_yes = 0;


#define NOP() {asm("nop");}
int main(void) {
    uint16_t ticker = 0;

    /* enable interrupts */
    //sei();

#if 0
    /* configure Analog-to-Digital Control Status Register A */
    ADCSRA =
    _BV(ADEN) | /* Analog-to-Digital ENable */
    _BV(ADPS0) | _BV(ADPS1); /* Analog-to-Digital Pre-Scaling factor of 8 */
#endif

    asm("ldi r24,0x80\n"
        "out 0x34,r24\n"
        "out 0x34,r24\n");

    DDR_SNES_PORT = SNES_LATCH | SNES_CLOCK;

    DDR_DISPLAY_ANODES      = 255;
    DDR_DISPLAY_CATHODES    = 255;
    DISPLAY_ANODES          = 0;
    DISPLAY_CATHODES        = 255;

    //start_adc();

    //display_set_frame_buffer(face_smiley);

    load_level(0);
    while(1) {
        //uint16_t snes_buttons = 0, snes_changes;
        uint8_t i;
        uint8_t snes_ready = snes_pump();
        
// NOTE i realize the button logic is COUNTERINTUITIVE

        /* Clear frame buffer TODO why redraw framebuffer 8 times per frame? */
        for (i=0; i<8; i++) display_frame_buffer[i] = 0;

        switch (game_mode) {
        goto_game:
        case GAME: {
        uint8_t moved = 0;

        if (snes_ready) {
            /* Player can reset the level with R */ 
            if (ON_BUTTON       (PLAYER0,BUTTON_R))
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_R)) {
                selection_yes = 0;
                game_mode = CONFIRM_RESET;
                goto goto_confirmation;
            }
            /* Player can reset the game with L */
            if (ON_BUTTON       (PLAYER0,BUTTON_L))
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_L)) {
                selection_yes = 0;
                game_mode = CONFIRM_QUIT;
                goto goto_confirmation;
            }
            /* Player moves with arrow keys */
            if (ON_BUTTON       (PLAYER0,BUTTON_RIGHT))
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_RIGHT))
            if (pos_x < 7) {
                pos_x++;
                moved = 1;
            }
            if (ON_BUTTON       (PLAYER0,BUTTON_LEFT))
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_LEFT))
            if (pos_x > 0) {
                pos_x--;
                moved = 1;
            }
            if (ON_BUTTON       (PLAYER0,BUTTON_DOWN))
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_DOWN))
            if (pos_y < 7) {
                pos_y++;
                moved = 1;
            }
            if (ON_BUTTON       (PLAYER0,BUTTON_UP))
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_UP))
            if (pos_y > 0) {
                pos_y--;
                moved = 1;
            }
            if (moved) {
                int j;
                //if (game_board[pos_y] & _BV(pos_x)) {
                    game_board[pos_y] ^= _BV(pos_x);
                    for (j=0; j<8; j++)
                        if (game_board[j]) j=9;
                    if (j == 8) load_level(++current_level);
                //}
                //else load_level(current_level);
            }
        }/* snes_ready */

        /* Display game board lights */
        for (i=0; i<8; i++)
            display_frame_buffer[i] = game_board[i];

        /* Display player position */ 
        if (IS_BUTTON_UP(PLAYER0,BUTTON_B)) { /* B button hides player */
            if ((ticker & 0170) == 010)
                display_frame_buffer[pos_y] |= _BV(pos_x); /* plot high */
            else
                display_frame_buffer[pos_y] &=~_BV(pos_x); /* plot high */
        }

        }break; /* case GAME */

        goto_confirmation:
        case CONFIRM_RESET:
        case CONFIRM_QUIT:
        {
        uint8_t draw_y = 1, draw_n = 1;

        /* user control */
        if (snes_ready) {
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_LEFT))
            if (ON_BUTTON       (PLAYER0,BUTTON_LEFT))
                selection_yes = 1;
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_RIGHT))
            if (ON_BUTTON       (PLAYER0,BUTTON_RIGHT))
                selection_yes = 0;
            if (IS_BUTTON_DOWN  (PLAYER0,BUTTON_B))
            if (ON_BUTTON       (PLAYER0,BUTTON_B)) {
                if (selection_yes) {
                    if (game_mode == CONFIRM_RESET)
                        load_level(current_level);
                    else if (game_mode == CONFIRM_QUIT)
                        load_level(current_level = 0);
                }
                game_mode = GAME;
                goto goto_game;
            }
        } /* snes_ready */

        /* Selection flashes! */
        if ((ticker & 0170) != 010) {
            if (selection_yes == 0) draw_n = 0;
            else                draw_y = 0;
        }
        if (draw_y) { /* TODO optimize! */
            PLOT(0,0);
            PLOT(0,1);
            PLOT(2,0);
            PLOT(2,1);
            PLOT(1,2);
            PLOT(1,3);
        }
        if (draw_n) { /* TODO optimize! */
            PLOT(7,7);
            PLOT(7,6);
            PLOT(7,5);
            PLOT(7,4);
            PLOT(4,7);
            PLOT(4,6);
            PLOT(4,5);
            PLOT(4,4);
            PLOT(6,6);
            PLOT(5,5);
        }
        }
        break;

        /*
        case CONFIRM_RESET_MESSAGE:
        case CONFIRM_QUIT_MESSAGE:
        goto_message:
        if ((ticker & 0x30) == 0x10) {
            if (++message_scroll >= (message_length+8))
        }
        break;*/

        }/* switch (game_mode) */


        /* Display (one eighth of) framebuffer */
        display_show();
        ticker++;
    }
}
