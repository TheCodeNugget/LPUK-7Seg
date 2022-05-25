#include <msp430g2553.h>

/**
 * main.c
 */

// The order of the bits for 7-Seg display is 0bABCDEFGP
volatile unsigned int index[16] = { // 7-Seg Mapping
    0b11111100, // 0
    0b01100000, // 1
    0b11011010, // 2
    0b11110010, // 3
    0b01100110, // 4
    0b10110110, // 5
    0b10111110, // 6
    0b11100000, // 7
    0b11111110, // 8
    0b11110110, // 9
    0b11101110, // A
    0b00111110, // b <- No capital B in 7Seg
    0b10011100, // C
    0b01111010, // d <- No Capital D in 7Seg
    0b10011110, // E
    0b10001110  // F
    // PS: More Letters are Possible with 7Seg, Me Me lazy
};

int main(void) { // TODO: Remove This Comment

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    DCOCTL = 0;                 // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;     // Set range
    DCOCTL = CALDCO_16MHZ;      // 1 cycle = 1s/16MHz = 625ns

    P2DIR |= BIT0 + BIT3 + BIT4;   // Set Pin-2.(0/3/4) to output for pin 14/11/12 of IC102

    int n=0, count = 0;
    while(1){ // Hex Timer
        for(n=0; n<1000;n++){
            hex4Digit(n,100);
        }
    }
}

//delay in msec
void hex4Digit(int number, int delay){ // Hex to 7Seg

    int k = 0 , first, second, third, last;

    // Decimal To Hex Conversion
    int d4 = number % 16;
    int d3 = (number / (16)) % 16;
    int d2 = (number / (16 * 16)) % 16;
    int d1 = (number / (16 * 16 * 16)) % 16;

    first = index[d1];
    second = index[d2];
    third = index[d3];
    last = index[d4];

    for(k=0;k<delay*0.4;k++){
        write2_7segment(first,4);
        __delay_cycles(10000);
        write2_7segment(second,3);
        __delay_cycles(10000);
        write2_7segment(third,2);
        __delay_cycles(10000);
        write2_7segment(last,1);
        __delay_cycles(10000);
    }
}

void write2_7segment(int data, int index){
    volatile unsigned int sck=0x0008; // Pin-2.3
    volatile unsigned int rck=0x0010; // Pin-2.4
    volatile unsigned int bit=0; // Bit Holder for shifting
    volatile unsigned int i=0;


    for(i=0;i<4;i++){ // Fill IC102 to Write into IC104
        P2OUT=0;
        __delay_cycles(1);
        P2OUT=sck;
        __delay_cycles(1);
        P2OUT=0;
        __delay_cycles(1);

    }
    for(i=0;i<4;i++){ // Write Into IC104 for 7-Seg Power

        if(index==1){ //Write 1 to Operated Digit C
            P2OUT=1;
            __delay_cycles(1);
            P2OUT=sck|1;
            __delay_cycles(1);
            P2OUT=1;
            __delay_cycles(1);
        }else{ //Write 0 to Unoperated Digit's C
            P2OUT=0;
            __delay_cycles(1);
             P2OUT=sck|0;
            __delay_cycles(1);
            P2OUT=0;
            __delay_cycles(1);
        }
        index=index-1;
    }

    for(i=0;i<8;i++){ // Rewrite IC102 With the Digit Data
        bit=data & 1; // Grab 1st Bit of Data
        __delay_cycles(1);
        P2OUT=bit; // Write bit Value to Output
        P2OUT=sck|bit; //SCK is on, Write to Register
        __delay_cycles(1);
        P2OUT=bit; //SCK is off, Save Register Value
        __delay_cycles(1);
        data=data >> 1; // Bit-Shift Right by 1 bit for next cycle
    }

    P2OUT=rck; //Register Done Send Data to 7-Seg
    __delay_cycles(1);

}
