#pragma  once
/*
	Player.cpp		written by Louis Hofer

	This file contains the implementation for functions prototyped in the PlayerC class.
*/

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <Xinput.h>
#include "openGLFramework.h"
#include "Object.h"
#include "Player.h"
#include "glut.h"
#include "SOIL.h"
#include "SoundManager.h"

/* Public functions */
/*
	Instantiates the sprites a player is responsible for.
	Loads the sprite sheets for a player into memory.
*/
PlayerC::PlayerC(char *spritePath, char *tilePath, SpriteC *digits, float spriteHeight, float spriteWidth, float initPosX, float initPosY, float initVelX, float initVelY, int id, float speed)
{
	mId = id;
	mPixelsPerMillisecondSpeed = speed;
	mSpriteHandler = new SpriteC(spritePath, spriteHeight, spriteWidth, rowsInSpriteSheet, columnsInSpriteSheet);
	mPlayerTile = new SpriteC(tilePath, playerTileHeight, playerTileWidth, 1, 1);
	mDigits = digits;

	reset(initPosX, initPosY, initVelX, initVelY);
}

PlayerC::~PlayerC()
{
	delete mSpriteHandler;
	delete mPlayerTile;
};

/*
	Updates the player when their controller is connected.
*/
void PlayerC::update(DWORD milliseconds)
{
	if (isConnected())
	{
		updateAnimationFrameTime(milliseconds);

		handleDamageDelay(milliseconds);
		handleActionDelay(milliseconds);

		handleFalling();

		applyVelocity(milliseconds);
		handleCollision();

		updateSprite();

		mPreviousControllerState = mControllerState;
	}
}

/*
	Renders the player's associated sprites at its current position.
*/
void PlayerC::render()
{
	mSpriteHandler->render(mPosition, mU, mV);

	mPlayerTile->render(mTilePosition, 0, 0, false);

	drawHealthDigits();
}

/*
	Sets the player's data members to their starting values.
*/
void PlayerC::reset(float x, float y, float vX, float vY)
{
	mVelocity.x = vX;
	mVelocity.y = vY;
	mPosition.x = x;
	mPosition.y = y;

	mTilePosition.x = x;
	mTilePosition.y = tileHeight;

	if (mId % 2)
		mLastDirectionalInput = -deadValue - 1;
	else
	{
		mLastDirectionalInput = deadValue + 1;
		mTilePosition.x += mSpriteHandler->mWidth - evenPlayerTileWidthOffset;
	}

	mIsJumping = true;
	mIsDoubleJumping = true;
	mUseGravity = true;
	mIsDashing = false;
	mDead = false;
	mAttacking = false;
	mBeingHit = false;

	mV = 0;
	mCurrentActionDelay = 0;
	mCurrentAnimationFrame = 0;
	mCurrentFrameMilliseconds = 100.0f;
	mDamageDelay = 0;
	mHealth = 100;
	mLastDamageTaken = 0;
	mLastDamageDealt = 0;

	changeSpriteState((mId % 2) + Jump);
	mLastAction = PlayerAction::Invalid;
}

/*
	Sets the vibration motors on the player's controller.
*/
void PlayerC::vibrate(int leftVal, int rightVal)
{
	XINPUT_VIBRATION vibration;

	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	vibration.wLeftMotorSpeed = leftVal;
	vibration.wRightMotorSpeed = rightVal;

	XInputSetState(mId, &vibration);
}

/*
	Determines whether a player's controller is connected or not.
	Sets the state of the controller in the process.
*/
BOOL PlayerC::isConnected()
{
	ZeroMemory(&mControllerState, sizeof(mControllerState));
	DWORD state = XInputGetState(mId, &mControllerState);

	return state == ERROR_SUCCESS;
}

/*
	Gets the controller state, setting mControllerState and returning the value.
*/
XINPUT_STATE PlayerC::getControllerState()
{
	ZeroMemory(&mControllerState, sizeof(mControllerState));

	XInputGetState(mId, &mControllerState);

	return mControllerState;
}

XINPUT_STATE PlayerC::getPreviousControllerState()
{
	return mPreviousControllerState;
}

Coord2D PlayerC::getPosition()
{
	return mPosition;
}

/* Private functions */
/*
	Sets the player's last action type to invalid in the frame after attacking or dealing damage.
*/
void PlayerC::resetAttackCollision()
{
	if (mAttacking)
	{
		mLastAction = PlayerAction::Invalid;
		mAttacking = false;
	}

	if (mLastAction == PlayerAction::Damaged)
	{
		mLastAction = PlayerAction::Invalid;
	}
}

