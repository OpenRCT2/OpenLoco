#include "Vehicle.h"
#include "../Audio/Audio.h"
#include "../Config.h"
#include "../Graphics/Gfx.h"
#include "../Interop/Interop.hpp"
#include "../Map/TileManager.h"
#include "../Objects/ObjectManager.h"
#include "../Objects/VehicleObject.h"
#include "../OpenLoco.h"
#include "../Things/ThingManager.h"
#include "../Utility/Numeric.hpp"
#include "../ViewportManager.h"
#include <algorithm>
#include <cassert>

using namespace OpenLoco::Interop;
using namespace OpenLoco::Literals;

namespace OpenLoco::Vehicles
{
#pragma pack(push, 1)
    // There are some common elements in the vehicle components at various offsets these can be accessed via vehicle_base
    struct VehicleCommon : vehicle_base
    {
        uint8_t pad_20;
        company_id_t owner; // 0x21
        uint8_t pad_22[0x26 - 0x22];
        thing_id_t head; // 0x26
        uint16_t var_28; // 0x28
        uint8_t pad_2A[0x2C - 0x2A];
        uint16_t var_2C;     // 0x2C
        uint16_t var_2E;     // 0x2E
        int16_t tile_x;      // 0x30
        int16_t tile_y;      // 0x32
        uint8_t tile_base_z; // 0x34
        uint8_t track_type;  // 0x35 field same in all vehicles
        uint16_t var_36;     // 0x36 field same in all vehicles
        uint8_t var_38;      // 0x38
        uint8_t pad_39;
        thing_id_t next_car_id; // 0x3A
        uint8_t pad_3C[0x42 - 0x3C];
        TransportMode mode; // 0x42 field same in all vehicles
    };
    static_assert(sizeof(VehicleCommon) == 0x43); // Can't use offset_of change this to last field if more found
#pragma pack(pop)

    vehicle_base* vehicle_base::nextVehicle()
    {
        return ThingManager::get<vehicle_base>(next_thing_id);
    }

    vehicle_base* vehicle_base::nextVehicleComponent()
    {
        auto* veh = reinterpret_cast<VehicleCommon*>(this);
        return ThingManager::get<vehicle_base>(veh->next_car_id);
    }

    TransportMode vehicle_base::getTransportMode() const
    {
        const auto* veh = reinterpret_cast<const VehicleCommon*>(this);
        return veh->mode;
    }

    uint8_t vehicle_base::getOwner() const
    {
        const auto* veh = reinterpret_cast<const VehicleCommon*>(this);
        return veh->owner;
    }

    uint8_t vehicle_base::getFlags38() const
    {
        const auto* veh = reinterpret_cast<const VehicleCommon*>(this);
        return veh->var_38;
    }

    uint8_t vehicle_base::getTrackType() const
    {
        const auto* veh = reinterpret_cast<const VehicleCommon*>(this);
        return veh->track_type;
    }

    uint16_t vehicle_base::getVar36() const
    {
        const auto* veh = reinterpret_cast<const VehicleCommon*>(this);
        return veh->var_36;
    }

    thing_id_t vehicle_base::getHead() const
    {
        const auto* veh = reinterpret_cast<const VehicleCommon*>(this);
        return veh->head;
    }

    void vehicle_base::setNextCar(const thing_id_t newNextCar)
    {
        auto* veh = reinterpret_cast<VehicleCommon*>(this);
        veh->next_car_id = newNextCar;
    }

    bool vehicle_base::updateComponent()
    {
        int32_t result = 0;
        registers regs;
        regs.esi = (int32_t)this;
        switch (getSubType())
        {
            case VehicleThingType::head:
                result = asVehicleHead()->update();
                break;
            case VehicleThingType::vehicle_1:
                result = call(0x004A9788, regs);
                break;
            case VehicleThingType::vehicle_2:
                result = call(0x004A9B0B, regs);
                break;
            case VehicleThingType::bogie:
                result = call(0x004AA008, regs);
                break;
            case VehicleThingType::body_start:
            case VehicleThingType::body_continued:
                result = asVehicleBody()->update();
                break;
            case VehicleThingType::tail:
                result = call(0x004AA24A, regs);
                break;
            default:
                break;
        }
        return (result & (1 << 8)) != 0;
    }

    CarComponent::CarComponent(vehicle_base*& component)
    {
        front = component->asVehicleBogie();
        back = front->nextVehicleComponent()->asVehicleBogie();
        body = back->nextVehicleComponent()->asVehicleBody();
        component = body->nextVehicleComponent();
    }

    Vehicle::Vehicle(uint16_t _head)
    {
        auto component = ThingManager::get<vehicle_base>(_head);
        if (component == nullptr)
        {
            throw std::runtime_error("Bad vehicle structure");
        }
        head = component->asVehicleHead();
        veh1 = head->nextVehicleComponent()->asVehicle1();
        veh2 = veh1->nextVehicleComponent()->asVehicle2();
        component = veh2->nextVehicleComponent();
        if (component->getSubType() != VehicleThingType::tail)
        {
            cars = Cars{ Car{ component } };
        }
        while (component->getSubType() != VehicleThingType::tail)
        {
            component = component->nextVehicleComponent();
        }
        tail = component->asVehicleTail();
    }
}