/**
 * @file main.c
 * @author Vincent Luedtke
 * @date 30.10.20
 * @brief Program to let the nibo drive until it encounters an obstacle. The nibo will stop in that case.
 */

#include <nibo/niboconfig.h>
#include <nibo/delay.h>
#include <avr/interrupt.h>
#include <nibo/display.h>
#include <nibo/gfx.h>
#include <nibo/spi.h>
#include <nibo/bot.h>
#include <nibo/copro.h>
#include <stdbool.h>
#include <iso646.h>
#include <nibo/iodefs.h>
#include <nibo/leds.h>

#include "jan.h"
#include "utils.h"


void init();

uint8_t request_distance_data();

void start_nibo();


/**
 * Main function to run endless while-loop in.
 */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    init();

    start_nibo();

    while (true) {
        // if update of co-processor failed then restart while loop
        if (!request_distance_data()) continue;

        switch (get_current_state()) {
            case DEAD_END:
                leave_dead_end();
                // Rückwärts heraus fahren, bis rechts oder links frei, danach 180 Grad Drehung
            case ALLEY:
                // Justieren und geradeaus fahren
            case OBSTACLE_AHEAD:
                // Sensor, der besten Wert liefert finden und in diese Richtung fahren
            case OBSTACLE_LEFT_AHEAD:
                // Darauf achten, dass er nicht näher kommt
            case OBSTACLE_RIGHT_AHEAD:
                copro_stop();
                break;
            default:
                // default = nibo is free
                copro_setSpeed(20, 20);
                break;
        }

        delay(500);
    }
    return 0;
}
#pragma clang diagnostic pop

/**
 * Function waiting for the user to start the nibo.
 */
void start_nibo() {
    gfx_move(0, 0);
    gfx_set_proportional(1);
    gfx_print_text("Press S3 to start!");

    while (get_input_bit(IO_INPUT_1));

    gfx_move(0, 0);
    gfx_set_proportional(1);
    /* TODO: Display has to be cleared before changing its message to the following (because there's text from the
             previous message shown on the display after changing to the following one) */
    gfx_print_text("Start nibo...");
}

/**
 * Function to initialise all the needed peripherals.
 */
void init() {
    sei();
    bot_init();
    spi_init();
    display_init(DISPLAY_TYPE_TXT);
    gfx_init();

    copro_ir_startMeasure();
}

/**
 * Function to update co-processor, return the result and print an error message on the display if needed.
 * @return 1 if everything went well; 0 otherwise
 */
uint8_t request_distance_data() {
    uint8_t update = copro_update();
    if (!update) {
        gfx_move(10, 10);
        gfx_set_proportional(1);
        gfx_print_text("COPRO Error:");
        gfx_move(0, 10);
        gfx_print_text("can't update distance sensor data");
    }
    return update;
}
