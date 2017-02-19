/*
	PlayerManager.cpp		written by Louis Hofer

	This file contains the implementation for functions prototyped in the PlayerManagerC singleton class.
*/

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <Xinput.h>
#include <string.h>
#include "SOIL.h"
#include "openGLFramework.h"
#include "PlayerManager.h"
#include "ScreenManager.h"
#include "SoundManager.h"

PlayerManagerC* PlayerManagerC::sInstance = NULL;

/* Public functions */
PlayerManagerC* PlayerManagerC::CreateInstance()
{
	if (sInstance == NULL)
		sInstance = new PlayerManagerC();

	return sInstance;
}

/*
	Initializes the manager by instantiating the players and creating commonly used sprites 
*/
void PlayerManagerC::init()
{
	int i;

	if (!mLoaded)
	{
		mDigits = new SpriteC(digitsPath, 20.0f, 20.0f, 1, 11);
		mPauseScreenSprite = new SpriteC(pauseScreenPath, 384.0f, 512.0f, 1, 1);
	}

	for (i = 0; i < MAX_NUMBER_OF_PLAYERS; i++)
	{
		if (!mLoaded)
		{
			char tileFileName[30];
			char spriteSheetFileName[50];
			char numberComponent[2] = { (char)i + '0', 0 };
			strcpy(tileFileName, tilePath);
			strcat(tileFileName, numberComponent);
			strcat(tileFileName, fileType);

			strcpy(spriteSheetFileName, spritePath);
			strcat(spriteSheetFileName, numberComponent);
			strcat(spriteSheetFileName, fileType);

			mPlayerArray[i] = new PlayerC(spriteSheetFileName, tileFileName, mDigits, playerSpriteHeight, playerSpriteWidth, spawnXLocations[i], spawnYLocations[i], 0, 0, i, playerSpeed);
		}
		else
		{
			mPlayerArray[i]->reset(spawnXLocations[i], spawnYLocations[i], 0, 0);
		}

		if (mPlayerArray[i]->isConnected())
		{
			mPreviousControllerStates[i] = mPlayerArray[i]->getControllerState();
		}
	}

	mGameOver = false;
	mWinner = 0;

	mNumberOfPlayers = i;

	mPaused = false;
	mPausedBy = 0;

	pauseScreenPosition.x = pauseScreenStartX;
	pauseScreenPosition.y = pauseScreenStartY;

	mLoaded = true;
}

/*
	Updates players in game, applying pauses and attacks as necessary.
*/
void PlayerManagerC::update(DWORD milliseconds)
{
	int playersLeft = 0;

	for (int i = 0; i < mNumberOfPlayers; i++)
	{
		if (mPlayerArray[i]->isConnected() && !mPlayerArray[i]->mDead)
		{
			playersLeft++;
		}

		if (mPlayerArray[i]->isConnected() && !mPaused)
		{
			mPlayerArray[i]->update(milliseconds);
		}

		if (mPlayerArray[i]->mAttacking)
		{
			applyAttacks(mPlayerArray[i]);
		}
	}

	handleGameOver(playersLeft);

	handlePauseMenu();
}

void PlayerManagerC::render()
{
	renderPlayers();

	if (mPaused)
		renderPauseScreen();
}

void PlayerManagerC::shutdown()
{
	if (mLoaded)
	{
		for (int i = 0; i < mNumberOfPlayers; i++)
		{
			delete mPlayerArray[i];
		}

		delete mPauseScreenSprite;
		delete mDigits;
	}
}

PlayerC* PlayerManagerC::getPlayer(int playerNumber)
{
	assert(playerNumber <= (MAX_NUMBER_OF_PLAYERS - 1));

	return mPlayerArray[playerNumber];
}

/* Private functions */

/*
	If there are one or less players left the game is over and the winner is the one that isn't dead.
*/
void PlayerManagerC::handleGameOver(int playersLeft)
{
	if (playersLeft <= 1)
	{
		mGameOver = true;

		for (int i = 0; i < mNumberOfPlayers; i++)
		{
			if (mPlayerArray[i]->isConnected())
			{
				mPlayerArray[i]->vibrate(0, 0);

				if (!mPlayerArray[i]->mDead)
					mWinner = i;
			}
		}
	}
}

