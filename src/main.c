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
#include <stdlib.h>

#include "jan.c"
#include "vincent.c"
#include "patrick.c"



/**
 * Threshold for obstacles considered to be near a distance sensor.
 */
#define NEAR        (uint16_t) 0x00a0

/**
 * Enum for nibo's current state.
 */
enum state {
    ALLEY,
    DEAD_END,
    OBSTACLE_AHEAD,
    OBSTACLE_LEFT,
    OBSTACLE_RIGHT,
    OBSTACLE_LEFT_AHEAD,
    OBSTACLE_RIGHT_AHEAD,
    FREE
};

/**
 * Enum for the position of distance sensors.
 */
enum distance_sensors {
    DS_LEFT = 4,
    DS_FRONT_LEFT = 3,
    DS_FRONT = 2,
    DS_FRONT_RIGHT = 1,
    DS_RIGHT = 0
};

/**
 * Enum for the position of LEDs.
 */
enum leds {
    LEDS_LEFT = 2,
    LEDS_FRONT_LEFT = 3,
    LEDS_FRONT_FRONT_LEFT = 4,
    LEDS_FRONT_FRONT_RIGHT = 5,
    LEDS_FRONT_RIGHT = 6,
    LEDS_RIGHT = 7
};

void init();

uint8_t request_distance_data();

enum state get_current_state();

bool is_near(enum distance_sensors sensor, uint16_t threshold);

void start_nibo();

void drive_in_alley();


/**
 * Main function to run endless while-loop in.
 */
#pragma clang diagnostic push
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
                break;
            case ALLEY:
                // Justieren und geradeaus fahren
                drive_in_alley();
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
                copro_stop();
                break;
        }

        delay(50);
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
        gfx_print_text("COPRO Error   ");
    }
    return update;
}

/**
 * Function to get nibo's current state.
 * @return nibo's current state
 */
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
 * Function to find out if a distance sensor detected a nearby obstacle.
 * @param sensor the sensor to get distance from
 * @param threshold the threshold for when an obstacle is considered "near"
 * @return true if there is an obstacle nearby, false otherwise
 */
bool is_near(enum distance_sensors sensor, uint16_t threshold) {
    return copro_distance[sensor] / 128 > threshold;
}

/**
 * Function to drive through alley or tunnel.
 */
void drive_in_alley() {
    if (abs(copro_distance[4] - copro_distance[0]) / 128 < 0x00a0 / 2) {

        // driving straight through the middle of the alley
        if (abs(copro_distance[3] - copro_distance[1]) / 128 < 0x00a0) {
            copro_setSpeed(20, 20);
            gfx_move(0, 10);
            gfx_set_proportional(1);
            gfx_print_text("20 20");
        } else {
            // copro is in middle but doesn't drive straight
            if (copro_distance[3] > copro_distance[1]) {
                // oriented to the left so adjust slightly to the right
                copro_setSpeed(18, 15);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("15 18");
            } else {
                // oriented to the right so adjust slightly to the left
                copro_setSpeed(15, 18);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("18 15");
            }
        }

    } else {

        // not straight in the middle of the alley
        if (copro_distance[4] > copro_distance[0]) {

            if (copro_distance[3] > copro_distance[1]) {
                copro_setSpeed(18, 10);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("10 18");
            } else {
                copro_setSpeed(17, 20);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("20 17");
            }

        } else {

            if (copro_distance[1] > copro_distance[3]) {
                copro_setSpeed(10, 18);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("18 10");
            } else {
                copro_setSpeed(20, 17);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("17 20");
            }

        }

    }
}
