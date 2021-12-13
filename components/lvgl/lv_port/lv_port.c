#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "lv_port.h"
#include "lvgl.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

static lv_disp_drv_t disp_drv;
static const char *TAG = "lv_port";

static void lv_tick_inc_cb(void *data)
{
    uint32_t tick_inc_period_ms = *((uint32_t *)data);
    lv_tick_inc(tick_inc_period_ms);
}

static bool lv_port_flush_ready(void)
{
    lv_disp_flush_ready(&disp_drv);
    return false;
}

static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t)drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static void lv_port_disp_init(void)
{
    static lv_disp_draw_buf_t draw_buf_dsc;
    size_t disp_buf_height = 40;
    static lv_color_t p_disp_buf[LCD_WIDTH * 40];

    spi_bus_config_t buscfg = {
        .sclk_io_num = SCLK_PIN,
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_WIDTH * LCD_HEIGHT * sizeof(uint16_t)};
    ESP_ERROR_CHECK(spi_bus_initialize(SPI_PORT, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = DC_PIN,
        .cs_gpio_num = CS_PIN,
        .pclk_hz = SPI_CLK,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .on_color_trans_done = lv_port_flush_ready,
        .user_ctx = &disp_drv};
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI_PORT, &io_config, &io_handle));

    ESP_LOGI(TAG, "Install LCD driver of st7789");
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = RST_PIN,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));

    esp_lcd_panel_reset(panel_handle);
    esp_lcd_panel_init(panel_handle);

    esp_lcd_panel_invert_color(panel_handle, true);
    esp_lcd_panel_set_gap(panel_handle, 0, 0);

    if (LCD_DIRECTION == 0)
    {
        esp_lcd_panel_swap_xy(panel_handle, 0);
        esp_lcd_panel_mirror(panel_handle, false, false);
    }
    else if(LCD_DIRECTION == 90)
    {
        esp_lcd_panel_swap_xy(panel_handle, 1);
        esp_lcd_panel_mirror(panel_handle, true, false);
    }
    else if(LCD_DIRECTION == 180)
    {
        esp_lcd_panel_swap_xy(panel_handle, 0);
        esp_lcd_panel_mirror(panel_handle, true, true);
    }
    else if(LCD_DIRECTION == 270)
    {
        esp_lcd_panel_swap_xy(panel_handle, 1);
        esp_lcd_panel_mirror(panel_handle, false, true);
    }
    else
    {
        ESP_LOGE(TAG,"LCD_DIRECTION error");
    }

    lv_disp_draw_buf_init(&draw_buf_dsc, p_disp_buf, NULL, LCD_WIDTH * disp_buf_height);
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_WIDTH;
    disp_drv.ver_res = LCD_HEIGHT;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf_dsc;
    disp_drv.user_data = panel_handle;
    lv_disp_drv_register(&disp_drv);
}

static esp_err_t lv_port_tick_init(void)
{
    static const uint32_t tick_inc_period_ms = 5;
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = lv_tick_inc_cb,
        .name = "lvgl_tick",
        .arg = &tick_inc_period_ms,
        .dispatch_method = ESP_TIMER_TASK,
        .skip_unhandled_events = true,
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, tick_inc_period_ms * 1000));
    return ESP_OK;
}

esp_err_t lv_port_init(void)
{
    lv_init();
    lv_port_disp_init();
    lv_port_tick_init();
    return ESP_OK;
}
