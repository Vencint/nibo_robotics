/**
 * @file utils.c
 * @author Vincent Luedtke
 * @date 14.11.20
 * @brief explain stuff here
 */

#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include <nibo/niboconfig.h>
#include <nibo/delay.h>
#include <nibo/bot.h>
#include <nibo/copro.h>

#include "utils.h"

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