/*
	Determines whether to allow the player to be hit by checking the current damage delay.
	Vibrates the controller if the player is unable to take damage.
*/
void PlayerC::handleDamageDelay(DWORD milliseconds)
{
	mDamageDelay -= milliseconds;

	if (mDamageDelay <= 0)
	{
		vibrate(0, 0);
		mDamageDelay = 0;
		handleBeingHit();
	}
	else
	{
		vibrate(leftVibration, rightVibration);
		mBeingHit = false;
	}
}

/*
	Determines whether to allow the player to take an action by checking the current action delay.
	If the player is in currently in an animation delay they will not be able to perform certain actions.
*/
void PlayerC::handleActionDelay(DWORD milliseconds)
{
	mCurrentActionDelay -= milliseconds;

	if (mCurrentActionDelay <= 0)
	{
		mCurrentActionDelay = 0;
		resetAttackCollision();

		handleHorizontalMovement();
		handleActions();
		setAttacking();
		handleJumping();
	}
}

/*
	Damages the player and triggers an animation if the player is being hit.
*/
void PlayerC::handleBeingHit()
{
	if (mBeingHit)
	{
		mBeingHit = false;
		mVelocity.x = 0;

		int index = Damage;

		if (mLastDirectionalInput <= 0)
			index += 1;

		changeSpriteState(index);

		mLastAction = PlayerAction::Damaged;
		mHealth -= mLastDamageTaken;

		if (mHealth <= 0)
			mDead = true;
	}
}

/*
	Checks player input to see if they are initiating a jump, sets their y velocity and triggers an animation if so.
*/
void PlayerC::handleJumping()
{
	if (mControllerState.Gamepad.sThumbLY > deadValue && !mIsDoubleJumping && !(mPreviousControllerState.Gamepad.sThumbLY > deadValue))
	{
		if (mIsJumping)
		{
			mIsDoubleJumping = true;
		}
		else
		{
			mIsJumping = true;
			mHeightBeforeJump = mPosition.y;
		}

		mVelocity.y = jumpSpeed;

		applyAction(Jump);
	}
}

/*
	Applies gravity if the player has reached the peak of their jump or double jump.
*/
void PlayerC::handleFalling()
{
	if (!mUseGravity)
	{
		if (mIsDoubleJumping && mPosition.y >= (mHeightBeforeJump + jumpHeight + jumpHeight))
		{
			mUseGravity = true;
			mVelocity.y = 0;
		}
		else if (mIsJumping && mPosition.y >= (mHeightBeforeJump + jumpHeight))
		{
			mUseGravity = true;
			mVelocity.y = 0;
		}
	}

	if (mUseGravity)
	{
		if (mControllerState.Gamepad.sThumbLY < -deadValue && mControllerState.Gamepad.sThumbLX < deadValue && mControllerState.Gamepad.sThumbLX > -deadValue && mCurrentActionDelay <= 0)
			mVelocity.y = -terminalVelocity;
		else
			mVelocity.y -= gravityAcceleration;
	}
}

/*
	Sets the appropriate animation and velocity depending on the directional inputs of the player.
*/
void PlayerC::handleHorizontalMovement()
{
	if (mControllerState.Gamepad.sThumbLX < -deadValue)
	{
		if (mIsDashing || inputToDash())
		{
			mVelocity.x = -2 * mPixelsPerMillisecondSpeed;
		}
		else
		{
			mVelocity.x = -1 * mPixelsPerMillisecondSpeed;
		}

		if (!mIsJumping && mIsDashing)
			changeSpriteState(Dash + 1);
		else if (!mIsJumping)
			changeSpriteState(Walk + 1);
		else
			changeSpriteState(Jump + 1);

		mLastDirectionalInput = mControllerState.Gamepad.sThumbLX;
	}
	else if (mControllerState.Gamepad.sThumbLX > deadValue)
	{
		if (mIsDashing || inputToDash())
		{
			mVelocity.x = 2 * mPixelsPerMillisecondSpeed;
		}
		else
		{
			mVelocity.x = mPixelsPerMillisecondSpeed;
		}

		if (!mIsJumping && mIsDashing)
			changeSpriteState(Dash);
		else if (!mIsJumping)
			changeSpriteState(Walk);
		else
			changeSpriteState(Jump);

		mLastDirectionalInput = mControllerState.Gamepad.sThumbLX;
	}
	else
	{
		mVelocity.x = 0;
		mIsDashing = false;

		if (!mIsJumping)
		{
			if (mLastDirectionalInput > deadValue)
				changeSpriteState(Stand);
			else
				changeSpriteState(Stand + 1);
		}
	}
}

