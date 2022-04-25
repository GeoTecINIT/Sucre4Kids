// Example usage for U8g2 library by me.

#include "U8g2.h"

// Initialize objects from the lib
U8g2 u8g2;

void setup() {
    // Call functions on initialized library objects that require hardware
    u8g2.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    u8g2.process();
}
