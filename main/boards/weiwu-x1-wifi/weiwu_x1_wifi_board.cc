#include "board.h"
#include "wifi_board.h"
#include "application.h"
#include "system_info.h"
#include "audio_codecs/no_audio_codec.h"
#include <driver/gpio.h>
#include "led/gpio_led.h"

class WeiwuX1WifiBoard : public WifiBoard {
protected:
    Button* main_button_ = nullptr;
    Button* touch_button_ = nullptr;

    void InitializeIot() override {
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
        static NoAudioCodecSimplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_SPK_GPIO_BCLK, AUDIO_I2S_SPK_GPIO_LRCK, AUDIO_I2S_SPK_GPIO_DOUT, 
            AUDIO_I2S_MIC_GPIO_SCK, AUDIO_I2S_MIC_GPIO_WS, AUDIO_I2S_MIC_GPIO_DIN);
#else
        static NoAudioCodecDuplex audio_codec(AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
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