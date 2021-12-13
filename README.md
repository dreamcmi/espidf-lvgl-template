# ESP-IDF LVGL Template

本项目参考了 [ESP-BOX](https://github.com/espressif/esp-box)和[ESP-IDF](https://github.com/espressif/esp-idf)。

通过使用ESP_LCD组件移植LVGL，总体思路是使用lvgl的图形库绘制bitmap,然后使用`esp_lcd_panel_draw_bitmap`向屏幕发送数据，这样看起来很方便~

注意: ESP_LCD组件在ESP-IDF V4.4才开始集成，建议使用4.4分支开发。

## LICENSE

MIT License
