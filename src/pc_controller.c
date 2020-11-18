/**
 * This class contains the user interface for remote-controlling the nibo.
 * @author Jan Schorowski
 * @version 1.0
 * @date 11/2020
 */

#include <stdlib.h>
#include <stdio.h>

/**
 * The main method, which contains the "endless" loop for the user interface.
 * @return 0
 */
int main() {
    // variables for user input
    char input_buffer[20];
    char *p_input_buffer;
    int user_input;

    // endlessly loop until the user enters 0
    while (1) {
        printf("------------------------------------------------------------------------------"
               "\nPlease enter one of the following numbers to access their assigned features.\n"
               "1\tTake (remote-)control of the nibo\n"
               "2\tChange the behaviour of the nibo\n"
               "0\tExit the program\n");

        // Get user input with error handling
        if (fgets(input_buffer, sizeof(input_buffer), stdin) != NULL) {
            // user has entered something
            user_input = (int) strtol(input_buffer, &p_input_buffer, 10);

            /*
             *  If the first character of the buffer is \n, the user pressed [Enter] with entering any text at all,
             *  which is invalid.
             *  The pointer p_input_buffer now points to the first invalid character after the number.
             *  -> If this character is \0 it means we reached the end of the array  => valid input
             *  -> If this character is \n it also means we reached the end of the input successfully.
             *  -> If this character is anything else, it means there was some additional characters entered after the
             *      number, which is invalid.
             *  If invalid input has been detected/received, let the user know and continue looping.
             *
             *  Source for the idea and explanation: https://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1043372399&id=1043284385
             */
            if (input_buffer[0] != '\n' && (*p_input_buffer == '\n' || *p_input_buffer == '\0')) {
                // exit the program if user enters a 0
                if (user_input == 0) {
                    printf("You have entered a 0 -> stopping the program now.\n");
                    break;
                }

                // evaluate user input, execute methods according to it
                switch (user_input) {
                    case 1:
                        // remote control the nibo - maybe check if the currently selected mode allows that first
                        printf("Entering function to remote-control the nibo.\n");
                        break;
                    case 2:
                        // change current control mode of the nibo
                        printf("Entering function to change the current behaviour of the nibo.\n");
                        break;
                    default:
                        printf("There's no feature assigned to the entered number.\n");
                }
            } else {
                printf("Invalid input received. Please try again.\n");
            }
        }
    }

    return 0;
}