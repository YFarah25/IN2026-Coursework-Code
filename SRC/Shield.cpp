#include <stdlib.h>
#include "GameUtil.h"
#include "Shield.h"
#include "BoundingShape.h"

Shield::Shield(void) : GameObject("Shield")
{
	mAngle = rand() % 360;
	mRotation = rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 4.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 4.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

Shield::~Shield(void)
{
}

void Shield::Render(void)
{
	// Disable lighting 
	glDisable(GL_LIGHTING);
	glBegin(GL_TRIANGLES);
	glColor3f(0.4, 0.6, 1.0);
	//bottom two and top point of triangle
	glVertex2f(-4, -6);
	glVertex2f(6, -4);
	glVertex2f(0, 4);
	glEnd();
	//enable lighting
	glEnable(GL_LIGHTING);
}

bool Shield::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Shield::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}