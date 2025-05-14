#include "entity.h"

EntityList::EntityList(HANDLE driverHandle, uintptr_t client)
    : driver(driverHandle), clientBase(client) {
}

bool Entity::isValid() const {
    return health > 0 && health <= 100 && address != 0;
}

std::vector<Entity> EntityList::getEntities() const {
    std::vector<Entity> entities;
    uintptr_t entity_list = driver::read_memory<uintptr_t>(driver, clientBase + offsets::dwEntityList);

    for (int i = 0; i < 32; ++i) {
        uintptr_t listEntry = driver::read_memory<uintptr_t>(driver, entity_list + (8 * (i & 0x7FFF) >> 9) + 16);
        if (!listEntry)
            continue;

        uintptr_t player = driver::read_memory<uintptr_t>(driver, listEntry + 120 * (i & 0x1FF));
        if (!player)
            continue;

        int health = driver::read_memory<int>(driver, player + offsets::m_iHealth);
        int team = driver::read_memory<int>(driver, player + offsets::m_iTeamNum);
        Vector3 origin = driver::read_memory<Vector3>(driver, player + offsets::m_vOldOrigin);
        Vector3 head = { origin.x, origin.y, origin.z + 75.f };

        Entity entity = { player, team, health, origin, head };
        if (entity.isValid())
            entities.push_back(entity);
    }

    return entities;
}
