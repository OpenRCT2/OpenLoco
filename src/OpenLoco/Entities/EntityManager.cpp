#include "EntityManager.h"
#include "../Console.h"
#include "../Interop/Interop.hpp"
#include "../Localisation/StringIds.h"
#include "../Map/Tile.h"
#include "../OpenLoco.h"
#include "../Vehicles/Vehicle.h"

using namespace OpenLoco::Interop;

namespace OpenLoco::EntityManager
{
    loco_global<EntityId_t[numEntityLists], 0x00525E3E> _heads;
    loco_global<uint16_t[numEntityLists], 0x00525E4C> _listCounts;
    loco_global<Entity[maxEntities], 0x006DB6DC> _entities;
    loco_global<EntityId_t[0x40001], 0x01025A8C> _entitySpatialIndex;
    static loco_global<string_id, 0x009C68E6> gGameCommandErrorText;
    constexpr size_t _entitySpatialIndexNull = 0x40000;

    // 0x0046FDFD
    void reset()
    {
        call(0x0046FDFD);
    }

    EntityId_t firstId(EntityListType list)
    {
        return _heads[(size_t)list];
    }

    uint16_t getListCount(const EntityListType list)
    {
        return _listCounts[static_cast<size_t>(list)];
    }

    template<>
    Vehicles::VehicleHead* first()
    {
        return get<Vehicles::VehicleHead>(firstId(EntityListType::vehicleHead));
    }

    template<>
    EntityBase* get(EntityId_t id)
    {
        EntityBase* result = nullptr;
        if (id < maxEntities)
        {
            return &_entities.get()[id];
        }
        return result;
    }

    constexpr size_t getSpatialIndexOffset(const Map::map_pos& loc)
    {
        size_t index = _entitySpatialIndexNull;
        if (loc.x != Location::null)
        {
            uint16_t x = loc.x & 0x3FE0;
            uint16_t y = loc.y & 0x3FE0;

            index = (x << 4) | (y >> 5);
        }

        if (index >= 0x40001)
        {
            return _entitySpatialIndexNull;
        }
        return index;
    }

    EntityId_t firstQuadrantId(const Map::map_pos& loc)
    {
        auto index = getSpatialIndexOffset(loc);
        return _entitySpatialIndex[index];
    }

    static EntityBase* createEntity(EntityId_t id, EntityListType list)
    {
        auto* newEntity = get<EntityBase>(id);
        moveEntityToList(newEntity, list);

        newEntity->x = Location::null;
        newEntity->y = Location::null;
        newEntity->z = 0;
        auto index = getSpatialIndexOffset({ Location::null, Location::null });
        auto nextSpatialId = _entitySpatialIndex[index];
        _entitySpatialIndex[index] = newEntity->id;
        newEntity->nextQuadrantId = nextSpatialId;

        newEntity->name = StringIds::empty_pop;
        newEntity->var_14 = 16;
        newEntity->var_09 = 20;
        newEntity->var_15 = 8;
        newEntity->var_0C = 0;
        newEntity->sprite_left = Location::null;

        return newEntity;
    }

    // 0x004700A5
    EntityBase* createEntityMisc()
    {
        if (getListCount(EntityListType::misc) >= 4000)
        {
            return nullptr;
        }
        if (getListCount(EntityListType::null) <= 0)
        {
            return nullptr;
        }

        auto newId = _heads[static_cast<uint8_t>(EntityListType::null)];
        return createEntity(newId, EntityListType::misc);
    }

    // 0x0047011C
    EntityBase* createEntityMoney()
    {
        if (getListCount(EntityListType::nullMoney) <= 0)
        {
            return nullptr;
        }

        auto newId = _heads[static_cast<uint8_t>(EntityListType::nullMoney)];
        return createEntity(newId, EntityListType::misc);
    }

