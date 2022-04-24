#ifndef ADS1115_H
#define ADS1115_H

#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/i2c.h>

enum deviceRegister
{
    conversionReg    = 0b00,           // Conversion register
    configurationReg = 0b01,           // Configuration register
    loThreshold   = 0b10,           // Lo_thresh register
    hiThreshold   = 0b11            // Hi_thresh register
};

enum readOperation                  // Bit 15 (1 bit long) in configuration register (reading)
{
    busy          = 0b0 << 15,      // Conversion not ready
    ready         = 0b1 << 15       // Conversion ready to read
};

enum writeOperation                 // Bit 15 (1 bit long) in configuration register (writing)
{
    noop          = 0b0 << 15,      // Not requesting anything
    convert       = 0b1 << 15,      // Requesting conversion
};

enum multiplexer                    // Bits 12-14 (3 bits long) in configuration register
{
    diff_0_1      = 0b000 << 12,    // Differential P = AIN0, N = AIN1 (default)
    diff_0_3      = 0b001 << 12,    // Differential P = AIN0, N = AIN3
    diff_1_3      = 0b010 << 12,    // Differential P = AIN1, N = AIN3
    diff_2_3      = 0b011 << 12,    // Differential P = AIN2, N = AIN3
    single_0      = 0b100 << 12,    // Single-ended P = AIN0, N = GND
    single_1      = 0b101 << 12,    // Single-ended P = AIN1, N = GND
    single_2      = 0b110 << 12,    // Single-ended P = AIN2, N = GND
    single_3      = 0b111 << 12     // Single-ended P = AIN3, N = GND
};

enum gain                           // Bits 9-11 (3 bits long) in configuration register
{
    pga_6_144V    = 0b000 << 9,     // +/-6.144V range = Gain 2/3
    pga_4_096V    = 0b001 << 9,     // +/-4.096V range = Gain 1
    pga_2_048V    = 0b010 << 9,     // +/-2.048V range = Gain 2 (default)
    pga_1_024V    = 0b011 << 9,     // +/-1.024V range = Gain 4
    pga_0_512V    = 0b100 << 9,     // +/-0.512V range = Gain 8
    pga_0_256V    = 0b101 << 9,     // +/-0.256V range = Gain 16
    pga_0_256V2   = 0b110 << 9,     // not used
    pga_0_256V3   = 0b111 << 9      // not used
};

enum sampleMode                     // Bit 8 (1 bit long) in configuration register
{
    continuous    = 0b0 << 8,       // Continuous conversion mode
    single        = 0b1 << 8        // Power-down single-shot mode (default)
};

enum sampleRate                     // Bits 5-7 (3 bits long) in configuration register
{
    sps8          = 0b000 << 5,     // 8 samples per second
    sps16         = 0b001 << 5,     // 16 samples per second
    sps32         = 0b010 << 5,     // 32 samples per second
    sps64         = 0b011 << 5,     // 64 samples per second
    sps128        = 0b100 << 5,     // 128 samples per second (default)
    sps250        = 0b101 << 5,     // 250 samples per second
    sps475        = 0b110 << 5,     // 475 samples per second
    sps860        = 0b111 << 5,     // 860 samples per second
};

enum comparatorMode                 // Bit 4 (1 bit long) in configuration register
{
    traditional   = 0b0 << 4,       // Traditional comparator (default)
    window        = 0b1 << 4        // Window comparator
};

enum alertPolarity                  // Bit 3 (1 bit long) in configuration register
{
    activeLow     = 0b0 << 3,       // Polarity of the ALERT/RDY pin active low (default)
    activeHigh    = 0b1 << 3        // Polarity of the ALERT/RDY pin active high
};

enum latchMode                      // Bit 2 (1 bit long) in configuration register
{
    nonLatching   = 0b0 << 2,       // The ALERT/RDY pin does not latch when asserted (default)
    latching      = 0b1 << 2        // The asserted ALERT/RDY pin remains latched until data is read
};

enum alertMode                      // Bits 0-1 (2 bits long) in configuration register
{
    alert1        = 0b00 << 0,      // Assert ALERT/RDY after one conversions
    alert2        = 0b01 << 0,      // Assert ALERT/RDY after two conversions
    alert4        = 0b10 << 0,      // Assert ALERT/RDY after four conversions
    none          = 0b11 << 0       // Disable the comparator and put ALERT/RDY in high state (default)
};

const uint8_t DEVICE_ADDRESS[] =    // I2C device addresses
{
    0x48,                           // ADDR tied to GND
    0x49,                           // ADDR tied to VDD
    0x4A,                           // ADDR tied to SDA
    0x4B                            // ADDR tied to SCL
};

const uint16_t SELECT_CHANNEL[] =   // Channel selection commands indexed by channel
{
    single_0,          // Select channel 0 in multiplexer
    single_1,          // Select channel 1 in multiplexer
    single_2,          // Select channel 2 in multiplexer
    single_3           // Select channel 3 in multiplexer
};

const int DELAYS[] =                // Poll delays for available rates in microseconds
{
    1000 + 1000 * 1000 / 8,
    1000 + 1000 * 1000 / 16,
    1000 + 1000 * 1000 / 32,
    1000 + 1000 * 1000 / 64,
    1000 + 1000 * 1000 / 128,
    1000 + 1000 * 1000 / 250,
    1000 + 1000 * 1000 / 475,
    1000 + 1000 * 1000 / 860
};

#endif ADS1115_H