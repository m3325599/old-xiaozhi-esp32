#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/display.h"
#include "system_reset.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "led/single_led.h"
#include "assets/lang_config.h"

#include <wifi_station.h>
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <esp_sleep.h>

#define TAG "WEIWU_X1_WIFI"

static void init_spk_en_init()
{
    gpio_reset_pin(AUDIO_I2S_SPK_GPIO_EN);
    gpio_set_direction(AUDIO_I2S_SPK_GPIO_EN, GPIO_MODE_OUTPUT);
    vTaskDelay(pdMS_TO_TICKS(50));
    gpio_set_level(AUDIO_I2S_SPK_GPIO_EN, 1);
}

static void init_simple_led()
{
    gpio_reset_pin(SIMPLE_LED_GPIO);
    gpio_set_direction(SIMPLE_LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(SIMPLE_LED_GPIO, 1);
}

static void prepear_and_sleep()
{
    auto& application = Application::GetInstance();
    auto& board = Board::GetInstance();
    application.SetDeviceState(kDeviceStateIdle);
    application.ResetProtocol();
    board.GetAudioCodec()->EnableOutput(true);

    application.Alert(Lang::Strings::SHUTDOWN, Lang::Strings::SHUTDOWN, "", Lang::Strings::SHUTDOWN);

    xTaskCreate([](void* ctx) {
        ESP_LOGI(TAG, "Sleeping in 3 seconds");
        vTaskDelay(pdMS_TO_TICKS(3000));

        ESP_LOGW(TAG, "启动电源按钮开机...");
        esp_err_t err = esp_sleep_enable_ext0_wakeup((gpio_num_t)BOOT_BUTTON_GPIO, 0);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to enable BOOT_BUTTON_GPIO as wakeup source: %s", esp_err_to_name(err));
            return;
        }

        ESP_LOGW(TAG, "系统进入睡眠");
        esp_deep_sleep_start();
    }, "sleep_task", 4096, NULL, 5, NULL);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}

class CompactWifiBoard : public WifiBoard {
private:
    Button boot_button_;
    Button touch_button_;

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting && !WifiStation::GetInstance().IsConnected()) {
                EnterWifiConfigMode();
            }
            app.ToggleChatState();
        });

        boot_button_.OnLongPress([this]() {
            ESP_LOGI(TAG, "Boot Button LongPress");
            prepear_and_sleep();
        });

        touch_button_.OnPressDown([this]() {
            Application::GetInstance().StartListening();
        });
        touch_button_.OnPressUp([this]() {
            Application::GetInstance().StopListening();
        });
    }

public:
    CompactWifiBoard() :
        boot_button_(BOOT_BUTTON_GPIO),
        touch_button_(TOUCH_BUTTON_GPIO) {
        InitializeButtons();
        init_simple_led();
        init_spk_en_init();
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
        return &led;
    }

    virtual AudioCodec* GetAudioCodec() override {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        static NoAudioCodecSimplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT,
            AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
#else
        static NoAudioCodecDuplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
#endif
        return &audio_codec;
    }

    virtual Display* GetDisplay() override {
        static NoDisplay display;
        return &display;
    }
};

DECLARE_BOARD(CompactWifiBoard);
