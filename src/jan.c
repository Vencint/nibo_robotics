/**
 * @file main.c
 * @author Jan Schorowski
 * @date 13.11.20
 * @brief This c file contains methods written by Jan Schorowski.
 * This includes the handling of the nibo when it detects a dead end.
 */

#include <utils.h>

/**
 *
 * @param ds_current current distance sensor value
 * @param ds_previous distance sensor value of the previous loop
 * @return the speed of the wheel
 */
uint16_t determine_steering(const uint16_t ds_current, const uint16_t ds_previous) {
    uint16_t difference = 0;

    // determine how much steering is necessary
    difference = ds_current - ds_previous;
    if (difference < 5) {
        return -9;
    } else if (difference < 10) {
        return -8;
    } else if (difference < 15) {
        return -7;
    } else if (difference < 20) {
        return -6;
    } else {
        return -5;
    }
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
                    speed_right_wheel = -9;
                } else if (difference < 10) {
                    speed_right_wheel = -8;
                } else if (difference < 15) {
                    speed_right_wheel = -7;
                } else if (difference < 20) {
                    speed_right_wheel = -5;
                } else {
                    speed_right_wheel = -3;
                }
            } else {
                // reverse in a straight line
                speed_right_wheel = -10;
            }

            /* if the front-left side of the nibo is very close to an object, turn with lesser radius by reversing with
             * the left wheel more quickly */
            if (ds_current[DS_RIGHT] > 230) {
                speed_left_wheel = -12;
            } else {
                speed_left_wheel = -10;
            }
        } else if (ds_current[DS_RIGHT] > ds_previous[DS_RIGHT]) {
            // Nibo is reversing to the right, steer to the left

            difference = ds_current - ds_previous;
            if (difference > 1) {
                if (difference < 5) {
                    speed_left_wheel = -9;
                } else if (difference < 10) {
                    speed_left_wheel = -8;
                } else if (difference < 15) {
                    speed_left_wheel = -7;
                } else if (difference < 20) {
                    speed_left_wheel = -5;
                } else {
                    speed_left_wheel = -3;
                }
            } else {
                // reverse in a straight line
                speed_left_wheel = -10;
            }

            /* if the front-right side of the nibo is very close to an object, turn with lesser radius by reversing with
             * the right wheel more quickly */
            if (ds_current[DS_RIGHT] > 230) {
                speed_right_wheel = -12;
            } else {
                speed_right_wheel = -10;
            }
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
        delay(500);
    }

    return 0;
}

