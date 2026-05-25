#include "board.h"
#include "wifi_board.h"
#include "application.h"
#include "system_info.h"
#include "audio_codecs/no_audio_codec.h"
#include <driver/gpio.h>
#include "led/gpio_led.h"
#include <esp_log.h>

#define TAG "WeiwuX1WifiBoard"

// 定义所有可能的 GPIO 组合用于调试
#define GPIO_TEST_1_MIC_WS   GPIO_NUM_2
#define GPIO_TEST_1_MIC_SCK  GPIO_NUM_38
#define GPIO_TEST_1_MIC_DIN  GPIO_NUM_39
#define GPIO_TEST_1_SPK_BCLK GPIO_NUM_11
#define GPIO_TEST_1_SPK_LRCK GPIO_NUM_12
#define GPIO_TEST_1_SPK_DOUT GPIO_NUM_10

#define GPIO_TEST_2_MIC_WS   GPIO_NUM_4
#define GPIO_TEST_2_MIC_SCK  GPIO_NUM_5
#define GPIO_TEST_2_MIC_DIN  GPIO_NUM_6
#define GPIO_TEST_2_SPK_BCLK GPIO_NUM_4
#define GPIO_TEST_2_SPK_LRCK GPIO_NUM_5
#define GPIO_TEST_2_SPK_DOUT GPIO_NUM_7

#define GPIO_TEST_3_MIC_WS   GPIO_NUM_5
#define GPIO_TEST_3_MIC_SCK  GPIO_NUM_4
#define GPIO_TEST_3_MIC_DIN  GPIO_NUM_6
#define GPIO_TEST_3_SPK_BCLK GPIO_NUM_5
#define GPIO_TEST_3_SPK_LRCK GPIO_NUM_4
#define GPIO_TEST_3_SPK_DOUT GPIO_NUM_7

class WeiwuX1WifiBoard : public WifiBoard {
protected:
    Button* main_button_ = nullptr;
    Button* touch_button_ = nullptr;

    void LogAllGpioConfigs() {
        ESP_LOGI(TAG, "========================================");
        ESP_LOGI(TAG, "WEIWU-X1-WIFI GPIO CONFIGURATION DUMP");
        ESP_LOGI(TAG, "========================================");
        
        // 当前实际配置
        ESP_LOGI(TAG, "--- CURRENT ACTIVE CONFIG ---");
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        ESP_LOGI(TAG, "Mode: SIMPLEX (Separate MIC and SPK I2S)");
        ESP_LOGI(TAG, "MIC  - WS: %d, SCK: %d, DIN: %d", 
                 AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_DIN);
        ESP_LOGI(TAG, "SPK  - BCLK: %d, LRCK: %d, DOUT: %d",
                 AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT);
#else
        ESP_LOGI(TAG, "Mode: DUPLEX (Shared I2S)");
        ESP_LOGI(TAG, "I2S - BCLK: %d, WS: %d, DOUT: %d, DIN: %d",
                 AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
#endif
        ESP_LOGI(TAG, "BTN  - BOOT: %d, TOUCH: %d", BOOT_BUTTON_GPIO, TOUCH_BUTTON_GPIO);
        ESP_LOGI(TAG, "LED  - SIMPLE: %d, BUILTIN: %d", SIMPLE_LED_GPIO, BUILTIN_LED_GPIO);
        
        // 所有可能的 GPIO 组合
        ESP_LOGI(TAG, "--- POSSIBLE GPIO COMBINATIONS ---");
        ESP_LOGI(TAG, "[Combo 1] MIC_WS=%d, MIC_SCK=%d, MIC_DIN=%d | SPK_BCLK=%d, SPK_LRCK=%d, SPK_DOUT=%d",
                 GPIO_TEST_1_MIC_WS, GPIO_TEST_1_MIC_SCK, GPIO_TEST_1_MIC_DIN,
                 GPIO_TEST_1_SPK_BCLK, GPIO_TEST_1_SPK_LRCK, GPIO_TEST_1_SPK_DOUT);
        ESP_LOGI(TAG, "[Combo 2] MIC_WS=%d, MIC_SCK=%d, MIC_DIN=%d | SPK_BCLK=%d, SPK_LRCK=%d, SPK_DOUT=%d",
                 GPIO_TEST_2_MIC_WS, GPIO_TEST_2_MIC_SCK, GPIO_TEST_2_MIC_DIN,
                 GPIO_TEST_2_SPK_BCLK, GPIO_TEST_2_SPK_LRCK, GPIO_TEST_2_SPK_DOUT);
        ESP_LOGI(TAG, "[Combo 3] MIC_WS=%d, MIC_SCK=%d, MIC_DIN=%d | SPK_BCLK=%d, SPK_LRCK=%d, SPK_DOUT=%d",
                 GPIO_TEST_3_MIC_WS, GPIO_TEST_3_MIC_SCK, GPIO_TEST_3_MIC_DIN,
                 GPIO_TEST_3_SPK_BCLK, GPIO_TEST_3_SPK_LRCK, GPIO_TEST_3_SPK_DOUT);
        
        ESP_LOGI(TAG, "========================================");
    }

    void InitializeIot() override {
        // 首先输出所有 GPIO 配置
        LogAllGpioConfigs();
        
        // 初始化主按键 - GPIO 0与日志匹配
        main_button_ = new Button(GPIO_NUM_0, true, true);
        main_button_->OnClick([this]() {
            Application::GetInstance().ToggleChatState();
        });
        main_button_->OnPressDown([this]() {
            Application::GetInstance().TriggerWakeWord();
        });

        // 初始化触摸按键 - GPIO 5与日志匹配  
        touch_button_ = new Button(GPIO_NUM_5, false, true);
        touch_button_->OnClick([this]() {
            // 自定义功能，例如音量调节或静音
            Application::GetInstance().ToggleMute();
        });

        // 初始化系统信息
        auto& system_info = SystemInfo::GetInstance();
        system_info.SetDeviceType("weiwu-x1-wifi");
        system_info.SetDisplayWidth(0);
        system_info.SetDisplayHeight(0);
    }

    virtual Led* GetLed() override {
        static SingleLed led(SYSTEM_LED_GPIO); // Use system LED GPIO from config
        return &led;
    }

    virtual Button* GetButton(SystemButton key) override {
        switch (key) {
            case kButtonMain:
                return main_button_;
            case kButtonTouch:
                return touch_button_;
            default:
                return nullptr;
        }
    }

    virtual Display* GetDisplay() override {
        return nullptr; // No display on this device
    }

    virtual AudioCodec* GetAudioCodec() override {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        // NoAudioCodecSimplex: 使用Simplex模式，独立配置麦克风和扬声器
        ESP_LOGI(TAG, "Audio Config: Simplex Mode");
        ESP_LOGI(TAG, "MIC GPIO - SCK: %d, WS: %d, DIN: %d", 
                 AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
        ESP_LOGI(TAG, "SPK GPIO - BCLK: %d, LRCK: %d, DOUT: %d",
                 AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT);
        
        static NoAudioCodecSimplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT,
            AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
#else
        ESP_LOGI(TAG, "Audio Config: Duplex Mode");
        ESP_LOGI(TAG, "I2S GPIO - BCLK: %d, WS: %d, DOUT: %d, DIN: %d",
                 AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
        
        static NoAudioCodecDuplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN);
#endif
        return &audio_codec;
    }

    ~WeiwuX1WifiBoard() {
        if (main_button_ != nullptr) {
            delete main_button_;
        }
        if (touch_button_ != nullptr) {
            delete touch_button_;
        }
    }
};

DECLARE_BOARD(WeiwuX1WifiBoard);