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


/**
 * This method tries to manoeuvre the nibo out of a dead end.
 * The nibo will drive backwards and will try to stay in the center of the dead end. When no more obstacle on the left
 * or right side is detected then the nibo will turn towards the free side and can drive freely again.
 * @return 1 if the nibo managed to leave the dead end, 0 if the nibo is still in the dead end (so far)
 * @author Jan Schorowski
 */
int leave_dead_end() {
    // init arrays which contain the distance sensor values of the current and previous while loop
    uint16_t ds_current[5];
    uint16_t ds_previous[5];
    int16_t speed_left_wheel = -10;
    int16_t speed_right_wheel = -10;

    uint16_t difference = 0;

    // assign current distance values to the array of "previous" distance values for now
    for (int i = 0; i < 5; ++i) {
        ds_previous[i] = copro_distance[i] / 256;
    }

    // start reversing
    copro_setSpeed(speed_left_wheel, speed_right_wheel);
    delay(300);

    // Enter loop for manoeuvring out of the dead end
    while (true) {
        // update distance sensor data
        copro_update();
        for (int i = 0; i < 5; ++i) {
            ds_current[i] = copro_distance[i] / 256;
        }

        // Check for obstacles on the left and right hand side of the nibo
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
        if (ds_current[DS_LEFT] > ds_previous[DS_LEFT]) {
            // Nibo is reversing to the left, steer to the right
            difference = ds_current - ds_previous;
            if (difference > 1) {
                if (difference < 5) {
                    speed_right_wheel = -8; // -9
                } else if (difference < 10) {
                    speed_right_wheel = -6; // -8
                } /*else if (difference < 15) {
                    speed_right_wheel = -7;
                } else if (difference < 20) {
                    speed_right_wheel = -5;
                } else {
                    speed_right_wheel = -3;
                }*/
            } else {
                // reverse in a straight line
                speed_right_wheel = -10;
            }

            /* if the front-left side of the nibo is very close to an object, turn with lesser radius by reversing with
             * the left wheel more quickly */
            /*if (ds_current[DS_RIGHT] > 230) {
                speed_left_wheel = -12;
            } else {
                speed_left_wheel = -10;
            }*/
        } else if (ds_current[DS_RIGHT] > ds_previous[DS_RIGHT]) {
            // Nibo is reversing to the right, steer to the left

            difference = ds_current - ds_previous;
            if (difference > 1) {
                if (difference < 5) {
                    speed_left_wheel = -8; // -9
                } else if (difference < 10) {
                    speed_left_wheel = -8; // -8
                } /*else if (difference < 15) {
                    speed_left_wheel = -7;
                } else if (difference < 20) {
                    speed_left_wheel = -5;
                } else {
                    speed_left_wheel = -3;
                }*/
            } else {
                // reverse in a straight line
                speed_left_wheel = -10;
            }

            /* if the front-right side of the nibo is very close to an object, turn with lesser radius by reversing with
             * the right wheel more quickly */
            /*if (ds_current[DS_RIGHT] > 230) {
                speed_right_wheel = -12;
            } else {
                speed_right_wheel = -10;
            }*/
        } else {
            // reverse in a straight line
            speed_left_wheel = -10;
        }

        /* copy current distance sensor values to the array which holds distance sensor values of the previous run
         * (for the next loop) */
        for (int i = 0; i < 5; ++i) {
            ds_previous[i] = ds_current[i];
        }

        // finally set the speed and have a delay to not overwhelm the motor with commands
        copro_setSpeed(speed_left_wheel, speed_right_wheel);
        delay(50);
    }

    return 0;
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
