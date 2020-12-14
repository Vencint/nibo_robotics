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
#include <stdio.h>

#include "jan.h"
#include "patrick.h"
#include "utils.h"
#include "vincent.h"


void init();

uint8_t request_distance_data();

void start_nibo();


void display_speed();

void display_state(char *current_state);

void distance_detection();

void light_led_at_location(uint8_t color, int location);


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

        gfx_term_clear();
        display_speed();

        distance_detection();

        switch (get_current_state()) {
            case DEAD_END:
                display_state("dead end detected");
                // Rückwärts heraus fahren, bis rechts oder links frei, danach 180 Grad Drehung
                leave_dead_end();
                break;
            case ALLEY:
                display_state("alley detected");
                // Justieren und geradeaus fahren
                drive_in_alley();
                break;
            case OBSTACLE_AHEAD:
                display_state("obstacle ahead");
                // Sensor, der besten Wert liefert finden und in diese Richtung fahren
                obstacle_ahead();
                break;
            case OBSTACLE_LEFT_AHEAD:
                display_state("obstacle left ahead");
                // Darauf achten, dass er nicht näher kommt
                obstacle_left();
                break;
            case OBSTACLE_RIGHT_AHEAD:
                display_state("obstacle right ahead");
                obstacle_right();
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

void display_state(char *current_state) {
    char text[25];
    gfx_move(0, 10);
    sprintf(text, "%s", current_state);
    gfx_print_text(text);
}


/**
 * This function shows the current speed (in ticks) of the left and right wheel using the display.
 */
void display_speed() {
    char text[25];
    gfx_move(0, 0);
    sprintf(text, "Ticks: l=%3i r=%3i", copro_speed_l, copro_speed_r);
    gfx_print_text(text);
}


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
    spi_init();
    leds_init();
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
        // Clear text currently shown on the display
        gfx_fill(0);

        gfx_move(10, 10);
        gfx_set_proportional(1);
        gfx_print_text("COPRO Error:");
        gfx_move(10, 30);
        gfx_print_text("can't update distance sensor data");
    }
    return update;
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
 * @param location location of a distance sensor for which to turn corresponding LED on
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