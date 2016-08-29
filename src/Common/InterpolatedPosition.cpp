#include "InterpolatedPosition.h"

InterpolatedPosition::InterpolatedPosition()
{
	timeSinceLastTarget = 0;
}

InterpolatedPosition::InterpolatedPosition(float initalTargetX, float initalTargetY)
{
	timeSinceLastTarget = 0;
	target.x = initalTargetX;
	target.y = initalTargetY;

	start = target;
	current = target;
}

InterpolatedPosition::~InterpolatedPosition()
{
}

void InterpolatedPosition::setNewTarget(float newTargetX, float newTargetY)
{
	if (newTargetX != target.x || newTargetY != target.y)
	{
		start = current;
		target.x = newTargetX;
		target.y = newTargetY;
		timeSinceLastTarget = 0;
	}
}

Position InterpolatedPosition::getNextPosition(float deltaTMS, float TotalTMS)
{
	timeSinceLastTarget += deltaTMS;

	if (timeSinceLastTarget > TotalTMS)
	{
		return target;
	}
	else
	{
		float T = (timeSinceLastTarget / TotalTMS);
		float oneMinusT = (1 - T);
		float newX = oneMinusT*start.x + T*target.x;
		float newY = oneMinusT*start.y + T*target.y;

		current.x = newX;
		current.y = newY;

		return current;
	}
}