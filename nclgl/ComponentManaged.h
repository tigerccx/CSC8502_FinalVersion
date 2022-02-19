#pragma once

#include "Component.h"
class ComponentManaged:
	public Component
{
public:
	ComponentManaged() :Component() {
		isManaged = true;
	}
};