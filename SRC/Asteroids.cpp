#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "Shield.h"
#include "Life.h"
#include "AlienSpaceship.h"


// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mGameStarted = false;
	list<shared_ptr<GameObject>>gameObjects;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);


	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	// Create a spaceship and add it to the world
	//mGameWorld->AddObject(CreateSpaceship());
	// Create some asteroids and add them to the world
	//CreateAsteroids(10);

	// Create a shield powerup and add to world
	//CreateShield();

	// Create a life powerup and add to world
	//CreateLife();

	//Create the GUI
	CreateGUI();

	mGameWorld->AddObject(CreateAlienSpaceship());


	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (!mGameStarted) {
		switch (key) {
			// Switch from start screen to the game starting 
		case ' ':

			mGameStarted = true;
			mStartGameLabel->SetVisible(false);
			mTitleLabel->SetVisible(false);
			mScoreLabel->SetVisible(true);
			mLivesLabel->SetVisible(true);
			mLevelLabel->SetVisible(true);


			// Create a spaceship and add it to the world
			mGameWorld->AddObject(CreateSpaceship());
			mScoreKeeper.mScore = 0;
			mPlayer.mLives = 3;

			// Shield on spawn should expire after 2 seconds
			SetTimer(2000, SHIELD_EXPIRE);
			//ClearGameObjects();
			CreateAsteroids(10);

			// Create a shield powerup and add to world
			CreateShield();

			// Create a life powerup and add to world
			CreateLife();


	
			break;
		default:
			break;
		}
	}
	switch (key)
	{
	case ' ':
		mSpaceship->Shoot();
		break;
		// Quit game
	case 'q':
		Stop();
		break;

	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mGameStarted)
	{
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_DOWN: mSpaceship->Thrust(-10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(120); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-120); break;
			// Default case - do nothing
		default: break;
		}
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (mGameStarted)
	{
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If down arrow key is released stop applying forward thrust
		case GLUT_KEY_DOWN: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
	{
		gameObjects.remove(object);
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0 && mPlayer.mLives > 0)
		{
			SetTimer(2000, START_NEXT_LEVEL);
		}
	}

	if (object->GetType() == GameObjectType("Shield") && mSpaceship && mPlayer.mLives > 0)
	{
		if (mSpaceship->mShielded == false)
		{
			mSpaceship->mShielded = true;
		}
		
		SetTimer(5000, SPAWN_SHIELD);
	}

	if (object->GetType() == GameObjectType("Life") && mSpaceship && mPlayer.mLives > 0)
	{
		mPlayer.mLives += 1;
		// Format the lives left message using an string-based stream
		std::ostringstream msg_stream;
		msg_stream << "LIVES: " << mPlayer.mLives;
		// Get the lives left message as a string
		std::string lives_msg = msg_stream.str();
		mLivesLabel->SetText(lives_msg);

		SetTimer(5000, SPAWN_LIFE);

	}

}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
			mSpaceship->Reset();
			mGameWorld->AddObject(mSpaceship);
			// Shield on spawn should expire after 2 seconds
			SetTimer(2000, SHIELD_EXPIRE);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		UpdateLevel(mLevel);
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
		// Only give temporary shield if the player isn't already shielded
		if (!mSpaceship->mShielded) {
			mSpaceship->SetShielded(true);
			// Shield on new level should expire after 2 seconds
			SetTimer(2000, SHIELD_EXPIRE);
		}

	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
		//mLivesLabel->SetVisible(false);
		ClearGameObjects();
		// Loops back to start screen
		SetTimer(3000, SHOW_NEW_GAME);
	}
	// Start screen after game over screen
	if (value == SHOW_NEW_GAME)
	{
		mGameOverLabel->SetVisible(false);
		mLivesLabel->SetVisible(false);
		mScoreLabel->SetVisible(false);
		mLevelLabel->SetVisible(false);
		mStartGameLabel->SetVisible(true);
		mTitleLabel->SetVisible(true);
		mScoreKeeper.mScore = 0;
		mLevel = 0;
		UpdateLevel(mLevel);
		mPlayer.mLives = 3;
		mLivesLabel->SetText("LIVES: 3");
		mScoreLabel->SetText("SCORE: 0");
		// Readd score keeper to the game world
		mGameWorld->AddListener(&mScoreKeeper);

		mGameStarted = false;
	}

	if (value == SHIELD_EXPIRE)
	{
		mSpaceship->SetShielded(false);
	}

	if (value == SPAWN_SHIELD)
	{
		CreateShield();
	}

	if (value == SPAWN_LIFE)
	{
		CreateLife();
	}
			

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
    // shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> spaceship_shape = make_shared<Shape>("spaceship.shape");
	shared_ptr<Shape> shield_shape = make_shared<Shape>("shield.shape");
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetSpaceshipShape(spaceship_shape);
	mSpaceship->SetShieldShape(shield_shape);
	mSpaceship->SetBulletShape(bullet_shape);

	/*Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);*/

	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;


}

