#pragma once

#include "Common.h"

class InterpolatedPosition
{
public:
	InterpolatedPosition();
	InterpolatedPosition(float initalTargetX, float initalTargetY);
	~InterpolatedPosition();

	void setNewTarget(float newTargetX, float newTargetY);
	Position getNextPosition(float deltaTMS, float TotalTMS);

private:
	Position start;
	Position current;
	Position target;

	float timeSinceLastTarget;
};

