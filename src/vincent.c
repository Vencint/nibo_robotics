//
// Created by Patrick Schlesinger on 11.11.20.
//

#include <stdlib.h>

#ifndef vincent
#define vincent

/*
enum distance_sensors {
    DS_LEFT = 4,
    DS_FRONT_LEFT = 3,
    DS_FRONT = 2,
    DS_FRONT_RIGHT = 1,
    DS_RIGHT = 0
};
*/

void drive_in_alley() {
    if (abs(copro_distance[4] - copro_distance[0]) / 128 < 0x00a0 / 2) {

        // driving straight through the middle of the alley
        if (abs(copro_distance[3] - copro_distance[1]) / 128 < 0x00a0) {
            copro_setSpeed(20, 20);
            gfx_move(0, 10);
            gfx_set_proportional(1);
            gfx_print_text("20 20");
        } else {
            // copro is in middle but doesn't drive straight
            if (copro_distance[3] > copro_distance[1]) {
                // oriented to the left so adjust slightly to the right
                copro_setSpeed(18, 15);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("15 18");
            } else {
                // oriented to the right so adjust slightly to the left
                copro_setSpeed(15, 18);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("18 15");
            }
        }

    } else {

        // not straight in the middle of the alley
        if (copro_distance[4] > copro_distance[0]) {

            if (copro_distance[3] > copro_distance[1]) {
                copro_setSpeed(18, 10);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("10 18");
            } else {
                copro_setSpeed(17, 20);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("20 17");
            }

        } else {

            if (copro_distance[1] > copro_distance[3]) {
                copro_setSpeed(10, 18);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("18 10");
            } else {
                copro_setSpeed(20, 17);
                gfx_move(0, 10);
                gfx_set_proportional(1);
                gfx_print_text("17 20");
            }

        }

    }
}

#endif
