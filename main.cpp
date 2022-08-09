#include "mbed.h"

// main() runs in its own thread in the OS
int main()
{
    while (true) {
        printf("Hello via SWO!\n");
        wait_us(1000000);        
    }
}