/**
@file patrick.c
@author Daniel Wittekind (Tutorials), Patrick Schlesinger
@date 13.11.2020
@brief  Funktionen zum Abbiegen bei Hindernissen, welche sich links,
        rechts oder vor dem Nibo befinden
 */

// Standard Includes fuer das Funktionieren des Nibo2
#include <nibo/niboconfig.h>
#include <nibo/iodefs.h>
#include <nibo/bot.h>

// Bibliothek zum Ansteuern des Display
#include <nibo/display.h>

// Bibliothek mit Grafikfunktionen
#include <nibo/gfx.h>

// Wartefunktionen
#include <nibo/delay.h>

/*
 * Die Distanzsensoren sowie die Motoren werden vom Coprozessor gesteuert.
 * Daher sind Bibliotheken fuer die Kommunikation mit diesem noetig.
 * Bibliothek fuer Coprozessor-spezifische Funktionen
 */
#include <nibo/copro.h>

// Kommunikationsfunktionen fuer die SPI Schnittstelle zum Coprozessor
#include <nibo/spi.h>

// Interrupts fuer die Kommunikation mit dem Coprozessor
#include <avr/interrupt.h>

// Ein- und Ausgabefunktionen
#include <stdio.h>


void obstacle_left(){
    copro_setSpeed(20,10);
    delay(300);
    copro_setSpeed(20,20);
}

void obstacle_right(){
    copro_setSpeed(10,20);
    delay(300);
    copro_setSpeed(20,20);
}

void obstacle_ahead(){
    copro_setSpeed(-20,20);
    delay(500);
    copro_setSpeed(20,20);
}