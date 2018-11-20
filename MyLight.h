#pragma once

#include "../DXFramework/Light.h"
class MyLight : public Light
{
public:

	MyLight();
	~MyLight();

	// setters
	void setRange(float new_range) { range = new_range; }
	void setRadius(float new_radius) { radius = new_radius; }
	void setSpotInnerAngle(float new_angle) { spotInnerAngle = new_angle; }
	void setSpotOutterAngle(float new_angle) { spotOutterAngle = new_angle; }
	void setLightType(int light_type) { lightType = static_cast<LIGHT_TYPE>(light_type); }
	// getters
	float getRadius() { return radius; }
	float getRange() { return range; }
	float getSpotInnerAngle() { return spotInnerAngle; }
	float getSpotOutterAngle() { return spotOutterAngle; }
	int getLightType() { return static_cast<int>(lightType); }
private:
	enum LIGHT_TYPE
	{
		directional,
		point,
		spot
	};
	LIGHT_TYPE lightType;
	float radius;
	float range;
	float spotInnerAngle;
	float spotOutterAngle;
};

