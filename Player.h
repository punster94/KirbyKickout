#pragma once
/*
	Player.h		written by Louis Hofer

	This class is used to represent one of four possible players.
	This file and the associated class contains constants that are used by PlayerC instantiations to interact within the game.
*/

#include <windows.h>
#include <Xinput.h>
#include "types.h"
#include "Object.h"
#include "baseTypes.h"
#include "glut.h"
#include "Sprite.h"

/*
	Indexes into the sprite sheet for each animation type
*/
#define Stand 0
#define Walk 2
#define Jump 5
#define Dash 7
#define UpAttack 9
#define DownAttack 11
#define DashAttack 13
#define RapidPunch 15
#define SideAttack 17
#define Aerial 19
#define Damage 21
#define DodgeRoll 23
#define Block 25
#define Taunting 27
#define DownSpecial 28
#define UpSpecial 29
#define SideSpecial 31

/*
	Enumeration used to represent the type of action a player has taken.
*/
namespace PlayerAction
{
	enum PlayerAction { Invalid, TestAction, Attack, Special, Dodge, Taunt, Damaged, MaxState };
}

class PlayerC : ObjectC
{
public:
	/* Public Functions */
	PlayerC(char *spritePath, char *tilePath, SpriteC *digits, float spriteHeight, float spriteWidth, float initPosX, float initPosY, float initVelX, float initVelY, int id, float speed);
	~PlayerC();

	void update(DWORD milliseconds);
	void render();
	void reset(float x, float y, float vX, float vY);
	void vibrate(int leftVal = 0, int rightVal = 0);

	BOOL isConnected();
	XINPUT_STATE getControllerState();
	XINPUT_STATE getPreviousControllerState();
	Coord2D getPosition();

	/* Public data members */
	bool mDead;
	bool mBeingHit;
	bool mAttacking;

	int mId;
	int mLastDamageDealt;
	int mLastDamageTaken;

	SpriteC *mSpriteHandler;

	PlayerAction::PlayerAction mLastAction;
	
private:
	/* Private functions */
	void resetAttackCollision();
	void handleDamageDelay(DWORD milliseconds);
	void handleActionDelay(DWORD milliseconds);
	void handleBeingHit();
	void handleJumping();
	void handleFalling();
	void handleHorizontalMovement();
	void handleCollision();
	void applyVelocity(DWORD milliseconds);
	void handleActions();
	void applyAction(int spriteStartIndex);
	void setAttacking();
	void changeSpriteState(int u);
	void setAnimationTimes(int u);
	void updateSprite();
	void updateAnimationFrameTime(DWORD milliseconds);
	bool inputToDash();
	void drawHealthDigits();

	/* Private data members */
	bool mIsJumping;
	bool mIsDoubleJumping;
	bool mUseGravity;
	bool mIsDashing;

	int mHealth;
	int mCurrentAnimationFrame;

	float mPixelsPerMillisecondSpeed;
	float mLastDirectionalInput;
	float mHeightBeforeJump;
	float mCurrentActionDelay;
	float mDamageDelay;
	float mU, mV;
	float mLastU;
	float mCurrentFrameMilliseconds;
	float mMillisecondsPerFrame;

	SpriteC *mPlayerTile;
	SpriteC *mDigits;

	Coord2D mTilePosition;

	XINPUT_STATE mControllerState;
	XINPUT_STATE mPreviousControllerState;

	/* Private constant data */
	const BYTE triggerDeadValue = 100;

	const short deadValue = 15000;

	const int rowsInSpriteSheet = 11;
	const int columnsInSpriteSheet = 33;
	const int ceilingHeight = 380;
	const int floorHeight = -250;
	const int leftBound = -512;
	const int rightBound = 512;
	const int leftVibration = 16000;
	const int rightVibration = 8000;
	const int largeDamage = 10;
	const int smallDamage = 5;

	const float playerTileHeight = 71.0f;
	const float playerTileWidth = 130.0f;
	const float jumpHeight = 200.0f;
	const float terminalVelocity = 2.0f;
	const float gravityAcceleration = 0.04f;
	const float jumpSpeed = 1.15f;
	const float tileHeight = -300.0f;
	const float evenPlayerTileWidthOffset = 184.0f;
	const float evenPlayerDigitXOffset = 2.0f;
	const float evenPlayerDigitYOffset = 10.0f;
	const float oddPlayerDigitXOffset = 50.0f;
	const float oddPlayerDigitYOffset = 14.0f;
	const float digitWidth = 20.0f;

	const float collisionStartX[33] =
	{
		28.0f,28.0f,28.0f,28.0f,28.0f,28.0f,
		28.0f,28.0f,28.0f,14.0f,14.0f,-3.0f,
		-3.0f,28.0f,28.0f,80.0f,-16.0f,28.0f,
		-17.0f,14.0f,14.0f,28.0f,28.0f,28.0f,
		28.0f,28.0f,28.0f,28.0f,28.0f,40.0f,
		-17.0f,28.0f,-37.0f
	};

	const float collisionStartY[33] =
	{
		-55.0f,-55.0f,-55.0f,-55.0f,-55.0f,-55.0f,
		-55.0f,-55.0f,-55.0f,-30.0f,-30.0f,-55.0f,
		-55.0f,-55.0f,-55.0f,-55.0f,-55.0f,-55.0f,
		-55.0f,-25.0f,-25.0f,-55.0f,-55.0f,-55.0f,
		-55.0f,-55.0f,-55.0f,-55.0f,-55.0f,-20.0f,
		-20.0f,-55.0f,-55.0f
	};

	const float collisionEndX[33] =
	{
		115.0f,115.0f,115.0f,115.0f,115.0f,115.0f,
		115.0f,115.0f,115.0f,129.0f,129.0f,146.0f,
		146.0f,115.0f,115.0f,160.0f,64.0f,160.0f,
		115.0f,129.0f,129.0f,115.0f,115.0f,115.0f,
		115.0f,115.0f,115.0f,115.0f,115.0f,160.0f,
		103.0f,180.0f,115.0f
	};

	const float collisionEndY[33] =
	{
		-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,
		-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,
		-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,
		-144.0f,-114.0f,-114.0f,-144.0f,-144.0f,-144.0f,
		-144.0f,-144.0f,-144.0f,-144.0f,-144.0f,-124.0f,
		-124.0f,-134.0f,-134.0f
	};

	const float animationDurations[33] =
	{
		0,0,0,0,0,0,
		0,0,0,500.0f,500.0f,450.0f,
		450.0f,600.0f,600.0f,800.0f,800.0f,400.0f,
		400.0f,300.0f,300.0f,300.0f,300.0f,400.0f,
		400.0f,300.0f,300.0f,500.0f,1000.0f,1000.0f,
		1000.0f,1000.0f,1000.0f
	};

	const float animationSpeeds[33] =
	{
		100.0f,100.0f,100.0f,100.0f,100.0f,100.0,
		100.0f,50.0f,50.0f,100.0f,100.0f,70.0f,
		70.0f,50.0f,50.0f,100.0f,100.0f,100.0f,
		100.0f,70.0f,70.0f,50.0f,50.0f,70.0f,
		70.0f,70.0f,70.0f,100.0f,100.0f,70.0f,
		70.0f,70.0f,70.0f
	};

	const float damageDelays[33] =
	{
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,0,0,0,
		0,0,0,1000.0f,1000.0f,0,
		0,0,0,0,1000.0f,1000.0f,
		1000.0f,0,0
	};
};