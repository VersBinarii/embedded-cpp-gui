#include "main.h"
#include "lcd.h"

namespace ILI9341 {

// clang-format off
static constexpr uint8_t initcmd[] = {
  0xEF, 3, 0x03, 0x80, 0x02,
  0xCF, 3, 0x00, 0xC1, 0x30,
  0xED, 4, 0x64, 0x03, 0x12, 0x81,
  0xE8, 3, 0x85, 0x00, 0x78,
  0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  0xF7, 1, 0x20,
  0xEA, 2, 0x00, 0x00,
  ILI9341_PWCTR1  , 1, 0x23,             // Power control VRH[5:0]
  ILI9341_PWCTR2  , 1, 0x10,             // Power control SAP[2:0];BT[3:0]
  ILI9341_VMCTR1  , 2, 0x3e, 0x28,       // VCM control
  ILI9341_VMCTR2  , 1, 0x86,             // VCM control2
  ILI9341_MADCTL  , 1, 0x48,             // Memory Access Control
  ILI9341_VSCRSADD, 1, 0x00,             // Vertical scroll zero
  ILI9341_PIXFMT  , 1, 0x55,
  ILI9341_FRMCTR1 , 2, 0x00, 0x18,
  ILI9341_DFUNCTR , 3, 0x08, 0x82, 0x27, // Display Function Control
  0xF2, 1, 0x00,                         // 3Gamma Function Disable
  ILI9341_GAMMASET , 1, 0x01,             // Gamma curve selected
  ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  ILI9341_SLPOUT  , 0x80,                // Exit Sleep
  ILI9341_DISPON  , 0x80,                // Display on
  0x00                                   // End of list
};
// clang-format on

//
// Rotation stuff
//
#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

    //
    //  Basic function - write those for your MCU
    //
    static void delay (uint32_t ms) { HAL_Delay (ms); }

    static void send_to_tft (LCD &lcd, uint8_t *byte, uint32_t length) {
        // That is taken from HAL Transmit function
        while (length > 0U) {
            /* Wait until TXE flag is set to send data */
            if (__HAL_SPI_GET_FLAG (lcd.spi, SPI_FLAG_TXE)) {
                // Fill Data Register in SPI
                *((__IO uint8_t *)&lcd.spi->Instance->DR) = (*byte);
                // Next byte
                byte++;
                // Length decrement
                length--;
            }
        }

        // Wait for Transfer end
        while (__HAL_SPI_GET_FLAG (lcd.spi, SPI_FLAG_BSY) != RESET) {}
    }

    static void send_command (LCD &lcd, uint8_t command) {
        // CS Low
#if (ILI9341_USE_CS == 1)
        ILI9341_CS_LOW;
#endif
        // DC to Command - DC to Low
        ILI9341_DC_LOW;

        // Send to TFT 1 byte
        send_to_tft (lcd, &command, 1);

        // CS High
#if (ILI9341_USE_CS == 1)
        ILI9341_CS_HIGH;
#endif
    }

    static void send_command_and_data (LCD &lcd, uint8_t command,
                                       uint8_t *data, uint32_t length) {
        // CS Low
#if (ILI9341_USE_CS == 1)
        ILI9341_CS_LOW;
#endif
        // DC to Command - DC to Low
        ILI9341_DC_LOW;
        // Send to TFT 1 byte
        send_to_tft (lcd, &command, 1);

        // DC to Data - DC to High
        ILI9341_DC_HIGH;
        // Send to TFT Length byte
        send_to_tft (lcd, data, length);

        // CS High
#if (ILI9341_USE_CS == 1)
        ILI9341_CS_HIGH;
#endif
    }

    //
    // TFT Functions
    //
    void LCD::set_rotation (uint8_t rotation) {
        if (rotation > 3)
            return;

        //
        // Set appropriate bits for Rotation
        //
        switch (rotation) {
        case 0:
            rotation = (MADCTL_MX | MADCTL_BGR);
            break;
        case 1:
            rotation = (MADCTL_MV | MADCTL_BGR);
            break;
        case 2:
            rotation = (MADCTL_MY | MADCTL_BGR);
            break;
        case 3:
            rotation = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            break;
        }

        // Write indo MAD Control register our Rotation data
        send_command_and_data (*this, ILI9341_MADCTL, &rotation, 1);
    }

