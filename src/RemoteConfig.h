#ifndef REMOTE_CONFIG_H_
#define REMOTE_CONFIG_H_

#include <stdint.h>

//=============================================================================
// Decoded remote control packet
//=============================================================================

struct RemotePacket
{
    uint32_t frame;      // Complete raw frame data
    uint32_t address;    // Decoded address field
    uint8_t key;         // Decoded command field
    bool detected;       // Valid frame received
};

//=============================================================================
// Timing structures
//=============================================================================

struct Range
{
    uint16_t min;               // Minimum allowed value
    uint16_t max;               // Maximum allowed value
};

struct PulseTiming
{
    Range highTime;             // Valid HIGH pulse duration range
    Range lowTime;              // Valid LOW pulse duration range
};

struct TimingTable
{
    PulseTiming sync;           // Synchronization pulse timing
    PulseTiming bit0;           // Logic '0' pulse timing
    PulseTiming bit1;           // Logic '1' pulse timing
};

//=============================================================================
// Remote protocol configuration structure
//=============================================================================

struct RemoteConfig
{
    uint8_t totalBits;          // Total number of bits in a frame
    uint8_t addressBits;        // Number of address bits
    uint8_t keyBits;            // Number of command bits

    uint16_t pulseWidthUs;      // Base pulse width (T) in microseconds
    uint8_t tolerancePercent;   // Allowed timing error percentage

    uint8_t syncHighRatio;      // Sync pulse HIGH duration multiplier
    uint8_t syncLowRatio;       // Sync pulse LOW duration multiplier

    uint8_t bit0HighRatio;      // Logic '0' HIGH duration multiplier
    uint8_t bit0LowRatio;       // Logic '0' LOW duration multiplier

    uint8_t bit1HighRatio;      // Logic '1' HIGH duration multiplier
    uint8_t bit1LowRatio;       // Logic '1' LOW duration multiplier
};

//=============================================================================
// EV1527 Family
//=============================================================================

constexpr RemoteConfig EV1527_CONFIG =
{
    24,
    20,
    4,

    320,
    20,

    1, 31,

    1, 3,

    3, 1
};

constexpr RemoteConfig HS1527_CONFIG = EV1527_CONFIG;
constexpr RemoteConfig RT1527_CONFIG = EV1527_CONFIG;
constexpr RemoteConfig FP1527_CONFIG = EV1527_CONFIG;

//=============================================================================
// PT2262 Family
//=============================================================================

constexpr RemoteConfig PT2262_CONFIG =
{
    24,
    20,
    4,

    350,
    20,

    1, 31,

    1, 3,

    3, 1
};

constexpr RemoteConfig SC2262_CONFIG = PT2262_CONFIG;
constexpr RemoteConfig LX2262_CONFIG = PT2262_CONFIG;

//=============================================================================
// Holtek Family
//=============================================================================

constexpr RemoteConfig HT12E_CONFIG =
{
    12,
    8,
    4,

    350,
    20,

    1, 31,

    1, 3,

    3, 1
};

constexpr RemoteConfig HT6P20_CONFIG =
{
    28,
    24,
    4,

    350,
    20,

    1, 31,

    1, 3,

    3, 1
};

constexpr RemoteConfig HT6P20B_CONFIG = HT6P20_CONFIG;

//=============================================================================
// Motorola Family
//=============================================================================

constexpr RemoteConfig MC145026_CONFIG =
{
    24,
    20,
    4,

    350,
    20,

    1, 31,

    1, 3,

    3, 1
};

constexpr RemoteConfig MC145027_CONFIG = MC145026_CONFIG;
constexpr RemoteConfig MC145028_CONFIG = MC145026_CONFIG;

#endif /* REMOTE_CONFIG_H_ */