    // 0x00470039
    EntityBase* EntityManager::createEntityVehicle()
    {
        if (getListCount(EntityListType::null) <= 0)
        {
            return nullptr;
        }

        auto newId = _heads[static_cast<uint8_t>(EntityListType::null)];
        return createEntity(newId, EntityListType::vehicle);
    }

    // 0x0047024A
    void freeEntity(EntityBase* const entity)
    {
        registers regs;
        regs.esi = reinterpret_cast<uint32_t>(entity);
        call(0x0047024A, regs);
    }

    // 0x004A8826
    void updateVehicles()
    {
        if ((addr<0x00525E28, uint32_t>() & 1) && !isEditorMode())
        {
            for (auto v : VehicleList())
            {
                v->updateVehicle();
            }
        }
    }

    // 0x004402F4
    void updateMiscEntities()
    {
        call(0x004402F4);
    }

    // 0x0047019F
    void moveEntityToList(EntityBase* const entity, const EntityListType list)
    {
        auto newListOffset = static_cast<uint8_t>(list) * 2;
        if (entity->linkedListOffset == newListOffset)
        {
            return;
        }

        auto curList = entity->linkedListOffset / 2;
        auto nextId = entity->next_thing_id;
        auto previousId = entity->llPreviousId;

        // Unlink previous entity from this entity
        if (previousId == EntityId::null)
        {
            _heads[curList] = nextId;
        }
        else
        {
            auto* previousEntity = get<EntityBase>(previousId);
            if (previousEntity == nullptr)
            {
                Console::error("Invalid previous entity id. Entity linked list corrupted?");
            }
            else
            {
                previousEntity->next_thing_id = nextId;
            }
        }
        // Unlink next entity from this entity
        if (nextId != EntityId::null)
        {
            auto* nextEntity = get<EntityBase>(nextId);
            if (nextEntity == nullptr)
            {
                Console::error("Invalid next entity id. Entity linked list corrupted?");
            }
            else
            {
                nextEntity->llPreviousId = previousId;
            }
        }

        entity->llPreviousId = EntityId::null;
        entity->linkedListOffset = newListOffset;
        entity->next_thing_id = _heads[static_cast<uint8_t>(list)];
        _heads[static_cast<uint8_t>(list)] = entity->id;
        // Link next entity to this entity
        if (entity->next_thing_id != EntityId::null)
        {
            auto* nextEntity = get<EntityBase>(entity->next_thing_id);
            if (nextEntity == nullptr)
            {
                Console::error("Invalid next entity id. Entity linked list corrupted?");
            }
            else
            {
                nextEntity->llPreviousId = entity->id;
            }
        }

        _listCounts[curList]--;
        _listCounts[static_cast<uint8_t>(list)]++;
    }

    // 0x00470188
    bool checkNumFreeEntities(const size_t numNewEntities)
    {
        if (EntityManager::getListCount(EntityManager::EntityListType::null) <= numNewEntities)
        {
            gGameCommandErrorText = StringIds::too_many_objects_in_game;
            return false;
        }
        return true;
    }

    // 0x0046FED5
    void zeroUnused()
    {
        for (auto* ent : EntityList<EntityListIterator<EntityBase>, EntityListType::null>())
        {
            auto next = ent->next_thing_id;
            auto id = ent->id;
            std::fill_n(reinterpret_cast<uint8_t*>(ent), sizeof(Entity), 0);
            ent->base_type = EntityBaseType::null;
            ent->next_thing_id = next;
            ent->id = id;
            ent->linkedListOffset = static_cast<uint8_t>(EntityListType::null) * 2;
        }
        for (auto* ent : EntityList<EntityListIterator<EntityBase>, EntityListType::nullMoney>())
        {
            auto next = ent->next_thing_id;
            auto id = ent->id;
            std::fill_n(reinterpret_cast<uint8_t*>(ent), sizeof(Entity), 0);
            ent->base_type = EntityBaseType::null;
            ent->next_thing_id = next;
            ent->id = id;
            ent->linkedListOffset = static_cast<uint8_t>(EntityListType::nullMoney) * 2;
        }
    }
}
