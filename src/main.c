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
                break;
            case OBSTACLE_AHEAD:
                // Sensor, der besten Wert liefert finden und in diese Richtung fahren
                break;
            case OBSTACLE_LEFT_AHEAD:
                // Darauf achten, dass er nicht näher kommt
                break;
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

/**
 * This method tries to manoeuvre the nibo out of a dead end.
 * @return 1 if the nibo managed to leave the dead end, 0 if the nibo is still in the dead end (so far)
 */
int leave_dead_end() {
    // init arrays which contain the distance sensor values of the current and previous while loop
    uint16_t ds_current[5];
    uint16_t ds_previous[5];
    int16_t speed_left_wheel = -10;
    int16_t speed_right_wheel = -10;

    // assign current distance values to the array of "previous" distance values for now
    for (int i = 0; i < 5; ++i) {
        ds_previous[i] = copro_distance[i] / 256;
    }

    // start reversing
    copro_setSpeed(speed_left_wheel, speed_right_wheel);
    delay(500);

    // Enter loop for manoeuvring out of the dead end
    while (true) {
        // update distance sensor data
        copro_update();
        for (int i = 0; i < 5; ++i) {
            ds_current[i] = copro_distance[i] / 256;
        }

        // Check for obstacles on the right and left hand side of the nibo
        if (ds_current[DS_LEFT] < 50 and ds_current[DS_FRONT_LEFT] < 50) {
            // there's no obstacle to the left of the nibo, steer left and leave this function
            copro_setSpeed(-10, 10);
            delay(500);
            break;
        } else if (ds_current[DS_RIGHT] < 50 and ds_current[DS_FRONT_RIGHT] < 50) {
            // there's no obstacle to the right of the nibo, steer right and leave this function
            copro_setSpeed(10, -10);
            delay(500);
            break;
        }

        // compare current distance sensor values to the values of the previous loop
        if (ds_current[DS_LEFT] < ds_previous[DS_LEFT] && ds_current[DS_RIGHT] > ds_previous[DS_RIGHT]) {
            // Nibo is reversing to the left, steer a little to the right
            speed_left_wheel = -10;
            speed_right_wheel = -8;
        } else if (ds_current[DS_RIGHT] < ds_previous[DS_RIGHT] && ds_current[DS_LEFT] > ds_previous[DS_LEFT]) {
            // Nibo is reversing to the right, steer a little to the left
            speed_left_wheel = -8;
            speed_right_wheel = -10;
        } else {
            // Try to reverse in a straight line
            speed_left_wheel = -10;
            speed_right_wheel = -10;
        }

        /* copy current distance sensor values to the array which holds distance sensor values of the previous run
         * (for the next loop) */
        for (int i = 0; i < 5; ++i) {
            ds_previous[i] = ds_current[i];
        }

        // finally set the speed and have a delay to not overwhelm the motor with commands
        copro_setSpeed(speed_left_wheel, speed_right_wheel);
        delay(500);
    }
    return 0;
}
