#pragma once
#include "node.h"
class Light;
class FloorLamp : public Node
{
public:
			FloorLamp();
virtual	~FloorLamp();
	Light*	create();
private:
};

