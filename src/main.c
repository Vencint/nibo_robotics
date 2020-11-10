#include <nibo/niboconfig.h>
#include <nibo/leds.h>
#include <nibo/delay.h>
#include <avr/interrupt.h>
#include <nibo/pwm.h>

int main() {
    sei();
    leds_init();
    pwm_init();
    int brightness = 1024;
    char dir = 0;
    while(1) {
        for(int i = 0; i < 8; i++){
            leds_set_status(LEDS_ORANGE, i);
            delay(100);
            leds_set_status(LEDS_RED, i);
            leds_set_status_intensity(brightness);
        }
        if(dir){
            brightness *= 4;
        }else{
            brightness /= 4;
        }
        if(brightness <= 2){
            dir = 1;
        }else if(brightness >= 1024){
            dir = 0;
        }
    }
    return 0;
}
