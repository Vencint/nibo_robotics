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


/**
 * Main function to run endless while-loop in.
 */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

int main() {
    init();

    start_nibo();

    while (true) {
        // if update of co-processor failed then restart while loop
        if (!request_distance_data()) continue;

        switch (get_current_state()) {
            case DEAD_END:
            case ALLEY:
            case OBSTACLE_AHEAD:
            case OBSTACLE_LEFT:
            case OBSTACLE_RIGHT:
                copro_stop();
                break;
            default:
                // default = nibo is free
                copro_setSpeed(20, 20);
                break;
        }

        delay(50);
    }
    return 0;
}

/**
 * Function waiting for the user to start the nibo.
 */
void start_nibo() {
    gfx_move(70, 0);
    gfx_set_proportional(1);
    gfx_print_text("Press S3 to start!");

    while (get_input_bit(IO_INPUT_1));

    gfx_move(70, 0);
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
    display_init(NIBO_DISPLAY_TYPE);
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
    } else if (is_near(DS_LEFT, NEAR)) {
        return OBSTACLE_LEFT;
    } else if (is_near(DS_RIGHT, NEAR)) {
        return OBSTACLE_RIGHT;
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
