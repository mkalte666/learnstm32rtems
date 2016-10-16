#include <stdio.h>

#include <bsp.h>
#include <bsp/io.h>

#include "micromod.h"
#include "media/music.h"


#define STEP_PIN STM32F4_GPIO_PIN(1,1)
#define DIR_PIN  STM32F4_GPIO_PIN(1,0) 


// micromod config 
#define SPEED_FACTOR 0.5
#define SAMPLE_RATE 8000
#define SAMPLE_RATE_MICROMOD (SAMPLE_RATE/SPEED_FACTOR)
#define NS_PER_SAMPLE (1000000000/SAMPLE_RATE)

// floppy config 
#define DRIVE_CHANNEL_OFFSET 0
#define DRIVE_PORT 4
#define NUM_DRIVES 4
#define MAX_STEPS_PER_DIR 78

typedef struct 
{
    uint64_t freqDivider;
    uint64_t freqClock;
    uint64_t freqTime;
    long dirCounter;
    long dDir;
    uint8_t dirPin;
    uint8_t stpPin;
} Drive_t;

stm32f4_gpio_config led3config =
{
    .fields={
        .pin_first = STM32F4_GPIO_PIN(DRIVE_PORT, 0),
        .pin_last = STM32F4_GPIO_PIN(DRIVE_PORT, NUM_DRIVES*2-1),
        .mode = STM32F4_GPIO_MODE_OUTPUT,
        .otype = STM32F4_GPIO_OTYPE_PUSH_PULL,
        .ospeed = STM32F4_GPIO_OSPEED_2_MHZ,
        .pupd = STM32F4_GPIO_PULL_DOWN,
        .output = 1,
        .af = 0
    }
};

#define LED_INIT() stm32f4_gpio_set_config(&led3config)

rtems_task Init(
    rtems_task_argument argument
)
{
    rtems_status_code status;

    LED_INIT();
    
    // innit module stuff
    (void)micromod_initialise(music, SAMPLE_RATE_MICROMOD);     
    
    struct timespec t;
    rtems_clock_get_uptime(&t);
    uint64_t sampleClock = t.tv_sec*1000000000 + t.tv_nsec;

    // init the drives 
    Drive_t drives[NUM_DRIVES];
    for (int i = 0; i < NUM_DRIVES; i++) {
        drives[i].freqDivider = 64;
        drives[i].freqClock = sampleClock;
        drives[i].freqTime = 0;
        drives[i].dDir = 1;
        drives[i].dirCounter = 0;
        drives[i].dirPin = i*2;
        drives[i].stpPin = i*2+1;
    }
    
    while (1) {
        rtems_clock_get_uptime(&t);
        uint64_t curtime = t.tv_sec*1000000000 + t.tv_nsec;
        
        if (curtime - sampleClock > NS_PER_SAMPLE) {
            (void)micromod_get_audio(NULL,1);
            
            for (int i = 0; i < NUM_DRIVES; i++) {
                long freq = micromod_get_channel_freq(i+DRIVE_CHANNEL_OFFSET);
                if (freq == 0) {
                    drives[i].freqTime = 0;
                } else {
                    drives[i].freqTime = 1000000000/(freq/(drives[i].freqDivider));
                }
            }
            
            sampleClock = curtime;
        }

        // do the stepping
        for (int i = 0; i < NUM_DRIVES; i++) {
            Drive_t drive = drives[i];
            if (drive.freqTime > 0 && curtime-drive.freqClock > drive.freqTime) {
                stm32f4_gpio_set_output(STM32F4_GPIO_PIN(DRIVE_PORT,drive.stpPin), 1);
                stm32f4_gpio_set_output(STM32F4_GPIO_PIN(DRIVE_PORT,drive.stpPin), 0);
                drive.freqClock = curtime;

                drive.dirCounter += drive.dDir;
                if (drive.dirCounter <= 0 || drive.dirCounter >= MAX_STEPS_PER_DIR) {
                    drive.dDir *= -1;
                    stm32f4_gpio_set_output(STM32F4_GPIO_PIN(DRIVE_PORT,drive.dirPin), drive.dDir==1);      
                }

                drives[i] = drive;
            }
        }
    }

    status = rtems_task_delete( RTEMS_SELF );
}


/**************** START OF CONFIGURATION INFORMATION ****************/

//#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/
