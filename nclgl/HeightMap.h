#pragma once

#include <string>

#include "Mesh.h"

using namespace std;

class HeightMap :
    public Mesh
{
public:
    HeightMap(const string& name);
    virtual ~HeightMap() { };

    inline bool IsInit() const { return isInit; }
    inline Vector3 GetHeightmapSize() const { return heightmapSize; }
    
protected:
    Vector3 heightmapSize;
    bool isInit = false;
};

