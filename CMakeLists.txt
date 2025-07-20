set(requires
        freertos
        esp_h264
        esp32-camera
        esp_psram
        iot_usbh_modem 
        mbedtls
        nvs_flash
        esp_timer
        usb
        esp_audio_codec
        esp_driver_gpio
        esp_driver_i2c
        esp_driver_i2s
        fatfs
        )

set(SRC
        "Audio.c"
        "Communication.c"
        "ESPCAMNETMain.c"
        "Events.c"
        "Hardware.c"
        "MemoryMgt.c"
        "Modem.c"
        "NAU881x.c"
        "Setting.c"
        "Storage.c"
        "Video.c"
        "Monitor.c"
        "max17048.c"
        "I2C_routines.c"
)

idf_component_register( SRCS  ${SRC}
                        INCLUDE_DIRS "." "../Components" 
                        REQUIRES ${requires}
                       )