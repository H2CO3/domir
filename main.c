#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

typedef volatile uint8_t *Port;

// 8 / (8 + 18) = 0.308, which is between the recommended 0.25 and 0.333
#define CARRIER_THIRD_CYCLE_DELAY_US 8
#define CARRIER_TWO_THIRDS_CYCLE_DELAY_US 18

#define PAUSE_BIT_SET_US 1678
#define PAUSE_BIT_CLR_US 548

#define BIT_CYCLE_COUNT 22
#define AGC_CYCLE_COUNT 346

typedef struct PortPin {
    Port port;
    int pin;
} PortPin;

static void set_bit(PortPin pp) {
    *pp.port |= 1 << pp.pin;
}

static void clr_bit(PortPin pp) {
    *pp.port &= ~(1 << pp.pin);
}

static void send_burst(PortPin pp) {
    for (int i = 0; i < BIT_CYCLE_COUNT; i++) {
        set_bit(pp);
        _delay_us(CARRIER_THIRD_CYCLE_DELAY_US);
        clr_bit(pp);
        _delay_us(CARRIER_TWO_THIRDS_CYCLE_DELAY_US);
    }
}

static void send_bit(PortPin pp, bool bit) {
    send_burst(pp);

    if (bit) {
        _delay_us(PAUSE_BIT_SET_US);
    } else {
        _delay_us(PAUSE_BIT_CLR_US);
    }
}

static void send_byte(PortPin pp, uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        send_bit(pp, (byte >> i) & 0x01);
    }
}

static void send_header(PortPin pp) {
    for (int i = 0; i < AGC_CYCLE_COUNT; i++) {
        set_bit(pp);
        _delay_us(CARRIER_THIRD_CYCLE_DELAY_US);
        clr_bit(pp);
        _delay_us(CARRIER_TWO_THIRDS_CYCLE_DELAY_US);
    }

    clr_bit(pp);
    _delay_us(4500);
}

static void send_address_and_command_classic(
    PortPin pp,
    uint8_t address,
    uint8_t command
) {
    send_byte(pp, address);
    send_byte(pp, ~address);

    send_byte(pp, command);
    send_byte(pp, ~command);

    send_burst(pp); // burst after last bit to help encoding last bit
}

static void send_address_and_command_extended(
    PortPin pp,
    uint16_t address,
    uint8_t command
) {
    send_byte(pp, (address >> 0) & 0xff); // address low
    send_byte(pp, (address >> 8) & 0xff); // address high

    send_byte(pp, command);
    send_byte(pp, ~command);

    send_burst(pp); // burst after last bit to help encoding last bit
}

static void send_packet_classic(PortPin pp, uint8_t address, uint8_t command) {
    send_header(pp);
    send_address_and_command_classic(pp, address, command);
}

static void send_packet_extended(PortPin pp, uint16_t address, uint8_t command) {
    send_header(pp);
    send_address_and_command_extended(pp, address, command);
}

int main()
{
    PortPin ir_led_ddr  = { &DDRB,  4 };
    PortPin ir_led_port = { &PORTB, 4 };
    set_bit(ir_led_ddr);

	while (1) {
        send_packet_classic(ir_led_port, 0x00, 0xa2);
        _delay_ms(1000);
	}

	return 0;
}
