/**
 * @file utils.c
 * @author Vincent Luedtke, Patrick Schlesinger, Jan Schorowski
 * @date 14.11.20
 * @brief File containing useful functions that are needed by the entired application.
 */

#include <stdint.h>
#include <stdbool.h>
#include <iso646.h>
#include <nibo/niboconfig.h>
#include <nibo/delay.h>
#include <nibo/bot.h>
#include <nibo/copro.h>
#include <nibo/gfx.h>

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