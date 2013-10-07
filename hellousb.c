#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "uart.h"
#include <stdio.h>
#include "oc.h"
#include "timer.h"

#define HELLO       0   // Vendor request that prints "Hello World!"
#define SET_VALS    1   // Vendor request that receives 2 unsigned integer values
#define GET_VALS    2   // Vendor request that returns 2 unsigned integer values
#define PRINT_VALS  3   // Vendor request that prints 2 unsigned integer values 
#define PING_OUT    4   // Vendor request that prints 2 unsigned integer values 


// #define RX_THRESH 0.00073
#define RX_THRESH 0.0
#define TX_PERIOD 0.005
#define RX_TMOUT 0.070

uint16_t val1, val2;
float TOF;


//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

void pwm_clear(_TIMER *timer){
    // printf("timer disabled\n");
    oc_free(&oc3);
    timer_stop(timer);
}

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case HELLO:
            printf("Hello World!\n");
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_VALS:
            val1 = USB_setup.wValue.w;
            val2 = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_VALS:;
            temp.w = val1;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = val2;
            // BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;            
        case PRINT_VALS:
            printf("val1 = %u, val2 = %u\n", val1,val2);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case PING_OUT:
            oc_pwm(&oc3, &D[2], NULL, 40000, 32768);
            // void timer_after(_TIMER *self, float delay, uint16_t num_times,void (*callback)(_TIMER *self))
            timer_after(&timer3,TX_PERIOD,1,pwm_clear);
            timer_setPeriod(&timer4,RX_TMOUT); //change period later
            TOF=0.0;
            printf("timer period: %f\n",timer_period(&timer4));
            timer_start(&timer4);
            uint16_t maxTicks = 65535;
            uint16_t minTicks = ((TX_PERIOD)*65535/RX_TMOUT);
            while(TOF< maxTicks){
                TOF=timer_read(&timer4);
                printf("tof: %u\n",TOF);
                if(TOF<minTicks){
                    // printf("not within threshold range: %f \n", TOF);
                    continue;
                }
                if(pin_read(&D[3])==1){
                    // printf('signal read %f',.00004);
                    // TOF=timer_time(&timer4);
                    printf("pin 3 high\n");
                    break;
                }
            }

            printf("TOF : %u    \n", TOF);
            printf("\n\n\n\n");

            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}


void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_uart();
    init_pin();
    init_timer();
    init_oc();
    oc_servo(&oc1, &D[0], &timer1, 0.02, 8e-4, 2.2e-3, 32767);
    oc_servo(&oc2, &D[1], &timer2, 0.02, 8e-4, 2.2e-3, 32767);

    val1 = 32767;
    val2 = 32767;

    pin_digitalIn(&D[3]);


    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
        
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
        pin_write(&D[0],val1);
        pin_write(&D[1],val2);
        
    }
}

