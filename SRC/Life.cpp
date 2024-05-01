#include <stdlib.h>
#include "GameUtil.h"
#include "Life.h"
#include "BoundingShape.h"

Life::Life(void) : GameObject("Life")
{
	mAngle = rand() % 360;
	mRotation = rand() % 90;
	mPosition.x = rand() / 2;
	mPosition.y = rand() / 2;
	mPosition.z = 0.0;
	mVelocity.x = 2.0 * cos(DEG2RAD * mAngle);
	mVelocity.y = 2.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

Life::~Life(void)
{
}

void Life::Render(void)
{
	// Disable lighting 
	glDisable(GL_LIGHTING);
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.2, 0.4);
	for (int i = 0; i < 360; i++) {
		float degInRad = i * M_PI / 180;
		glVertex2f(2 + cos(degInRad) * 4, -2 + sin(degInRad) * 4);
	}
	glEnd();
	//enable lighting
	glEnable(GL_LIGHTING);
}

bool Life::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() != GameObjectType("Spaceship")) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Life::OnCollision(const GameObjectList& objects)
{
	mWorld->FlagForRemoval(GetThisPtr());
}