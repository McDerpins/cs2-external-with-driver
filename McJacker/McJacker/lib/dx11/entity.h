#pragma once
#include "../pch.h"
#include "../util/vector.h"
#include "../namespace/namespace.h"
#include "../offsets/offsets.h"

struct Entity {
    uintptr_t address;
    int team;
    int health;
    Vector3 origin;
    Vector3 head;

    bool isValid() const;
};

class EntityList {
public:
    EntityList(HANDLE driver, uintptr_t clientBase);

    std::vector<Entity> getEntities() const;

private:
    HANDLE driver;
    uintptr_t clientBase;
};
