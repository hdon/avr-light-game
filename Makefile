CFLAGS:=-DSNES_PORT='PORTB' -DSNES_PIN='PINB' -DDDR_SNES_PORT='DDRB' -DSNES_DATA0='_BV(0)' -DSNES_LATCH='_BV(1)' -DSNES_CLOCK='_BV(2)' -DDDR_DISPLAY_ANODES=DDRA -DDDR_DISPLAY_CATHODES=DDRC -DDISPLAY_ANODES=PORTA -DDISPLAY_CATHODES=PORTC -DSNES_DATA1='_BV(3)'

wnes.hex : wnes.exe
	avr-objcopy -R .eeprom -O ihex $< $@

wnes.exe : wnes.o display-driver.o snes.o
	avr-gcc $^ -o $@

wnes.o : leveldata.c

leveldata.c : levels.txt leveldump.js
	cat $< | js leveldump.js > $@

%.o : %.c
	avr-gcc -O3 $(CFLAGS) -Wall -mcall-prologues -mmcu=atmega32 -c $< -o $@

%.S : %.c
	avr-gcc -O3 -Wall -mcall-prologues -mmcu=atmega32 -S $< -o $@

upload :: wnes.hex
	avrdude -c ponyser -P /dev/ttyS0 -p m32 -U flash:w:wnes.hex:i

clean ::
	- rm -f *.exe *.o *.hex

