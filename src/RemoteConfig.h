#ifndef REMOTE_CONFIG_H_
#define REMOTE_CONFIG_H_

#include <stdint.h>

//=============================================================================
// Supported Protocols
//=============================================================================

// enum class RemoteProtocol : uint8_t
// {
//     EV1527,
//     HS1527,
//     PT2262,
//     CUSTOM
// };

//=============================================================================
// Protocol Configuration
//=============================================================================

typedef struct
{
    uint8_t totalBits;

    uint8_t addressBits;
    uint8_t keyBits;

    uint16_t minTime;
    uint16_t maxTime;

    uint8_t syncMinRatio;
    uint8_t syncMaxRatio;

    uint8_t bit0MinRatio;
    uint8_t bit0MaxRatio;

    uint8_t bit1MinRatio;
    uint8_t bit1MaxRatio;

} RemoteConfig;

//=============================================================================
// Decoded Data
//=============================================================================

struct RemotePacket
{
    uint32_t frame;
    uint32_t address;
    uint8_t key;
    bool detected;
};
//=============================================================================
// Predefined Protocol Configurations
//=============================================================================

constexpr RemoteConfig EV1527_CONFIG =
    {
        24, // totalBits

        20, // addressBits
        4,  // keyBits

        18,   // minTime
        1500, // maxTime

        25, // syncMinRatio
        37, // syncMaxRatio

        2, // bit0MinRatio
        4, // bit0MaxRatio

        2, // bit1MinRatio
        4  // bit1MaxRatio
};

constexpr RemoteConfig HS1527_CONFIG = EV1527_CONFIG;

#endif /* REMOTE_CONFIG_H_ */