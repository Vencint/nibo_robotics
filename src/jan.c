/**
 * @file main.c
 * @author Jan Schorowski
 * @date 13.11.20
 * @brief This c file contains methods written by Jan Schorowski.
 * This includes the handling of the nibo when it detects a dead end.
 */

#include <utils.h>

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
            copro_setSpeed(20, 0);
            delay(500);
            break;
        } else if (ds_current[DS_RIGHT] < 50 and ds_current[DS_FRONT_RIGHT] < 50) {
            // there's no obstacle to the right of the nibo, steer right and leave this function
            copro_setSpeed(0, 20);
            delay(500);
            break;
        }

        // compare current distance sensor values to the values of the previous loop
        if (ds_current[DS_LEFT] < ds_previous[DS_LEFT] and ds_current[DS_RIGHT] > ds_previous[DS_RIGHT]) {
            // Nibo is reversing to the left, steer a little to the right
            speed_left_wheel = -11;
            speed_right_wheel = -9;
        } else if (ds_current[DS_RIGHT] < ds_previous[DS_RIGHT] and ds_current[DS_LEFT] > ds_previous[DS_LEFT]) {
            // Nibo is reversing to the right, steer a little to the left
            speed_left_wheel = -9;
            speed_right_wheel = -11;
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

