#pragma once

#include "../Localisation/StringManager.h"

namespace OpenLoco
{
#pragma pack(push, 1)
    struct tunnel_object
    {
        string_id name;
        uint32_t image; // 0x02
    };
#pragma pack(pop)
}