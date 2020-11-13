/**
 * @file main.c
 * @author Jan Schorowski
 * @date 13.11.20
 * @brief This c file contains methods written by Jan Schorowski.
 * This includes the handling of the nibo when it detects a dead end.
 */


const uint16_t D_S_LEFT = 4;
const uint16_t D_S_FRONT_LEFT = 3;
const uint16_t D_S_FRONT_RIGHT = 1;
const uint16_t D_S_RIGHT = 0;

/**
 * This method tries to manoeuvre the nibo out of a dead end.
 * @return 1 if the nibo managed to leave the dead end, 0 if the nibo is still in the dead end (so far)
 */
int leave_dead_end() {

    /*
     * The plan, as of right now, is the following:
     * Basically:
     * - Go backwards, until one of the distance sensors on the far left or far right don't detect an object
     *       anymore, and steer in the direction where no object was found.
     * Problems to be solved:
     * - Need to watch all distance sensor values, since the nibo might not move in a straight line and
     *       its path regularly has to get corrected.
     * - Find a way for main() to keep calling this function instead of any other one when the front sensor doesn't
     *       detect an object.
     * - Evaluate the value of the distance sensor in the back of the nibo as well -> there might be no way out of
     *       the dead end.
     *
     * Plan: Get data from all distance sensors.
     *       Compare the values to those gathered in the previous run.
     *       Check if values of left & right DS have changed a lot, counter-steer a little if necessary
     *       reverse
     */

    // arrays which contain the distance sensor values of the current and previous while loop
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
        if (ds_current[D_S_LEFT] < 50 and ds_current[D_S_FRONT_LEFT] < 50) {
            // there's no obstacle to the left of the nibo, steer left and leave this function
            copro_setSpeed(20, 0);
            delay(500);
            break;
        } else if (ds_current[D_S_RIGHT] < 50 and ds_current[D_S_FRONT_RIGHT] < 50) {
            // there's no obstacle to the right of the nibo, steer right and leave this function
            copro_setSpeed(0, 20);
            delay(500);
            break;
        }

        // compare current distance sensor values to the values of the previous loop
        if (ds_current[D_S_LEFT] < ds_previous[D_S_LEFT] and ds_current[D_S_RIGHT] > ds_previous[D_S_RIGHT]) {
            // Nibo is reversing to the left, steer a little to the right
            speed_left_wheel = -11;
            speed_right_wheel = -9;
        } else if (ds_current[D_S_RIGHT] < ds_previous[D_S_RIGHT] and ds_current[D_S_LEFT] > ds_previous[D_S_LEFT]) {
            // Nibo is reversing to the right, steer a little to the left
            speed_left_wheel = -9;
            speed_right_wheel = -11;
        } else {
            // Try to reverse in a straight line
            speed_left_wheel = -10;
            speed_right_wheel = -10;
        }

        /* copy current distance sensor values to the array which holds distance sensor values of the previous run (for
         the next loop) */
        for (int i = 0; i < 5; ++i) {
            ds_previous[i] = ds_current[i];
        }

        // finally set the speed and have a delay to not overwhelm the motor with commands
        copro_setSpeed(speed_left_wheel, speed_right_wheel);
        delay(500);
    }
    return 0;
}

