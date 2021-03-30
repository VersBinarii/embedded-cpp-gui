/*
 * bme_280.h
 *
 *  Created on: Mar 23, 2021
 *      Author: kris
 */


#ifndef BME_280_H_
#define BME_280_H_

#include "main.h"

namespace BME {
    enum class Register {
        //
        //	Coeffs registers
        //
        DIG_T1 = 0x88,
        DIG_T2 = 0x8A,
        DIG_T3 = 0x8C,
        DIG_P1 = 0x8E,
        DIG_P2 = 0x90,
        DIG_P3 = 0x92,
        DIG_P4 = 0x94,
        DIG_P5 = 0x96,
        DIG_P6 = 0x98,
        DIG_P7 = 0x9A,
        DIG_P8 = 0x9C,
        DIG_P9 = 0x9E,
        DIG_H1 = 0xA1,
        DIG_H2 = 0xE1,
        DIG_H3 = 0xE3,
        DIG_H_E4 = 0xE4,
        DIG_H_E5 = 0xE5,
        DIG_H_E6 = 0xE6,
        DIG_H6 = 0xE7,

        VERSION = 0xD1,
        SOFTRESET = 0xE0,
        STATUS = 0xF3,
        CHIPID = 0xD0,
        CONTROL_HUM = 0xF2,
        CONTROL = 0xF4,
        CONFIG = 0xF5,
        PRESSUREDATA = 0xF7,
        TEMPDATA = 0xFA,
        HUMDATA = 0xFD,
    };

    // Oversampling
    enum Oversampling {
        SKIPPED = 0,
        OVERSAMPLING_X1 = 1,
        OVERSAMPLING_X2,
        OVERSAMPLING_X4,
        OVERSAMPLING_X8,
        OVERSAMPLING_X16,
    };

    // Mode
    enum Mode {
        SLEEPMODE = 0,
        FORCEDMODE = 1,
        NORMALMODE = 3,
    };

    struct BME280 {
        I2C_HandleTypeDef *i2c;
        int32_t t_fine;
        uint16_t t1, p1;
        uint8_t address;
        uint8_t h1, h3;
        int8_t h6;
        int16_t t2, t3, p2, p3, p4, p5, p6, p7, p8, p9, h2, h4, h5;
};

uint8_t BME280_init (BME280 &bme, I2C_HandleTypeDef *i2c, uint8_t address);
void BME280_read_temp (BME280 &bme, float &temperature);
uint8_t BME280_read_pressure (BME280 &bme, float &pressure);
uint8_t BME280_read_humidity (BME280 &bme, float &humidity);
}

#endif /* BME_280_H_ */