/*
	Prevents the player from moving out of the bounds of the screen.
*/
void PlayerC::handleCollision()
{
	if (mPosition.y >= (ceilingHeight - mSpriteHandler->mHitBoxStart.y))
	{
		mPosition.y = ceilingHeight - mSpriteHandler->mHitBoxStart.y;
	}

	if (mPosition.y <= (floorHeight - mSpriteHandler->mHitBoxEnd.y))
	{
		mPosition.y = floorHeight - mSpriteHandler->mHitBoxEnd.y;
		mVelocity.y = 0;

		if (mIsJumping)
		{
			int index = Stand;

			if (mLastDirectionalInput < -deadValue)
			{
				index += 1;
			}

			SoundManagerC::GetInstance()->playKirbySound(index);
			changeSpriteState(index);
		}

		mIsJumping = false;
		mIsDoubleJumping = false;
		mUseGravity = false;
	}

	if (mPosition.x <= (leftBound - mSpriteHandler->mHitBoxStart.x))
	{
		mPosition.x = leftBound - mSpriteHandler->mHitBoxStart.x;
		mVelocity.x = 0;
	}

	if (mPosition.x >= (rightBound - mSpriteHandler->mHitBoxEnd.x))
	{
		mPosition.x = rightBound - mSpriteHandler->mHitBoxEnd.x;
		mVelocity.x = 0;
	}
}

/*
	Applies the player's current velocity to its position based on the delta time.
*/
void PlayerC::applyVelocity(DWORD milliseconds)
{
	if (mVelocity.y > terminalVelocity)
		mVelocity.y = terminalVelocity;

	if (mVelocity.y < -terminalVelocity)
		mVelocity.y = -terminalVelocity;

	mPosition.x = mPosition.x + (mVelocity.x * milliseconds);
	mPosition.y = mPosition.y + (mVelocity.y * milliseconds);
}

