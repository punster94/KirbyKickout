#pragma once
/*
	PlayerManager.h		written by Louis Hofer

	This is a singleton class that is responsible for managing the state and interaction between all players it instantiates.
*/

#include "Player.h"
#include "types.h"

#define MAX_NUMBER_OF_PLAYERS 4

class PlayerManagerC
{
public:
	/* Public functions */
	static PlayerManagerC	*CreateInstance();
	static PlayerManagerC	*GetInstance() { return sInstance; };
	~PlayerManagerC() {};

	void init();
	void update(DWORD milliseconds);
	void render();
	void shutdown();

	PlayerC* getPlayer(int playerNumber);

	/* Public data members */
	bool mGameOver;
	int mWinner;

private:
	/* Private functions */
	PlayerManagerC() {};

	void handleGameOver(int playersLeft);
	void handlePauseMenu();
	void applyAttacks(PlayerC *player);
	void renderPlayers();
	void renderPauseScreen();

	bool collidesWithPlayer(PlayerC *attacker, PlayerC *defender);
	bool boxesIntersect(Coord2D topLeftA, Coord2D bottomRightA, Coord2D topLeftB, Coord2D bottomRightB);

	Coord2D offsetCoordinate(Coord2D coordinate, Coord2D offset);

	/* Private data members */
	bool mPaused;
	bool mLoaded = false;

	int mPausedBy;
	int mNumberOfPlayers;

	static PlayerManagerC *sInstance;

	PlayerC *mPlayerArray[MAX_NUMBER_OF_PLAYERS];

	SpriteC *mPauseScreenSprite;
	SpriteC *mDigits;

	XINPUT_STATE mPreviousControllerStates[4];

	/* Private constant data */
	const float pauseScreenStartX = -256.0f;
	const float pauseScreenStartY = 192.0f;
	const float playerSpriteHeight = 144.0f;
	const float playerSpriteWidth = 144.0f;
	const float playerSpeed = 0.3f;

	const float spawnXLocations[4] = { -450, 360, -180, 90 };
	const float spawnYLocations[4] = { -50, -50, -50, -50 };

	const char *tilePath = "Screens/Player";
	const char *spritePath = "SpriteSheets/KirbySpriteSheet";
	const char *fileType = ".png";

	char *digitsPath = "SpriteSheets/digits.png";
	char *pauseScreenPath = "Screens/Pause.png";
	
	Coord2D pauseScreenPosition;
};