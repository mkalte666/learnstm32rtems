#include <stdio.h>

#include <bsp.h>
#include <bsp/io.h>

stm32f4_gpio_config led3config =
{
    .fields={
        .pin_first = STM32F4_GPIO_PIN(2, 1),
        .pin_last = STM32F4_GPIO_PIN(2, 1),
        .mode = STM32F4_GPIO_MODE_OUTPUT,
        .otype = STM32F4_GPIO_OTYPE_PUSH_PULL,
        .ospeed = STM32F4_GPIO_OSPEED_2_MHZ,
        .pupd = STM32F4_GPIO_NO_PULL,
        .output = 1,
        .af = 0
    }
};

#define LED_INIT() stm32f4_gpio_set_config(&led3config)
#define LED_ON() stm32f4_gpio_set_output(STM32F4_GPIO_PIN(2,1), 1)
#define LED_OFF() stm32f4_gpio_set_output(STM32F4_GPIO_PIN(2,1), 0)

rtems_task Init(
          rtems_task_argument argument
        )
{
      rtems_status_code status;

        puts( "\n\n*** LED BLINKER -- task wake after ***" );

          LED_INIT();

            while (1) {

                    (void) rtems_task_wake_after( rtems_clock_get_ticks_per_second() / 50 );
                        LED_OFF();
                            (void) rtems_task_wake_after( rtems_clock_get_ticks_per_second() /50 );
                                LED_ON();

                                  }

              status = rtems_task_delete( RTEMS_SELF );
}


/**************** START OF CONFIGURATION INFORMATION ****************/

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_MAXIMUM_TASKS             1

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT
#include <rtems/confdefs.h>

/****************  END OF CONFIGURATION INFORMATION  ****************/
