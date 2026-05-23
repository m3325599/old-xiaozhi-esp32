#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE 16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// 如果使用 Duplex I2S 模式，请注释下面一行
#define AUDIO_I2S_METHOD_SIMPLEX

#ifdef AUDIO_I2S_METHOD_SIMPLEX

// 硅基数字麦克风 - 根据原理图配置
#define AUDIO_I2S_MIC_GPIO_WS GPIO_NUM_2    // MIC_WS (Word Select)
#define AUDIO_I2S_MIC_GPIO_SCK GPIO_NUM_38  // MIC_SCK (Serial Clock)
#define AUDIO_I2S_MIC_GPIO_DIN GPIO_NUM_39  // MIC_DATA (Data Input)

// D类功放(9db) - 根据原理图配置
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_10  // BCLK (Bit Clock)
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_11  // LRCK (Left/Right Clock)
#define AUDIO_I2S_SPK_GPIO_SD_MODE GPIO_NUM_12  // SD_MODE (Shutdown Mode)
#define AUDIO_I2S_SPK_GPIO_MUTE GPIO_NUM_13     // MUTE (Mute Control)
#define SYSTEM_LED_GPIO GPIO_NUM_13             // 使用MUTE引脚作为系统LED

#else

#define AUDIO_I2S_GPIO_WS GPIO_NUM_4
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_5
#define AUDIO_I2S_GPIO_DIN GPIO_NUM_6
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7

#endif

#define BOOT_BUTTON_GPIO GPIO_NUM_0  // 开关机按键
#define TOUCH_BUTTON_GPIO GPIO_NUM_5 // 自定义功能按键

#define SIMPLE_LED_GPIO GPIO_NUM_8   // 简单LED指示灯
#define BUILTIN_LED_GPIO GPIO_NUM_48 // 板载LED

// 显示配置 - 根据实际硬件连接调整
#define DISPLAY_SDA_PIN GPIO_NUM_41
#define DISPLAY_SCL_PIN GPIO_NUM_42

// OLED配置
#if CONFIG_OLED_SSD1306_128X32
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32
#elif CONFIG_OLED_SSD1306_128X64
#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128
#else
// 默认使用OLED 128x64
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#endif

#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false

#endif // _BOARD_CONFIG_H_