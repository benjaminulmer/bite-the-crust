#pragma 
#include "Vehicle.h"
#include <PxPhysicsAPI.h>

class WheelEntity :
	public Entity
{
public:
	WheelEntity(Vehicle* parent, physx::PxShape* physicsShape);
	virtual ~WheelEntity(void);

	glm::vec3 getPosition();
	glm::mat4 getModelMatrix();

private:
	Vehicle* vehicle;
	physx::PxShape* physicsShape;
};

