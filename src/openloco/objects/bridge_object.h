#pragma once

#include "../types.hpp"

namespace openloco
{
#pragma pack(push, 1)
    struct bridge_object
    {
        string_id name;
        uint8_t no_roof; // 0x02
        uint8_t pad_03[0x08 - 0x03];
        uint8_t span_length;         // 0x08
        uint8_t pillar_spacing;      // 0x09
        uint16_t max_speed;          // 0x0A
        uint8_t max_height;          // 0x0C
        uint8_t cost_index;          // 0x0D
        uint16_t base_cost_factor;   // 0x0E
        uint16_t height_cost_factor; // 0x10
        uint16_t sell_cost_factor;   // 0x12
        uint16_t disabled_track_cfg; // 0x14
        uint8_t pad_16[0x22 - 0x16];
        uint8_t var_22;
        uint8_t var_23;
        uint8_t pad_24[0x2A - 0x24];
        uint16_t designed_year; //0x2A
    };
#pragma pack(pop)
}
