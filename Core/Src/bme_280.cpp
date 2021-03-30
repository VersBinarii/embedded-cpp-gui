/*
 * bme_280.c
 *
 *  Created on: Mar 23, 2021
 *      Author: kris
 */

#include "bme_280.h"

namespace BME {

constexpr uint16_t I2C_TIMEOUT = 1000;

static void write_8 (BME280 &bme, Register reg, uint8_t value) {

    HAL_I2C_Mem_Write (bme.i2c, bme.address, static_cast<uint8_t> (reg), 1,
                       &value, 1, I2C_TIMEOUT);
}

static uint8_t read_8 (BME280 &bme, Register reg) {
    uint8_t value;

    HAL_I2C_Mem_Read (bme.i2c, bme.address, static_cast<uint8_t> (reg), 1,
                      &value, 1, I2C_TIMEOUT);
    return value;
}

static uint16_t read_16 (BME280 &bme, Register reg) {
    uint8_t value[2];

    HAL_I2C_Mem_Read (bme.i2c, bme.address, static_cast<uint8_t> (reg), 1,
                      value, 2, I2C_TIMEOUT);
    return value[1] << 8 | value[0];
}

static uint16_t read_16_h (BME280 &bme, Register reg) {
    uint8_t value[2];

    HAL_I2C_Mem_Read (bme.i2c, bme.address, static_cast<uint8_t> (reg), 1,
                      value, 2, I2C_TIMEOUT);
    return value[0] << 8 | value[1];
}

static uint32_t read_24 (BME280 &bme, Register reg) {
    uint8_t value[3];

    HAL_I2C_Mem_Read (bme.i2c, bme.address, static_cast<uint8_t> (reg), 1,
                      value, 2, I2C_TIMEOUT);
    return value[0] << 16 | value[1] << 8 | value[2];
}
static void set_mode (BME280 &bme, Mode mode) {
    uint8_t r_tmp = 0;

    r_tmp = read_8 (bme, Register::CONTROL);
    r_tmp &= 0xfc; // Clear bits 0 and 1
    r_tmp |= mode & 0x03;
    write_8 (bme, Register::CONTROL, r_tmp);
}

static void set_pressure_oversampling (BME280 &bme, Oversampling o) {
    uint8_t r_tmp = 0;
    r_tmp = read_8 (bme, Register::CONTROL);
    r_tmp &= 0xe3; // Clear bits 2,3,4
    r_tmp |= (o << 2) & 0x1c;
    write_8 (bme, Register::CONTROL, r_tmp);
}

static void set_temp_oversampling (BME280 &bme, Oversampling o) {
    uint8_t r_tmp = 0;
    r_tmp = read_8 (bme, Register::CONTROL);
    r_tmp &= 0x1f; // Clear bits 5,6,7
    r_tmp |= (o << 5) & 0xe0;
    write_8 (bme, Register::CONTROL, r_tmp);
}

static void set_hum_oversampling (BME280 &bme, Oversampling o) {
    uint8_t r_tmp = 0;
    r_tmp = read_8 (bme, Register::CONTROL_HUM);
    r_tmp &= 0xf8; // Clear bits 0,1,2
    r_tmp |= o & 0x07;
    write_8 (bme, Register::CONTROL_HUM, r_tmp);
}

static int32_t read_temp_raw (BME280 &bme) {
    int32_t tmp = 0;
    tmp = read_24 (bme, Register::TEMPDATA);
    // Align the bits
    return tmp >> 4;
}

static int32_t read_pressure_raw (BME280 &bme) {
    int32_t tmp = 0;
    tmp = read_24 (bme, Register::PRESSUREDATA);

    // Align the bits
    return tmp >> 4;
}

static int16_t read_hum_raw (BME280 &bme) {
    int16_t tmp = 0;
    tmp = read_16_h (bme, Register::HUMDATA);

    return tmp;
}

void BME280_read_temp (BME280 &bme, float &temperature) {
    int32_t var1, var2, T;
    int32_t adc_T = read_temp_raw (bme);

    var1
        = ((((adc_T >> 3) - ((int32_t) (bme.t1) << 1))) * ((int32_t) (bme.t2)))
          >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t) (bme.t1)))
              * ((adc_T >> 4) - ((int32_t) (bme.t1))))
             >> 12)
            * ((int32_t) (bme.t3)))
           >> 14;
    bme.t_fine = var1 + var2;
    T = ((bme.t_fine) * 5 + 128) >> 8;

    temperature = (float)(T / 100.0);
}