shared_ptr<GameObject> Asteroids::CreateAlienSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mAlienSpaceship = make_shared<AlienSpaceship>();
	mAlienSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mAlienSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> alien_bullet_shape = make_shared<Shape>("bullet.shape");
	mAlienSpaceship->SetBulletShape(alien_bullet_shape);
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> alien_spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mAlienSpaceship->SetSprite(alien_spaceship_sprite);
	mAlienSpaceship->SetScale(0.1f);


	/*Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);*/

	// Reset spaceship back to centre of the world
	mAlienSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mAlienSpaceship;


}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		gameObjects.push_back(asteroid);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::ClearGameObjects() {
	// Remove score listener so that removed asteroids don't add to score
	mGameWorld->RemoveListener(&mScoreKeeper);
	// Flag asteroids and powerups for removal
	for (auto i = gameObjects.begin(); i != gameObjects.end(); i++) {
		mGameWorld->FlagForRemoval(*i);

		//if (i->get()->GetType() == GameObjectType("Asteroid")) {}
	}

}

void Asteroids::CreateShield()
{
	mShield = make_shared<Shield>();
	mShield->SetBoundingShape(make_shared<BoundingSphere>(mShield->GetThisPtr(), 4.0f));
	// Add shield to list of game objects
	gameObjects.push_back(mShield);
	mGameWorld->AddObject(mShield);
}

void Asteroids::CreateLife()
{
	mLife = make_shared<Life>();
	mLife->SetBoundingShape(make_shared<BoundingSphere>(mLife->GetThisPtr(), 3.0f));
	gameObjects.push_back(mLife);
	mGameWorld->AddObject(mLife);
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("SCORE: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Set the visibility of the label to false (hidden)
	mScoreLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 0.95f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLevelLabel = make_shared<GUILabel>("LEVEL: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Set the visibility of the label to false (hidden)
	mLevelLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> level_component = static_pointer_cast<GUIComponent>(mLevelLabel);
	mGameDisplay->GetContainer()->AddComponent(level_component, GLVector2f(0.75f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mStartGameLabel = shared_ptr<GUILabel>(new GUILabel("PRESS SPACE TO START"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mStartGameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mStartGameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mStartGameLabel->SetVisible(true);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> start_game_component
		= static_pointer_cast<GUIComponent>(mStartGameLabel);
	mGameDisplay->GetContainer()->AddComponent(start_game_component, GLVector2f(0.5f, 0.5f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("LIVES: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Set the visibility of the label to false (hidden)
	mLivesLabel->SetVisible(false);
	// Set the color of the label to RGB value as a Vector3f
	mLivesLabel->SetColor(GLVector3f(1.0f, 0.2f, 0.4f));
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mTitleLabel = shared_ptr<GUILabel>(new GUILabel("ASTEROIDS"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mTitleLabel->SetVisible(true);
	// Set the color of the label to RGB value as a Vector3f
	mTitleLabel->SetColor(GLVector3f(0.4f, 0.6f, 1.0f));
	// Set the size of the label
	//mTitleLabel->SetSize(GLVector2i(20, 20));
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> title_component
		= static_pointer_cast<GUIComponent>(mTitleLabel);
	mGameDisplay->GetContainer()->AddComponent(title_component, GLVector2f(0.5f, 0.75f));

}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "SCORE: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::UpdateLevel(int level)
{
	std::ostringstream msg_stream;
	msg_stream << "LEVEL: " << level;
	// Get the score message as a string
	std::string level_msg = msg_stream.str();
	mLevelLabel->SetText(level_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "LIVES: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




