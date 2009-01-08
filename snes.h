#ifndef BUTTON_B
#include <inttypes.h>

#define PLAYER0         0
#define PLAYER1         12
#define PLAYER2         24
#define PLAYER3         36
#define BUTTON_B        0
#define BUTTON_Y        1
#define BUTTON_SELECT   2
#define BUTTON_START    3
#define BUTTON_UP       4
#define BUTTON_DOWN     5
#define BUTTON_LEFT     6
#define BUTTON_RIGHT    7
#define BUTTON_A        8
#define BUTTON_X        9
#define BUTTON_L        10
#define BUTTON_R        11

#define IS_BUTTON_DOWN(P,B) (!( snes_buttons[(P+B)/8] & ( 1<< ((P+B)%8) )))
#define IS_BUTTON_UP(P,B)     ( snes_buttons[(P+B)/8] & ( 1<< ((P+B)%8) ))
#define ON_BUTTON(P,B)        ( snes_changes[(P+B)/8] & ( 1<< ((P+B)%8) ))

extern uint8_t snes_buttons[];
extern uint8_t snes_changes[];
uint8_t snes_pump();

#endif
