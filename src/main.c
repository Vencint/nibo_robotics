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

#include "vincent.c"
#include "patrick.c"
#include "utils.h"


void init();

uint8_t request_distance_data();

void start_nibo();

int leave_dead_end();

/**
 * Function to find out if a distance sensor detected a nearby obstacle.
 * @param sensor the sensor to get distance from
 * @param threshold the threshold for when an obstacle is considered "near"
 * @return true if there is an obstacle nearby, false otherwise
 */
bool is_near(enum distance_sensors sensor, uint16_t threshold) {
    return copro_distance[sensor] / 128 > threshold;
}

enum state get_current_state() {
    if (is_near(DS_LEFT, NEAR) and is_near(DS_RIGHT, NEAR) and is_near(DS_FRONT, NEAR)) {
        return DEAD_END;
    } else if (is_near(DS_LEFT, NEAR) and is_near(DS_RIGHT, NEAR)) {
        return ALLEY;
    } else if (is_near(DS_FRONT, NEAR)) {
        return OBSTACLE_AHEAD;
    } else if (is_near(DS_FRONT_LEFT, NEAR)) {
        return OBSTACLE_LEFT_AHEAD;
    } else if (is_near(DS_FRONT_RIGHT, NEAR)) {
        return OBSTACLE_RIGHT_AHEAD;
    } else {
        return FREE;
    }
}

/**
 * Main function to run endless while-loop in.
 */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "EndlessLoop"

int main() {
    init();

    // TODO: WARUM FUNKTIONIERT DAS NICHTẞ1
    leds_set_status(4, 1);
    leds_set_status(5, 1);

    start_nibo();

    while (true) {
        // if update of co-processor failed then restart while loop
        if (!request_distance_data()) continue;

        switch (get_current_state()) {
            case DEAD_END:
                // Rückwärts heraus fahren, bis rechts oder links frei, danach 180 Grad Drehung
                leave_dead_end();
                break;
            case ALLEY:
                // Justieren und geradeaus fahren

                // TODO: Vor merge mit allen anderen entfernen!
                copro_stop();
                break;
            case OBSTACLE_AHEAD:
                // Sensor, der besten Wert liefert finden und in diese Richtung fahren

                // TODO: Vor merge mit allen anderen entfernen!
                copro_stop();
                break;
            case OBSTACLE_LEFT_AHEAD:
                // Darauf achten, dass er nicht näher kommt

                // TODO: Vor merge mit allen anderen entfernen!
                copro_stop();
                break;
            case OBSTACLE_RIGHT_AHEAD:
                // Darauf achten, dass er nicht näher kommt

                // TODO: Vor merge mit allen anderen entfernen!
                copro_stop();
                break;
            default:
                // default = nibo is free
                // TODO: Vor merge mit allen anderen entfernen!
                copro_stop();
                // copro_setSpeed(5, 5);
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

    // Clear text currently shown on the display
    gfx_fill(0);

    gfx_print_text("Start nibo...");
}

/**
 * Function to initialise all the needed peripherals.
 */
void init() {
    sei();
    bot_init();
    leds_init();
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

