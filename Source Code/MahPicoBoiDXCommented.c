/**
 * MahPicoBoi is based on the Raspberry Pi Pico UART_ADVANCED, button and hello_timer example projects of the Raspberry Pi Foundation
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//Import the required libraries from the Pico C/C++ SDK
#include "pico/stdlib.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/sync.h"

//We're recieving MIDI through UART0 on pin 13, and sending MIDI via pin 12 (see Pico Pinout, you can change these pin numbers if needed) 
#define UART_ID uart0
#define RX_PIN 13
#define TX_PIN 12

//We'll use the Pico's LED to show us the current state of the MahPicoBoi.
#define LED_PIN PICO_DEFAULT_LED_PIN
//Serial-Out is done on Pin 3 of the Pico. You can change this pin number as needed.
#define SERIAL_PIN 3
//Pin 4 generates the external clock signals that the GameBoy needs for its serial transfers.
#define CLOCK_PIN 4

//We're recieving 10bit Midi-Messages of 1 Start-Bit, 1 Stop-Bit and 8 Data-Bits without Parity @31250 Baud 
#define BAUD_RATE 31250
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

//This bool filters out Midi CC messages if set to 'true'. This may very well be desired when using DAWs to play Midi and can prevent crashes on the updated mGB build.
//During operation, the value of this bool will be changed whenever the Bootsel button is pressed.
static bool disableCC = false;

//Tracks whether the bootsel button is currently being held down. 
static bool bootselPressed = false;

//Tracks whether the LED is currently on (true) or off (false). 
static bool ledState = true;

//We use this 8 bit int to make sure that we only ever have one LED blink timer running at the same time. 
static uint8_t numberOfCallbacks = 0;

//Tracks how many databytes of our current MIDI message we've sent to our Game Boy. If this var is 0, we're waiting for the next Status Byte.
static uint8_t bytesSent = 0;

//From examples button.c
//Tells us, if bootsel is currently being pressed.
bool __no_inline_not_in_flash_func(get_bootsel_button)() {
    const uint CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i);

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
    bool button_state = !(sio_hw->gpio_hi_in & (1u << CS_PIN_INDEX));

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}

//This function sends a single byte out to the Game Boy Link Port via the Link Cable using our Serial and Clock pins before putting the Pico to sleep for 500 microseconds.
void SendMidiToGB(uint8_t byte)
{
    //With this for-loop, we'll send out all 8 data-bits of our Midi-message in sequence.
    for(int bCounter = 0; bCounter < 8; bCounter++) 
    {
        //First, we'll need to pull our clock pin from it's default HIGH state to LOW.
        //Additionally, we'll put the Pico to sleep for one microsecond between each action, just to make sure that all pins are in the desired state during transmission.
        gpio_put(CLOCK_PIN, 0);
        sleep_us(1);

        //Next, we'll check the value of our most significant remaining bit.
        //If it is a 1, we'll pull our serial pin HIGH, otherwise we'll pull it LOW.
        gpio_put(SERIAL_PIN, (byte & 0x80) );
        sleep_us(1);

        //Now, we'll put our Clock pin back to its default HIGH state.
        gpio_put(CLOCK_PIN, 1);
        sleep_us(1);
        
        //Finally, we'll shift the remaining bits of our byte one position to the right so that our second most significant bit now becomes our new most significant bit .
        byte <<= 1;
    }
    //After transmitting our byte, we'll put the Pico to sleep for 500us. This gives the Game Boy just enough time to deal with our transmitted MIDI-byte. 
    //Lower Values down to 1us also work, but do seem to be prone to occasional Note / Signal skipping on the DMG-01.
    //There's optimization potential here, I believe, but we'll keep it at 500us for now.
    sleep_us(500);
}

//This function reads out all data within the FIFO buffer of our UART.
//If the popped byte complies with our rules (e.g. no CC messages), it will be immediately sent out via the serial pin.
void ReadRX() 
{
    //Check if there is data in the FIFO.  
    while (uart_is_readable(UART_ID))
    {
        //Get the first byte within the FIFO.
        uint8_t byte = uart_getc(UART_ID);

        //Create a check variable so we can easily observe the first nibble of our byte.
        uint8_t check = byte >> 4;

        //This mess of bitwise ops is nothing more than a sanity check.
        //If we're waiting on a Status byte, the MSB of our nibble needs to be 1, otherwise it needs to be 0.
        if(!((bytesSent == 0) && !(check & 0x8)) && !((bytesSent > 0) && (check & 0x8)))
        {
            //If our byte is sane, send it out via the TX pin.
            if (uart_is_writable(UART_ID))
                uart_putc(UART_ID, byte);

            //If we intend on blocking CC signals, we also need to make sure that our first nibble doesn't equal 0xB.
            if(!(disableCC && (check == 0xB)))
            {
                //If our byte is truly confirmed sane, we can send it out to the Game Boy without worries.
                SendMidiToGB(byte);

                //Finally, we want to iterate bytesSent.
                //If we sent out a status or data byte, iterate bytesSent by 1 unless bytesSent is 2, in which case we'll reset it to 0 and start waitign for a new status byte.
                //If we just sent out the status byte of a programm change message, bytesSent needs to be iterated by 2 since PC messages are only 2 bytes long.
                (bytesSent == 2) ? (bytesSent = 0) : (bytesSent += 1 + (check == 0xC));
            }
        }
    }
}

//Blinks the LED of our Pico if we're blocking CC messages
int64_t Blink()
{
        //Invert and apply ledState
        ledState = !ledState;
        gpio_put(LED_PIN, ledState);

        //If we're still blocking CC messages, we'll start a timer to call this function again half a second from now.
        if(disableCC)
        {
            add_alarm_in_ms(500, Blink, NULL, false);
            numberOfCallbacks = 1;
        }
        //Otherwise, we'll just turn the LED on
        else
        {
            ledState = true;
            gpio_put(LED_PIN, ledState);
            numberOfCallbacks = 0;
        }
    return 0;
}

int main() 
{
    //This code initializes our UART with the typical Midi baud rate of 31250hz
    uart_init(UART_ID, BAUD_RATE);

    //This function sets up our TX and RX pins for sending and recieving MIDI via UART.
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(TX_PIN, GPIO_FUNC_UART);

    //This function sets up the format of the Midi messages we want to recieve via UART.
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    //This code initiates our LED Pin and pulls it high so that we can see the LED light turn on during normal operation.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);

    //This code initializes our Serial pin as a digital out pin and sets it to its default LOW state.
    gpio_init(SERIAL_PIN);
    gpio_set_dir(SERIAL_PIN, GPIO_OUT);
    gpio_put(SERIAL_PIN, 0);

    //This code initializes our Clock pin as a digital out pin and sets it to its default HIGH state.
    gpio_init(CLOCK_PIN);
    gpio_set_dir(CLOCK_PIN, GPIO_OUT);
    gpio_put(CLOCK_PIN, 1);
    
    //Run the main portion of our code forever
    while (1) 
    {
        //Check if bootsel is currently being pressed
        if(get_bootsel_button())
        {
            //If it wasn't pressed before, we'll invert the state of disableCC and set bootselPressed to true
            if (!bootselPressed)
            {
                disableCC = !disableCC;
                bootselPressed = true;

                //If we are now blocking CC messages, we'll start blinking our LED to let the user know.
                if(disableCC)
                {
                    gpio_put(LED_PIN, 0);
                    if(numberOfCallbacks == 0)
                        Blink();
                }
                //Otherwise, we'll just keep our LED on.
                else
                    gpio_put(LED_PIN, 1);
                        
            }
        }
        //If bootsel isn't currently being pressed, set bootselPressed to false.
        else
            bootselPressed = false;
        
        //If there's data in the FIFO read buffer, enter ReadRX().
        if(uart_is_readable(UART_ID))
            ReadRX();
    }
}