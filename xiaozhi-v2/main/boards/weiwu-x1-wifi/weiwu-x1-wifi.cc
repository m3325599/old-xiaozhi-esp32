// Weiwu X1 WiFi board support
// Fix build and ensure proper firmware generation
#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/display.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "led/single_led.h"
#include "assets/lang_config.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include "application.h"
#include "board.h"
#include <esp_sleep.h> // 睡眠

#define TAG "WEIWU_X1_WIFI"

// LV_FONT_DECLARE(font_puhui_14_1);
// LV_FONT_DECLARE(font_awesome_14_1);
static void init_spk_en_init()
{
    gpio_reset_pin(AUDIO_I2S_SPK_GPIO_EN);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(AUDIO_I2S_SPK_GPIO_EN, GPIO_MODE_OUTPUT);

    // 增加延迟，等待MAX98357上电稳定，避免上电破音
    vTaskDelay(pdMS_TO_TICKS(50));

    // 配置功放使能引脚
    gpio_set_level(AUDIO_I2S_SPK_GPIO_EN, 1);
}
static void init_simple_led()
{
    gpio_reset_pin(SIMPLE_LED_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(SIMPLE_LED_GPIO, GPIO_MODE_OUTPUT);
    // 配置功放使能引脚
    gpio_set_level(SIMPLE_LED_GPIO, 1);
}

static void prepear_and_sleep()
{
    ESP_LOGW(TAG, "系统进入睡眠");
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGW(TAG, "启动电源按钮开机...");
    esp_err_t err = esp_sleep_enable_ext0_wakeup((gpio_num_t)BOOT_BUTTON_GPIO, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable BOOT_BUTTON_GPIO as wakeup source: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGW(TAG, "系统进入深度睡眠");
    esp_deep_sleep_start();
}

class CompactWifiBoard : public WifiBoard
{
private:
    // i2c_master_bus_handle_t display_i2c_bus_;
    Button boot_button_;
    Button touch_button_;
    // Button volume_up_button_;
    // Button volume_down_button_;

    // void InitializeDisplayI2c() {
    //     i2c_master_bus_config_t bus_config = {
    //         .i2c_port = (i2c_port_t)0,
    //         .sda_io_num = DISPLAY_SDA_PIN,
    //         .scl_io_num = DISPLAY_SCL_PIN,
    //         .clk_source = I2C_CLK_SRC_DEFAULT,
    //         .glitch_ignore_cnt = 7,
    //         .intr_priority = 0,
    //         .trans_queue_depth = 0,
    //         .flags = {
    //             .enable_internal_pullup = 1,
    //         },
    //     };
    //     ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &display_i2c_bus_));
    // }

    void InitializeButtons()
    {
        boot_button_.OnClick([this]()
                             {
            auto& app = Application::GetInstance();
            app.ToggleChatState(); });

        boot_button_.OnLongPress([this]()
                                 {
                ESP_LOGI(TAG, "Boot Button LongPress");
                prepear_and_sleep(); });
        touch_button_.OnPressDown([this]()
                                  { Application::GetInstance().StartListening(); });
        touch_button_.OnPressUp([this]()
                                { Application::GetInstance().StopListening(); });

        // volume_up_button_.OnClick([this]() {
        //     auto codec = GetAudioCodec();
        //     auto volume = codec->output_volume() + 10;
        //     if (volume > 100) {
        //         volume = 100;
        //     }
        //     codec->SetOutputVolume(volume);
        //     GetDisplay()->ShowNotification("音量 " + std::to_string(volume));
        // });

        // volume_up_button_.OnLongPress([this]() {
        //     GetAudioCodec()->SetOutputVolume(100);
        //     GetDisplay()->ShowNotification("最大音量");
        // });

        // volume_down_button_.OnClick([this]() {
        //     auto codec = GetAudioCodec();
        //     auto volume = codec->output_volume() - 10;
        //     if (volume < 0) {
        //         volume = 0;
        //     }
        //     codec->SetOutputVolume(volume);
        //     GetDisplay()->ShowNotification("音量 " + std::to_string(volume));
        // });

        // volume_down_button_.OnLongPress([this]() {
        //     GetAudioCodec()->SetOutputVolume(0);
        //     GetDisplay()->ShowNotification("已静音");
        // });
    }

public:
    CompactWifiBoard() : boot_button_(BOOT_BUTTON_GPIO),
                         touch_button_(TOUCH_BUTTON_GPIO)
    {
        // volume_up_button_(VOLUME_UP_BUTTON_GPIO),
        // volume_down_button_(VOLUME_DOWN_BUTTON_GPIO) {
        // InitializeDisplayI2c();
        InitializeButtons();
        init_simple_led();
        init_spk_en_init();
    }

    virtual Led *
    GetLed() override
    {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec *GetAudioCodec() override
    {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        static NoAudioCodecSimplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
                                               AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT, AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
#else
        static NoAudioCodecDuplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
                                              AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
#endif
        return &audio_codec;
    }

    virtual Display *GetDisplay() override
    {
        static NoDisplay display;
        return &display;
    }
};

DECLARE_BOARD(CompactWifiBoard);