/*
	Manages the pause menu states and plays sounds when transitioning.
*/
void PlayerManagerC::handlePauseMenu()
{
	for (int i = 0; i < mNumberOfPlayers; i++)
	{
		PlayerC *player = mPlayerArray[i];

		if (player->isConnected())
		{
			XINPUT_STATE controllerState = player->getControllerState();

			if ((controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) && !(mPreviousControllerStates[i].Gamepad.wButtons & XINPUT_GAMEPAD_START))
			{
				if (mPaused && i == mPausedBy)
				{
					mPaused = false;

					SoundManagerC::GetInstance()->playCloseMenuSound();
				}
				else if (!mPaused)
				{
					mPaused = true;
					mPausedBy = i;

					SoundManagerC::GetInstance()->playMenuSound();
				}
			}
			else if (mPaused && controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK && i == mPausedBy)
			{
				mPaused = false;
				ScreenManagerC::GetInstance()->returnToMainMenu();
			}

			mPreviousControllerStates[i] = controllerState;
		}
	}
}

/*
	Iterates through the players and sets their being hit flag and damage taken if any of their hitboxes are colliding with anothers that is attacking.
*/
void PlayerManagerC::applyAttacks(PlayerC *player)
{
	for (int i = 0; i < mNumberOfPlayers; i++)
	{
		if (mPlayerArray[i]->isConnected() && collidesWithPlayer(player, mPlayerArray[i]) && mPlayerArray[i]->mLastAction != PlayerAction::Damaged && mPlayerArray[i]->mLastAction != PlayerAction::Dodge)
		{
			mPlayerArray[i]->mBeingHit = true;
			mPlayerArray[i]->mLastDamageTaken = player->mLastDamageDealt;
		}
	}
}

/*
	Renders each player if they are connected and not dead.
*/
void PlayerManagerC::renderPlayers()
{
	for (int i = 0; i < mNumberOfPlayers; i++)
	{
		if (mPlayerArray[i]->isConnected() && !mPlayerArray[i]->mDead)
			mPlayerArray[i]->render();
	}
}

void PlayerManagerC::renderPauseScreen()
{
	mPauseScreenSprite->render(pauseScreenPosition, 0, 0, false);
}

/*
	Returns whether or not two player's hitboxes are overlapping.
*/
bool PlayerManagerC::collidesWithPlayer(PlayerC *attacker, PlayerC *defender)
{
	bool collisionDetected = false;

	if (attacker == defender)
	{
		collisionDetected = false;
	}
	else
	{
		Coord2D attackerWithStartOffset = offsetCoordinate(attacker->getPosition(), attacker->mSpriteHandler->mHitBoxStart);
		Coord2D defenderWithStartOffset = offsetCoordinate(defender->getPosition(), defender->mSpriteHandler->mHitBoxStart);
		Coord2D attackerWithEndOffset = offsetCoordinate(attacker->getPosition(), attacker->mSpriteHandler->mHitBoxEnd);
		Coord2D defenderWithEndOffset = offsetCoordinate(defender->getPosition(), defender->mSpriteHandler->mHitBoxEnd);

		collisionDetected = boxesIntersect(attackerWithStartOffset, attackerWithEndOffset, defenderWithStartOffset, defenderWithEndOffset);
	}

	return collisionDetected;
}

bool PlayerManagerC::boxesIntersect(Coord2D topLeftA, Coord2D bottomRightA, Coord2D topLeftB, Coord2D bottomRightB)
{
	return topLeftA.x < bottomRightB.x && bottomRightA.x > topLeftB.x && bottomRightA.y < topLeftB.y && topLeftA.y > bottomRightB.y;
}

/*
	Returns a coordinate that is the sum of the two given coordinates.
*/
Coord2D PlayerManagerC::offsetCoordinate(Coord2D coordinate, Coord2D offset)
{
	Coord2D withOffset = coordinate;
	withOffset.x += offset.x;
	withOffset.y += offset.y;

	return withOffset;
}