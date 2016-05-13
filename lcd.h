 /*******************************************************************************
*		  File:		lcd.h
*		 Title:		LCD header file for use with 24C16.c
*	also needs:		lcd.h
*	  Compiler:		AVR-GCC 3.3.1
*	   Version:		1.0
*  last update:		30-03-2004
*		Target:		ATmega8
*		Author:		Christoph Redecker
*	   Company:		www.avrbeginners.net
********************************************************************************
* Electrical connection:
* LCd data port <-> PortD
* RS <- PortC.0
* RW <- PortC.1
*  E <- PortC.2
*******************************************************************************/
#define LCD_RS 2
#define LCD_RW 1
#define LCD_E 5

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
/*******************************************************************************
* LCD_putchar writes a character to the LCD display ram. It does not do ANY
*	busy flag checking, neither before nor after the write operation
* LCD_command writes command or address data (if bit 7 set) to the LCD.
* LCD_init initialises the LCD with the given values (see function definition)
* LCD_write prints a string on the LCD. No checking for non-terminated string is done!
* LCD_puthex prints a hex view of a given value on the LCD.
*	Example: LCD_puthex(1) prints "0x01" on the LCD
*******************************************************************************/
//Prototypes:
void LCD_putchar(char data);void LCD_command(char command);
void LCD_init(void);
void LCD_write(char* dstring);
void LCD_puthex(const char data);

char hstring[16] = "0123456789ABCDEF"; //used by LCD_puthex

void LCD_putchar(char data)
{
	//PortD is output
	DDRD = 0xFF;
	//put data on bus
	PORTD = data;
	//RW low, E low
	PORTC &= ~(1<<LCD_E);
	//RS high, strobe E
	PORTC |= (1<<LCD_E);
	PORTB |= (1<<LCD_RS);
	//the number of nops required varies with your clock frequency, try it out!
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	//RS low again, E low (belongs to strobe)
	PORTC &= ~(1<<LCD_E);
	PORTB &= ~(1<<LCD_RS);
	//release bus
	DDRD = 0;
	_delay_us(160);
}

//LCD_command works EXACTLY like LCD_putchar, but takes RS low for accessing the command reg
//see LCD_putchar for details on the code
void LCD_command(char command)
{
	DDRD = 0xFF;
	PORTD = command;
	PORTC &= ~(1<<LCD_E);
	PORTB &= ~(1<<LCD_RS);
	PORTC |= (1<<LCD_E);
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	asm volatile ("nop");
	PORTC &= ~(1<<LCD_E);
	DDRD = 0;
	_delay_us(160);
}

/*LCD_init initialises the LCD with the following paramters:
8 bit mode, 5*7 font, 2 lines (also for 4 lines)
auto-inc cursor after write and read
cursor and didsplay on, cursor blinking.
*/
void LCD_init(void)
{
	//setup the LCD control signals on PortC
	
    
	//LCD_RW is not used this time as it is connected directly to ground
	DDRB |= (1<<LCD_RS);
	DDRC |= (1<<LCD_E);
	PORTB |= (0<<LCD_RS);
	PORTC |= (0<<LCD_E);
	
	//if called right after power-up, we'll have to wait a bit (fine-tune for faster execution)
	_delay_ms(15);
	//tell the LCD that it's used in 8-bit mode 3 times, each with a delay inbetween.
	LCD_command(0x30);
	_delay_ms(5);
	LCD_command(0x30);
	_delay_us(160);
	LCD_command(0x30);
	_delay_us(160);
	//now: 8 bit interface, 5*7 font, 2 lines.
	LCD_command(0x38);
	//wait until command finished
	_delay_us(160);
	//display on, cursor off
	LCD_command(0x0C);
	_delay_us(160);
	//now clear the display, cursor home
	LCD_command(0x01);
	_delay_us(160);
	//cursor auto-inc
	LCD_command(0x06);
	_delay_ms(2);
	
}


//now it's time for a simple function for showing strings on the LCD. It uses the low-level
//functions above. usage example: LCD_write("Hello World!");
void LCD_write(char* dstring)
{
	//is the character pointed at by dstring a zero? If not, write character to LCD
	while(*dstring)
	{
		//if the LCD is bus, let it finish the current operation
		_delay_us(160);
		//the write the character from dstring to the LCD, then post-inc the dstring is pointing at.
		LCD_putchar(*dstring++);
	}
}

void LCD_puthex(const char data)
{
	//wait for the LCD to be ready
	_delay_us(160);
	//print '0'
	LCD_putchar('0');
	_delay_ms(2);
	//print 'x'
	LCD_putchar('x');
	_delay_ms(2);
	//print high nibble as hex
	LCD_putchar(hstring[data>>4]);
	_delay_ms(2);
	//print low nibble as hex
	LCD_putchar(hstring[data & 0x0F]);
	_delay_us(160);
}