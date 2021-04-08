#ifndef LCD_TFT_H_
#define LCD_TFT_H_

#include "main.h"

namespace ILI9341 {

#define ILI9341_ROTATION_0 0
#define ILI9341_ROTATION_90 1
#define ILI9341_ROTATION_180 2
#define ILI9341_ROTATION_270 3

#define ILI9341_ROTATION \
    ILI9341_ROTATION_180 // 0 - 0*, 1 - 90*, 2 - 180*, 3 - 270*

#define ILI9341_USE_HW_RESET 1
#define ILI9341_USE_CS 1

//
// Pin operation
//
#if (ILI9341_USE_CS == 1)
#define ILI9341_CS_LOW \
    HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define ILI9341_CS_HIGH \
    HAL_GPIO_WritePin (LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)
#endif
#define ILI9341_DC_LOW HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define ILI9341_DC_HIGH HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)

#define ILI9341_SPI_TIMEOUT 1000

//
// Registers
//
#if (ILI9341_ROTATION == 0) || (ILI9341_ROTATION == 2)
    constexpr uint16_t TFTWIDTH = 240;                          ///< ILI9341 max TFT width
    constexpr uint16_t TFTHEIGHT = 320; ///< ILI9341 max TFT height
#elif (ILI9341_ROTATION == 1) || (ILI9341_ROTATION == 3)
    constexpr uint16_t TFTWIDTH = 320;                                ///< ILI9341 max TFT width
    constexpr uint16_t TFTHEIGHT = 240; ///< ILI9341 max TFT height
#endif

#define ILI9341_NOP 0x00     ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04   ///< Read display identif ;ication information
#define ILI9341_RDDST 0x09   ///< Read Display Status

#define ILI9341_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12  ///< Partial Mode ON
#define ILI9341_NORON 0x13  ///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9341_INVON 0x21    ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29   ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30    ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1 \
    0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 \
    0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction
        //#define ILI9341_PWCTR6     0xFC

    // Color definitions
    enum Color {
        BLACK = 0x0000,       ///<   0,   0,   0
        NAVY = 0x000F,        ///<   0,   0, 123
        DARKGREEN = 0x03E0,   ///<   0, 125,   0
        DARKCYAN = 0x03EF,    ///<   0, 125, 123
        MAROON = 0x7800,      ///< 123,   0,   0
        PURPLE = 0x780F,      ///< 123,   0, 123
        OLIVE = 0x7BE0,       ///< 123, 125,   0
        LIGHTGREY = 0xC618,   ///< 198, 195, 198
        DARKGREY = 0x7BEF,    ///< 123, 125, 123
        BLUE = 0x001F,        ///<   0,   0, 255
        GREEN = 0x07E0,       ///<   0, 255,   0
        CYAN = 0x07FF,        ///<   0, 255, 255
        RED = 0xF800,         ///< 255,   0,   0
        MAGENTA = 0xF81F,     ///< 255,   0, 255
        YELLOW = 0xFFE0,      ///< 255, 255,   0
        WHITE = 0xFFFF,       ///< 255, 255, 255
        ORANGE = 0xFD20,      ///< 255, 165,   0
        GREENYELLOW = 0xAFE5, ///< 173, 255,  41
        PINK = 0xFC18,        ///< 255, 130, 198
    };

    struct LCD {
        SPI_HandleTypeDef *spi;

        LCD (SPI_HandleTypeDef *hspi);
        void set_rotation (uint8_t rotation);
        void write_pixel (int16_t x, int16_t y, Color color);
        void clear_display (Color color);
        void draw_image (uint32_t x, uint32_t y, const uint8_t *img,
                         uint32_t w, uint32_t h);
    };
}
#endif /* INC_TFT_ILI9341_H_ */
