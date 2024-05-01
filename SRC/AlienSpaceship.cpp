#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "AlienSpaceship.h"
#include "BoundingSphere.h"

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/**  Default constructor. */
AlienSpaceship::AlienSpaceship()
	: GameObject("AlienSpaceship"), mAlienThrust(0)
{
	mAlienShielded = true;
}

/** Construct a spaceship with given position, velocity, acceleration, angle, and rotation. */
AlienSpaceship::AlienSpaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("AlienSpaceship", p, v, a, h, r), mAlienThrust(0)
{
}

/** Copy constructor. */
AlienSpaceship::AlienSpaceship(const AlienSpaceship& s)
	: GameObject(s), mAlienThrust(0)
{
}

/** Destructor. */
AlienSpaceship::~AlienSpaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Update this spaceship. */
void AlienSpaceship::Update(int t)
{
	// Call parent update function
	GameObject::Update(t);

	// Update acceleration based on thrust and angle
	mAcceleration.x = mAlienThrust * cos(DEG2RAD * mAngle);
	mAcceleration.y = mAlienThrust * sin(DEG2RAD * mAngle);
}

/** Render this spaceship. */
void AlienSpaceship::Render(void)
{
	if (mAlienSpaceshipShape.get() != NULL) mAlienSpaceshipShape->Render();

	// If ship is thrusting
	if ((mAlienShielded) && (mAlienThrusterShape.get() != NULL)) {
		mAlienThrusterShape->Render();
	}

	GameObject::Render();
}

/** Fire the rockets. */
void AlienSpaceship::Thrust(float t)
{
	mAlienThrust = t;

}

/** Set the rotation. */
void AlienSpaceship::Rotate(float r)
{
	mRotation = r;
}


/** Shoot a bullet. */
void AlienSpaceship::Shoot(void)
{
	// Check the world exists
	if (!mWorld) return;
	// Construct a unit length vector in the direction the spaceship is headed
	GLVector3f spaceship_heading(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
	spaceship_heading.normalize();
	// Calculate the point at the node of the spaceship from position and heading
	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	// Calculate how fast the bullet should travel
	float bullet_speed = 30;
	// Construct a vector for the bullet's velocity
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;
	// Construct a new bullet
	shared_ptr<GameObject> bullet
	(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mAlienBulletShape);
	// Add the new bullet to the game world
	mWorld->AddObject(bullet);

}

bool AlienSpaceship::CollisionTest(shared_ptr<GameObject> o)
{
	if (o->GetType() == GameObjectType("Asteroid") || (o->GetType() != GameObjectType("Shield") || (o->GetType() != GameObjectType("Life")))) return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void AlienSpaceship::OnCollision(const GameObjectList& objects)
{
	for (const auto& obj : objects) {
		if (obj->GetType() == GameObjectType("Spaceship") || (obj->GetType() == GameObjectType("Bullet"))) {
			// Check if the shield is active
			if (mAlienShielded) {
				// Remove shield
				mAlienShielded = false;
			}
			else {
				// Shield is not active, handle death
				mWorld->FlagForRemoval(GetThisPtr());
				mAlienShielded = true;
			}
		}
	}
}