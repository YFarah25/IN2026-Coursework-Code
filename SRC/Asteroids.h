#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h" 
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"

class GameObject;
class Spaceship;
class AlienSpaceship;
class GUILabel;
class Shield;
class Life;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	Asteroids(int argc, char *argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////

	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////

	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////

	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////

	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<AlienSpaceship> mAlienSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	shared_ptr<GUILabel> mStartGameLabel;
	shared_ptr<GUILabel> mTitleLabel;
	shared_ptr<GUILabel> mLevelLabel;


	
	shared_ptr<Shield> mShield;
	void CreateShield();

	shared_ptr<Life> mLife;
	void CreateLife();

	void UpdateLevel(int level);


	uint mLevel;
	uint mAsteroidCount;
	bool mGameStarted;
	list<shared_ptr<GameObject>>gameObjects;

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	shared_ptr<GameObject> CreateAlienSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	void ClearGameObjects();
	shared_ptr<GameObject> CreateExplosion();
	
	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;
	const static uint SHOW_NEW_GAME = 3;
	const static uint SHIELD_EXPIRE = 4;
	const static uint SPAWN_SHIELD = 5;
	const static uint SPAWN_LIFE = 6;
	const static uint SPAWN_ALIEN = 7;
	const static uint MOVE_ALIEN = 8;
	const static uint ALIEN_DODGE = 9;


	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif