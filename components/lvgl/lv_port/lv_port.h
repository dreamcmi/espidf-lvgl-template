#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH       (320)
#define LCD_HEIGHT      (240)

#define SCLK_PIN        (0)
#define MOSI_PIN        (1)
#define CS_PIN          (2)
#define DC_PIN          (3)
#define RST_PIN         (4)

#define SPI_PORT        (SPI2_HOST)
#define SPI_CLK         (40*1000*1000)

#define LCD_DIRECTION   (270)

esp_err_t lv_port_init(void);

#ifdef __cplusplus
}
#endif
