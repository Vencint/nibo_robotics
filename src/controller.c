/**
 * @file controller.c
 * @author Vincent Luedtke, Patrick Schlesinger, Jan Schorowski
 * @date 14.11.20
 * @brief File containing functions to control the nibo's behaviour.
 *
 * Each of these functions inside this file handle a certain state which can be:
 * * nibo is in tunnel
 * * nibo is in alley
 * * nibo detected an obstacle ahead
 * * nibo detected obstacle right ahead
 * * nibo detected obstacle left ahead
 */

#include <nibo/niboconfig.h>
#include <nibo/delay.h>
#include <nibo/display.h>
#include <nibo/copro.h>
#include <stdbool.h>
#include <iso646.h>
#include <stdlib.h>

#include "utils.h"
#include "controller.h"


/**
 * This method tries to manoeuvre the nibo out of a dead end.
 * The nibo tries to make a turn of a little less than 180°. Afterwards, it'll check if there's an obstacle ahead and if
 * so, keep turning a little further, until it doesn't detect an obstacle anymore.
 * @author Jan Schorowski
 */
void leave_dead_end() {
    // turn on the spot
    copro_setSpeed(20, -30);
    delay(800);

    while (true) {
        copro_update();

        // if there's an obstacle ahead...
        if (is_near(DS_FRONT, NEAR)) {
            // yes: slowly turn further
            copro_setSpeed(10, -10);
        } else {
            // no: stop the nibo, break out of the loop
            copro_stop();
            break;
        }
        delay(1000);
    }
}


/**
 * Function to drive through alley or tunnel.
 * The nibo will try to stay in the center and will adjust if it diverges.
 * @author Vincent Luedtke
 */
void drive_in_alley() {
    if (abs(copro_distance[DS_LEFT] - copro_distance[DS_RIGHT]) / 128 < NEAR / 2) {
        if (abs(copro_distance[DS_FRONT_LEFT] - copro_distance[DS_FRONT_RIGHT]) / 128 < NEAR) {
            // driving straight through the middle of the alley
            copro_setSpeed(20, 20);
        } else {
            // copro is in middle but doesn't drive straight
            if (copro_distance[DS_FRONT_LEFT] > copro_distance[DS_FRONT_RIGHT]) {
                // oriented to the left so adjust slightly to the right
                copro_setSpeed(18, 15);
            } else {
                // oriented to the right so adjust slightly to the left
                copro_setSpeed(15, 18);
            }
        }
    } else {
        // not straight in the middle of the alley
        if (copro_distance[DS_LEFT] > copro_distance[DS_RIGHT]) {
            if (copro_distance[DS_FRONT_LEFT] > copro_distance[DS_FRONT_RIGHT]) {
                copro_setSpeed(18, 10);
            } else {
                copro_setSpeed(17, 20);
            }
        } else {
            if (copro_distance[DS_FRONT_RIGHT] > copro_distance[DS_FRONT_LEFT]) {
                copro_setSpeed(10, 18);
            } else {
                copro_setSpeed(20, 17);
            }
        }
    }
}


/**
 * Function to make nibo turn right.
 * @author Patrick Schlesinger
 */
void turn_right() {
    copro_setSpeed(22, 8);
}


/**
 * Function to make nibo turn left.
 * @author Patrick Schlesinger
 */
void turn_left() {
    copro_setSpeed(8, 22);
}


/**
 * Function to avoid obstacles ahead.
 * The nibo decides which way to turn depending on the data of the front left and front right distance sensors. Then it
 * will turn around until the front sensor does not detect an obstacle anymore.
 * @author Patrick Schlesinger
 */
void avoid_obstacle_ahead() {
    if (copro_distance[DS_FRONT_LEFT] >= copro_distance[DS_FRONT_RIGHT]) {
        while (is_near(copro_distance[DS_FRONT], NEAR)) {
            copro_stop();
            delay(100);
            copro_setSpeed(20, -20);
            delay(500);
            copro_update();
        }
    } else {
        while (is_near(copro_distance[DS_FRONT], NEAR)) {
            copro_stop();
            delay(100);
            copro_setSpeed(-20, 20);
            delay(500);
            copro_update();
        }
    }
}