     static void set_addr_window (LCD &lcd, uint16_t x1, uint16_t y1,
                                 uint16_t w, uint16_t h) {
        uint8_t to_send[4];
        // Calculate end ranges
         uint16_t x2 = (x1 + w - 1), y2 = (y1 + h - 1);

        // Fulfill X's buffer
        to_send[0] = x1 >> 8;
        to_send[1] = x1 & 0xFF;
        to_send[2] = x2 >> 8;
        to_send[3] = x2 & 0xFF;
        // Push X's buffer
        send_command_and_data (lcd, ILI9341_CASET, to_send, 4);

        // Fulfill Y's buffer
        to_send[0] = y1 >> 8;
        to_send[1] = y1 & 0xFF;
        to_send[2] = y2 >> 8;
        to_send[3] = y2 & 0xFF;
        // Push Y's buffer
        send_command_and_data (lcd, ILI9341_PASET, to_send, 4);
    }

    void LCD::write_pixel (int16_t x, int16_t y, Color color) {
        uint8_t to_send[2];

        if ((x >= 0) && (x < TFTWIDTH) && (y >= 0) && (y < TFTHEIGHT)) {
            // Set Window for 1x1 pixel
            set_addr_window (*this, x, y, 1, 1);

            // Fulfill buffer with color
            to_send[0] = color >> 8;
            to_send[1] = color & 0xFF;
            // Push color bytes to RAM
            send_command_and_data (*this, ILI9341_RAMWR, to_send, 2);
        }
    }

    void LCD::draw_image (uint32_t x, uint32_t y, const uint8_t *img,
                          uint32_t w, uint32_t h) {
        // Check if image will fit into screen - cannot make it outside by
        // hardware
        if (((x + w) <= TFTWIDTH) && ((y + h) <= TFTHEIGHT)) {
            // Set window for image
            set_addr_window (*this, x, y, w, h);
            // Push image to RAM
            send_command_and_data (*this, ILI9341_RAMWR, (uint8_t *)img,
                                   (w * h * 2));
        }
    }

    void LCD::clear_display (Color color) {
        // Set window for whole screen
        set_addr_window (*this, 0, 0, TFTWIDTH, TFTHEIGHT);
        // Set RAM writing
        send_command (*this, ILI9341_RAMWR);

        uint32_t length = TFTWIDTH * TFTHEIGHT;

#if (ILI9341_USE_CS == 1)
        ILI9341_CS_LOW;
#endif
        ILI9341_DC_HIGH; // Data mode

        while (length > 0U) {
            /* Wait until TXE flag is set to send data */
            if (__HAL_SPI_GET_FLAG (this->spi, SPI_FLAG_TXE)) {
                // Write higher byte of color to DR
                *((__IO uint8_t *)&this->spi->Instance->DR) = (color >> 8);
                // Wait for transfer
                while (__HAL_SPI_GET_FLAG (this->spi, SPI_FLAG_TXE) != SET) {}
                // Write lower byt of color to DR
                *((__IO uint8_t *)&this->spi->Instance->DR) = (color & 0xFF);
                // Decrease Lenght
                length--;
            }
        }

        // Wait for the end of transfer
        while (__HAL_SPI_GET_FLAG (this->spi, SPI_FLAG_BSY) != RESET) {}

#if (ILI9341_USE_CS == 1)
        ILI9341_CS_HIGH;
#endif
    }
    LCD::LCD (SPI_HandleTypeDef *hspi) : spi{ hspi } {

        uint8_t cmd, x, numArgs;
        const uint8_t *addr = initcmd;
        __HAL_SPI_ENABLE (this->spi);

        send_command (*this, ILI9341_SWRESET); // Engage software reset
        delay (150);

        while ((cmd = *(addr++)) > 0) {
            x = *(addr++);
            numArgs = x & 0x7F;
            // Push Init data
            send_command_and_data (*this, cmd, (uint8_t *)addr, numArgs);

            addr += numArgs;

            if (x & 0x80) {
                delay (150);
            }
        }

        // Set selected Rotation
        this->set_rotation (ILI9341_ROTATION);
    }
}
