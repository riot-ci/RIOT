#include <stdio.h>
#include <string.h>

/* RIOT includes */
#include <log.h>
#include <motor_driver.h>
#include <shell.h>
#include <shell_commands.h>
#include <thread.h>
#include <xtimer.h>

/* set interval to 20 milli-second */
#define INTERVAL (3000 * US_PER_MS)

#define MOTOR_0_ID  0
#define MOTOR_1_ID  1

#ifdef MOTOR_DRIVER_NUMOF
char motion_control_thread_stack[THREAD_STACKSIZE_DEFAULT];

void motors_control(int32_t duty_cycle)
{
    char str[6];

    if (duty_cycle >= 0) {
        strncpy(str, "CW", 3);
    }
    else {
        strncpy(str, "CCW", 4);
    }

    printf("Duty cycle = %" PRId32 "   Direction = %s\n", duty_cycle, str);

    if (motor_set(MOTOR_DRIVER_DEV(0), MOTOR_0_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu32 "\n", \
               (uint32_t)MOTOR_0_ID);
    }
    if (motor_set(MOTOR_DRIVER_DEV(0), MOTOR_1_ID, duty_cycle)) {
        printf("Cannot set PWM duty cycle for motor %" PRIu32 "\n", \
               (uint32_t)MOTOR_1_ID);
    }
}

void motors_brake(void)
{
    puts("Brake motors !!!");

    if (motor_brake(MOTOR_DRIVER_DEV(0), MOTOR_0_ID)) {
        printf("Cannot brake motor %" PRIu32 "\n", (uint32_t)MOTOR_0_ID);
    }
    if (motor_brake(MOTOR_DRIVER_DEV(0), MOTOR_1_ID)) {
        printf("Cannot brake motor %" PRIu32 "\n", (uint32_t)MOTOR_1_ID);
    }
}

void *motion_control_thread(void *arg)
{
    int8_t dir = 1;
    int ret = 0;
    xtimer_ticks32_t last_wakeup /*, start*/;
    int32_t pwm_res = motor_driver_config[MOTOR_DRIVER_DEV(0)].pwm_resolution;

    (void) arg;

    ret = motor_driver_init(MOTOR_DRIVER_DEV(0));
    if (ret) {
        LOG_ERROR("motor_driver_init failed with error code %d\n", ret);
    }
    assert(ret == 0);

    for (;;) {
        /* BRAKE - duty cycle 100% */
        last_wakeup = xtimer_now();
        motors_brake();
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

        /* CW - duty cycle 50% */
        last_wakeup = xtimer_now();
        motors_control(dir * pwm_res / 10);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

        /* Disable motor during INTERVAL µs (motor driver must have enable
           feature */
        last_wakeup = xtimer_now();
        motor_disable(MOTOR_DRIVER_DEV(0), MOTOR_0_ID);
        motor_disable(MOTOR_DRIVER_DEV(0), MOTOR_1_ID);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);
        motor_enable(MOTOR_DRIVER_DEV(0), MOTOR_0_ID);
        motor_enable(MOTOR_DRIVER_DEV(0), MOTOR_1_ID);

        /* CW - duty cycle 100% */
        last_wakeup = xtimer_now();
        motors_control(dir * pwm_res / 5);
        xtimer_periodic_wakeup(&last_wakeup, INTERVAL);

        /* Reverse direction */
        dir = dir * -1;
    }

    return NULL;
}
#endif /* MOTOR_DRIVER_NUMOF */

int main(void)
{
    xtimer_init();

#ifdef MOTOR_DRIVER_NUMOF
    thread_create(motion_control_thread_stack, sizeof(motion_control_thread_stack),
                  0, 0,
                  motion_control_thread, NULL, "motion_ctrl");
#endif /* MOTOR_DRIVER_NUMOF */

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
