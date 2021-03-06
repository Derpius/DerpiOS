#include "serial.h"
#include "../io_wrappers.h"

// Mostly code from https://littleosbook.github.io/book.pdf

void serial_configure_baud_rate(unsigned short com, unsigned short divisor)
{
	outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
	outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
	outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

void serial_configure_line(unsigned short com)
{
	/*
	Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
	Content: | d | b | prty  | s | dl  |
	Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
	*/
	outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

void serial_configure_buffers(unsigned short com)
{
	/*
	Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
	Content: | lvl | bs | r | dma | clt | clr | e |
	Value:   | 1 1 | 0  | 0 | 0   | 1   | 1   | 1 | = 0xc7
	*/
	outb(SERIAL_FIFO_COMMAND_PORT(com), 0xc7);
}

void serial_configure_modem(unsigned short com)
{
	/*

	Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
	Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
	Value:   | 0 | 0 | 0  | 0  | 0   | 0   | 1   | 1   | = 0x03
	*/
	outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

int serial_is_transmit_fifo_empty(unsigned int com)
{
	return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

void serial_init(unsigned int com, unsigned short divisor)
{
	serial_configure_baud_rate(com, divisor);
	serial_configure_line(com);
	serial_configure_buffers(com);
	serial_configure_modem(com);
}

void serial_print(const char* str, unsigned int com)
{
	for (; *str != '\0'; str++) {
		while (serial_is_transmit_fifo_empty(com) == 0);
		outb(com, *str);
	}
}
