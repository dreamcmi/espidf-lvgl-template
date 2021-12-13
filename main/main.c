#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lv_demo.h"
#include "lv_port.h"
#include "lvgl.h"

void app_main(void)
{
    ESP_ERROR_CHECK(lv_port_init());

    // lv_demo_widgets();      /* A widgets example. This is what you get out of the box */
    // lv_demo_printer();      /* A printer user interface example. */
    // lv_demo_music();        /* A modern, smartphone-like music player demo. */
    // lv_demo_stress();       /* A stress test for LVGL. */
    lv_demo_benchmark();    /* A demo to measure the performance of LVGL or to compare different settings. */

    while (vTaskDelay(1), true) {
        lv_task_handler();
    }
}