uint8_t BME280_read_pressure (BME280 &bme, float &pressure) {
    int32_t var1 = 0, var2 = 0;
    uint32_t p = 0;
    uint32_t adc_p = read_pressure_raw (bme);

    var1 = (((int32_t)bme.t_fine) >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)bme.p6);
    var2 = var2 + ((var1 * ((int32_t)bme.p5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)bme.p4) << 16);
    var1 = (((bme.p3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3)
            + ((((int32_t)bme.p2) * var1) >> 1))
           >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)bme.p1)) >> 15);
    if (var1 == 0) {
        return 1; // avoid exception caused by division by zero
    }
    p = (((uint32_t) (((int32_t)1048576) - adc_p) - (var2 >> 12))) * 3125;
    if (p < 0x80000000) {
        p = (p << 1) / ((uint32_t)var1);
    } else {
        p = (p / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)bme.p9) * ((int32_t) (((p >> 3) * (p >> 3)) >> 13)))
           >> 12;
    var2 = (((int32_t) (p >> 2)) * ((int32_t)bme.p8)) >> 13;
    p = (uint32_t) ((int32_t)p + ((var1 + var2 + bme.p7) >> 4));
    pressure = (float)(p / 100.0);
    return 0;
}

uint8_t BME280_read_humidity (BME280 &bme, float &humidity) {
    int32_t adc_H = read_hum_raw (bme);
    float var_H = (((float)bme.t_fine) - 76800.0);
    var_H = (adc_H
             - (((float)bme.h4) * 64.0 + ((float)bme.h5) / 16384.0 * var_H))
            * (((float)bme.h2) / 65536.0
               * (1.0
                  + ((float)bme.h6) / 67108864.0 * var_H
                        * (1.0 + ((float)bme.h3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((float)bme.h1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    humidity = var_H;
    return 0;
}

uint8_t BME280_init (BME280 &bme, I2C_HandleTypeDef *i2c, uint8_t address) {
    uint8_t chip_id = 0;

    bme.i2c = i2c;
    bme.address = address << 1;
    bme.t_fine = 0;
    chip_id = read_8 (bme, Register::CHIPID);

    if (chip_id != 0x60) {
        return 1;
    }

    bme.t1 = read_16 (bme, Register::DIG_T1);
    bme.t2 = read_16 (bme, Register::DIG_T2);
    bme.t3 = read_16 (bme, Register::DIG_T3);

    bme.p1 = read_16 (bme, Register::DIG_P1);
    bme.p2 = read_16 (bme, Register::DIG_P2);
    bme.p3 = read_16 (bme, Register::DIG_P3);
    bme.p4 = read_16 (bme, Register::DIG_P4);
    bme.p5 = read_16 (bme, Register::DIG_P5);
    bme.p6 = read_16 (bme, Register::DIG_P6);
    bme.p7 = read_16 (bme, Register::DIG_P7);
    bme.p8 = read_16 (bme, Register::DIG_P8);
    bme.p9 = read_16 (bme, Register::DIG_P9);

    bme.h1 = read_8 (bme, Register::DIG_H1);
    bme.h2 = (int16_t)read_16 (bme, Register::DIG_H2);
    bme.h3 = read_8 (bme, Register::DIG_H3);

    // Register e5 is shared between H4 and H5
    int8_t he4 = (int8_t)read_8 (bme, Register::DIG_H_E4);
    int8_t he5 = (int8_t)read_8 (bme, Register::DIG_H_E5);
    int8_t he6 = (int8_t)read_8 (bme, Register::DIG_H_E6);
    bme.h4 = ((int16_t) (he4 << 4) | (int16_t) (he5 & 0x0f));
    bme.h5 = (int16_t) (he6 << 4) | (int16_t) ((he5 >> 4) & 0x0f);
    bme.h6 = (int8_t)read_8 (bme, Register::DIG_H6);

    set_hum_oversampling (bme, OVERSAMPLING_X1);
    set_pressure_oversampling (bme, OVERSAMPLING_X16);
    set_temp_oversampling (bme, OVERSAMPLING_X2);
    set_mode (bme, NORMALMODE);

    return 0;
}
}
