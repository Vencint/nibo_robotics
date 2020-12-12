//
// Created by Patrick Schlesinger on 11.11.20.
//

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

#include "utils.h"


/**
 * Function to drive through alley or tunnel.
 */
void drive_in_alley() {
    if (abs(copro_distance[4] - copro_distance[0]) / 128 < 0x00a0 / 2) {

        // driving straight through the middle of the alley
        if (abs(copro_distance[3] - copro_distance[1]) / 128 < 0x00a0) {
            copro_setSpeed(20, 20);
        } else {
            // copro is in middle but doesn't drive straight
            if (copro_distance[3] > copro_distance[1]) {
                // oriented to the left so adjust slightly to the right
                copro_setSpeed(18, 15);
            } else {
                // oriented to the right so adjust slightly to the left
                copro_setSpeed(15, 18);
            }
        }

    } else {

        // not straight in the middle of the alley
        if (copro_distance[4] > copro_distance[0]) {

            if (copro_distance[3] > copro_distance[1]) {
                copro_setSpeed(18, 10);
            } else {
                copro_setSpeed(17, 20);
            }

        } else {

            if (copro_distance[1] > copro_distance[3]) {
                copro_setSpeed(10, 18);
            } else {
                copro_setSpeed(20, 17);
            }

        }

    }
}