/*
	Sets the action of the player this frame and initiates their animation if applicable.
*/
void PlayerC::handleActions()
{
	mLastAction = PlayerAction::Invalid;

	if (mControllerState.Gamepad.wButtons & (XINPUT_GAMEPAD_X | XINPUT_GAMEPAD_Y))
	{
		mVelocity.x = 0;
		changeSpriteState(Taunting);
		mLastAction = PlayerAction::Taunt;

		SoundManagerC::GetInstance()->playKirbySound(Taunting);
	}
	else if (mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A)
	{
		if (mControllerState.Gamepad.sThumbLY > deadValue && !mIsJumping)
		{
			mVelocity.x = 0;
			mLastDamageDealt = largeDamage;

			applyAction(UpAttack);
		}
		else if (mControllerState.Gamepad.sThumbLY < -deadValue && !mIsJumping)
		{
			mVelocity.x = 0;
			mLastDamageDealt = largeDamage;

			applyAction(DownAttack);
		}
		else if (mIsDashing && !mIsJumping && !(mPreviousControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
		{
			mIsDashing = false;
			mLastDamageDealt = smallDamage;

			applyAction(DashAttack);
		}
		else if ((mControllerState.Gamepad.sThumbLX < -deadValue || mControllerState.Gamepad.sThumbLX > deadValue) && !mIsJumping)
		{
			mVelocity.x = 0;
			mLastDamageDealt = largeDamage;

			applyAction(SideAttack);
		}
		else if (!mIsJumping)
		{
			mVelocity.x = 0;
			mLastDamageDealt = smallDamage;

			applyAction(RapidPunch);
		}
		else if (mIsJumping && !(mPreviousControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A))
		{
			mLastDamageDealt = largeDamage;

			applyAction(Aerial);
		}

		mLastAction = PlayerAction::Attack;
	}
	else if (mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		if (mControllerState.Gamepad.sThumbLY < -deadValue)
		{
			mVelocity.x = 0;
			mVelocity.y = -terminalVelocity;

			mIsJumping = false;
			mIsDoubleJumping = false;

			mLastDamageDealt = smallDamage;

			changeSpriteState(DownSpecial);

			SoundManagerC::GetInstance()->playKirbySound(DownSpecial);

			mLastAction = PlayerAction::Special;
		}
		else if (mControllerState.Gamepad.sThumbLY > deadValue)
		{
			mIsJumping = true;
			mIsDoubleJumping = true;
			mVelocity.y = jumpSpeed * 1.5f;

			mLastDamageDealt = smallDamage;

			applyAction(UpSpecial);
			mLastAction = PlayerAction::Special;
		}
		else if (mControllerState.Gamepad.sThumbLX > deadValue || mControllerState.Gamepad.sThumbLX < -deadValue)
		{
			mVelocity.x = 0;

			if (!mIsJumping)
				mVelocity.y = -terminalVelocity;

			mLastDamageDealt = smallDamage;

			applyAction(SideSpecial);
			mLastAction = PlayerAction::Special;
		}
	}
	else if (mControllerState.Gamepad.bLeftTrigger > triggerDeadValue || mControllerState.Gamepad.bRightTrigger > triggerDeadValue)
	{
		if (mControllerState.Gamepad.sThumbLX > deadValue || mControllerState.Gamepad.sThumbLX < -deadValue)
		{
			applyAction(DodgeRoll);
		}
		else
		{
			mVelocity.x = 0;
			applyAction(Block);
		}

		mLastAction = PlayerAction::Dodge;
	}
}

/*
	Applies the action specified by the index to the sprite and plays any corresponding sounds.
*/
void PlayerC::applyAction(int spriteStartIndex)
{
	if (mLastDirectionalInput <= 0)
		spriteStartIndex += 1;

	changeSpriteState(spriteStartIndex);

	SoundManagerC::GetInstance()->playKirbySound(spriteStartIndex);
}

/*
	Sets the public attacking flag on the player if the action chosen involves damaging an opponent.
*/
void PlayerC::setAttacking()
{
	if (mLastAction == PlayerAction::Attack || mLastAction == PlayerAction::Special)
	{
		mAttacking = true;
	}
}

/*
	Sets the current animation frame, sprite hitbox and animation times based on the given action index.
*/
void PlayerC::changeSpriteState(int u)
{
	mV = mCurrentAnimationFrame;
	mU = u;

	mSpriteHandler->mHitBoxStart.x = collisionStartX[u];
	mSpriteHandler->mHitBoxStart.y = collisionStartY[u];
	mSpriteHandler->mHitBoxEnd.x = collisionEndX[u];
	mSpriteHandler->mHitBoxEnd.y = collisionEndY[u];

	setAnimationTimes(u);
}

/*
	Sets the duration and speed of an animation as well as it's I-frames.
*/
void PlayerC::setAnimationTimes(int u)
{
	if (animationDurations[u] != 0)
		mCurrentActionDelay = animationDurations[u];

	if (animationSpeeds[u] != 0)
		mMillisecondsPerFrame = animationSpeeds[u];

	if (damageDelays[u] != 0)
		mDamageDelay = damageDelays[u];
}

/*
	Sets the new sprite index and records the previous one.
*/
void PlayerC::updateSprite()
{
	mV = mCurrentAnimationFrame;

	if (mLastU != mU)
	{
		mCurrentAnimationFrame = 0;
	}

	mLastU = mU;
}

/*
	Iterates the animation to the next frame in the set if the frame duration is over.
*/
void PlayerC::updateAnimationFrameTime(DWORD milliseconds)
{
	mCurrentFrameMilliseconds += milliseconds;

	if (mCurrentFrameMilliseconds >= mMillisecondsPerFrame)
	{
		mCurrentFrameMilliseconds = 0;
		mCurrentAnimationFrame = (mCurrentAnimationFrame + 1) % mSpriteHandler->getRows();
	}
}

/*
	Determines whether the player has entered to dash horizontally or not.
	Sets the isDashing data member.
*/
bool PlayerC::inputToDash()
{
	bool result = false;

	if (!mIsJumping && mLastDirectionalInput > deadValue && mControllerState.Gamepad.sThumbLX > deadValue && mPreviousControllerState.Gamepad.sThumbLX < deadValue)
	{
		result = true;
		mIsDashing = true;
		SoundManagerC::GetInstance()->playKirbySound(Dash);
	}
	else if (!mIsJumping && mLastDirectionalInput < -deadValue && mControllerState.Gamepad.sThumbLX < -deadValue && mPreviousControllerState.Gamepad.sThumbLX > -deadValue)
	{
		result = true;
		mIsDashing = true;
		SoundManagerC::GetInstance()->playKirbySound(Dash + 1);
	}

	return result;
}

/*
	Renders the sprite that contains digits 0-9 and blank three times based on the player's current health.
*/
void PlayerC::drawHealthDigits()
{
	Coord2D digitPosition = mTilePosition;
	int hundredsU = 0, tensU = 0, onesU = 0;

	if (mId % 2)
	{
		digitPosition.x += evenPlayerDigitXOffset;
		digitPosition.y -= evenPlayerDigitYOffset;
	}
	else
	{
		digitPosition.x += oddPlayerDigitXOffset;
		digitPosition.y -= oddPlayerDigitYOffset;
	}

	hundredsU = mHealth / 100;

	tensU = (mHealth - (100 * hundredsU)) / 10;

	onesU = mHealth % 10;

	if (hundredsU == 0)
	{
		hundredsU = 10;

		if (tensU == 0)
		{
			tensU = 10;
		}
	}

	mDigits->render(digitPosition, hundredsU, 0, false);
	digitPosition.x += (digitWidth - 1);
	mDigits->render(digitPosition, tensU, 0, false);
	digitPosition.x += (digitWidth - 1);
	mDigits->render(digitPosition, onesU, 0, false);
}