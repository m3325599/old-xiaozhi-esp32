#include "board.h"
#include "wifi_board.h"
#include "application.h"
#include "system_info.h"
#include "display/oled_display.h"
#include <driver/gpio.h>
#include <driver/i2c.h>
#include "led/gpio_led.h"

class WeiwuX1WifiBoard : public WifiBoard {
protected:
    i2c_master_bus_handle_t i2c_bus_;
    Button* main_button_ = nullptr;
    Button* touch_button_ = nullptr;
    Display* display_ = nullptr;

    void InitializeIot() override {
        // 初始化主按键
        main_button_ = new Button(GPIO_NUM_0, true, true);
        main_button_->OnClick([this]() {
            Application::GetInstance().ToggleChatState();
        });
        main_button_->OnPressDown([this]() {
            Application::GetInstance().TriggerWakeWord();
        });

        // 初始化触摸按键
        touch_button_ = new Button(GPIO_NUM_5, false, true);
        touch_button_->OnClick([this]() {
            // 自定义功能，例如音量调节或静音
            Application::GetInstance().ToggleMute();
        });

        // 初始化I2C总线
        i2c_master_bus_config_t i2c_config = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = DISPLAY_SDA_PIN,
            .scl_io_num = DISPLAY_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_keep_time_ns = 5,
            .flags.enable_internal_pullup = false,
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_config, &i2c_bus_));

        // 初始化显示
        display_ = new OledDisplay(i2c_bus_, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);

        // 初始化系统信息
        auto& system_info = SystemInfo::GetInstance();
        system_info.SetDeviceType("weiwu-x1-wifi");
        system_info.SetDisplayWidth(DISPLAY_WIDTH);
        system_info.SetDisplayHeight(DISPLAY_HEIGHT);
    }

    virtual Led* GetLed() override {
        static SingleLed led(BUILTIN_LED_GPIO);
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
        return display_;
    }

    ~WeiwuX1WifiBoard() {
        if (display_ != nullptr) {
            delete display_;
        }
        if (main_button_ != nullptr) {
            delete main_button_;
        }
        if (touch_button_ != nullptr) {
            delete touch_button_;
        }
        if (i2c_bus_ != nullptr) {
            i2c_del_master_bus(i2c_bus_);
        }
    }
};

extern "C" WeiwuX1WifiBoard* CreateBoard() {
    return new WeiwuX1WifiBoard();
}