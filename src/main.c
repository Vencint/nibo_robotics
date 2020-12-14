/**
 * @file main.c
 * @author Vincent Luedtke, Patrick Schlesinger, Jan Schorowski
 * @date 30.10.20
 * @brief Main part of the application taking care of initialization and endless while loop.
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
#include <stdio.h>

#include "utils.h"
#include "controller.h"


void init();

void start_nibo();

void display_speed();

void display_state(char *current_state);

void distance_detection();

void light_led_at_location(uint8_t color, int location);


/**
 * Main function to run endless while-loop in.
 * @author Vincent Luedtke, Patrick Schlesinger, Jan Schorowski
 */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma ide diagnostic ignored "EndlessLoop"
int main() {
    init();

    start_nibo();

    while (true) {
        // if update of co-processor failed then restart while loop
        if (not request_distance_data()) continue;

        gfx_term_clear();
        display_speed();

        distance_detection();

        switch (get_current_state()) {
            case DEAD_END:
                display_state("dead end detected");
                // reversing until left or right is free then turn around 180°
                leave_dead_end();
                break;
            case ALLEY:
                display_state("alley detected");
                // adjust and drive straight ahead
                drive_in_alley();
                break;
            case OBSTACLE_AHEAD:
                display_state("obstacle ahead");
                // decide for direction with best sensor values
                avoid_obstacle_ahead();
                break;
            case OBSTACLE_LEFT_AHEAD:
                display_state("obstacle left ahead");
                // adjust slightly to the right
                turn_right();
                break;
            case OBSTACLE_RIGHT_AHEAD:
                display_state("obstacle right ahead");
                // adjust slightly to the left
                turn_left();
                break;
            default:
                display_state("free");
                // default = nibo is free
                copro_setSpeed(20, 20);
                break;
        }
        delay(50);
    }
    return 0;
}
#pragma clang diagnostic pop

/**
 * Function to print the current state on the nibo's display.
 * @param current_state the current state the nibo is in that will be printed on the display
 */
void display_state(char *current_state) {
    char text[25];
    gfx_move(0, 10);
    sprintf(text, "%s", current_state);
    gfx_print_text(text);
}


/**
 * Function to print the current speed (in ticks) of the nibo's left and right wheel on the nibo's display.
 */
void display_speed() {
    char text[25];
    gfx_move(0, 0);
    sprintf(text, "Ticks: l=%3i r=%3i", copro_speed_l, copro_speed_r);
    gfx_print_text(text);
}


/**
 * Function waiting for the user to start the nibo by pressing S3.
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
    spi_init();
    leds_init();
    display_init(DISPLAY_TYPE_TXT);
    gfx_init();

    copro_ir_startMeasure();
}

/**
 * Function to determine if an obstacle is near (LEDs orange), very near (LEDs red) or not near at all (green).
 */
void distance_detection() {
    for (char location = 0; location < 5; location++) {
        uint16_t dist = copro_distance[location] / 128;
        if (dist > VERY_NEAR) {
            light_led_at_location(LEDS_RED, location);
        } else if (dist > NEAR) {
            light_led_at_location(LEDS_ORANGE, location);
        } else {
            light_led_at_location(LEDS_GREEN, location);
        }
    }
}

/**
 * Function to turn on an LED at a certain location.
 * @param color the color in which the LED should shine
 * @param location location of a distance sensor for which to turn on corresponding LED
 */
void light_led_at_location(uint8_t color, int location) {
    switch (location) {
        case DS_LEFT:
            leds_set_status(color, LEDS_LEFT);
            break;
        case DS_FRONT_LEFT:
            leds_set_status(color, LEDS_FRONT_LEFT);
            break;
        case DS_FRONT:
            leds_set_status(color, LEDS_FRONT_FRONT_LEFT);
            leds_set_status(color, LEDS_FRONT_FRONT_RIGHT);
            break;
        case DS_FRONT_RIGHT:
            leds_set_status(color, LEDS_FRONT_RIGHT);
            break;
        case DS_RIGHT:
            leds_set_status(color, LEDS_RIGHT);
            break;
        default:;
    }
}