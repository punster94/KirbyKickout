/*
	ScreenManager.cpp		written by Louis Hofer

	This file contains the implementation for functions prototyped in the ScreenManager.h singleton class: ScreenManagerC.
*/

#include <stdlib.h>
#include <windows.h>
#include <Xinput.h>
#include "openGLFramework.h"
#include "ScreenManager.h"
#include "PlayerManager.h"
#include "Sprite.h"
#include "SOIL.h"
#include "SoundManager.h"

ScreenManagerC* ScreenManagerC::sInstance = NULL;

/* Public functions */
ScreenManagerC* ScreenManagerC::CreateInstance()
{
	if (sInstance == NULL)
		sInstance = new ScreenManagerC();

	return sInstance;
}

/*
	Loads textures and sprites that are used in game screens and menus throughout the game.
*/
void ScreenManagerC::init()
{
	mButtonProgression = 0;
	mCurrentScreenState = ScreenState::StartScreen;
	mWasRendered = false;

	mStartScreenTexture = loadTexture(startScreenPath);
	mControlScreenTexture = loadTexture(controlScreenPath);
	mGameScreenTexture = loadTexture(gameScreenPath);

	mStartScreenButtonTexture1= loadTexture(startScreenButton1Path);
	mStartScreenButtonTexture2 = loadTexture(startScreenButton2Path);
	mStartScreenButtonTexture3 = loadTexture(startScreenButton3Path);
	mLoadingScreenTexture = loadTexture(loadingScreenPath);
	mEndScreenTexture = loadTexture(endScreenPath);

	mWinningPlayerSprite = new SpriteC(winningPlayerPath, 90.0f, 90.0f, 1, 4);
	mDigits = new SpriteC(digitsPath, 40.0f, 40.0f, 1, 11);
}

void ScreenManagerC::update(DWORD milliseconds)
{
	getControllerState();

	if (mControllerState.Gamepad.sThumbLY < deadValue && mControllerState.Gamepad.sThumbLY > -deadValue)
		mInputReceived = false;

	switch (mCurrentScreenState)
	{
	case ScreenState::StartScreen:
		startScreenUpdate();
		break;
	case ScreenState::ControlScreen:
		controlScreenUpdate();
		break;
	case ScreenState::LoadingScreen:
		loadingScreenUpdate();
		break;
	case ScreenState::GameScreen:
		gameScreenUpdate(milliseconds);
		break;
	case ScreenState::EndScreen:
		endScreenUpdate();
		break;
	default:
		break;
	}

	mPreviousControllerState = mControllerState;
}

void ScreenManagerC::shutdown()
{
	PlayerManagerC::GetInstance()->shutdown();
}

void ScreenManagerC::renderScreen()
{
	switch (mCurrentScreenState)
	{
	case ScreenState::StartScreen:
		renderStartScreen();
		break;
	case ScreenState::ControlScreen:
		renderControlScreen();
		break;
	case ScreenState::LoadingScreen:
		renderLoadingScreen();
		break;
	case ScreenState::GameScreen:
		renderGameScreen();
		break;
	case ScreenState::EndScreen:
		renderEndScreen();
		break;
	default:
		break;
	}
}

void ScreenManagerC::returnToMainMenu()
{
	mWasRendered = false;
	mCurrentScreenState = ScreenState::StartScreen;
}

/* Private functions */
/*
	Manages the possible button states on the start screen and changes the state of the game based on what the first player inputs.
*/
void ScreenManagerC::startScreenUpdate()
{
	if (!mInputReceived && mControllerState.Gamepad.sThumbLY > deadValue)
	{
		mButtonProgression = (mButtonProgression - 1 + numButtons) % numButtons;
		mInputReceived = true;

		SoundManagerC::GetInstance()->playMenuSound();
	}

	if (!mInputReceived && mControllerState.Gamepad.sThumbLY < -deadValue)
	{
		mButtonProgression = (mButtonProgression + 1) % numButtons;
		mInputReceived = true;
		SoundManagerC::GetInstance()->playMenuSound();
	}

	if ((mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START && !(mPreviousControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START)) || ((mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && (mButtonProgression == 0)))
	{
		mButtonProgression = 0;
		mCurrentScreenState = ScreenState::LoadingScreen;
		mWasRendered = false;

		SoundManagerC::GetInstance()->playSelectSound();
	}
	else if ((mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && (mButtonProgression == 1))
	{
		mButtonProgression = 0;
		mCurrentScreenState = ScreenState::ControlScreen;
		mWasRendered = false;

		SoundManagerC::GetInstance()->playSelectSound();
	}
	else if ((mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) && (mButtonProgression == 2))
	{
		SoundManagerC::GetInstance()->playCloseMenuSound();
		TerminateApplication(g_window);
	}
}

/*
	Checks to see if the first player pressed the back button and returns to the start screen if so.
*/
void ScreenManagerC::controlScreenUpdate()
{
	if (mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK && !(mPreviousControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK))
	{
		mButtonProgression = 1;
		mCurrentScreenState = ScreenState::StartScreen;
		mWasRendered = false;

		SoundManagerC::GetInstance()->playSelectSound();
	}
}

/*
	Begins loading music and sets the game state to GameScreen.
	On the first frame of the main game state, all player sprite sheets are loaded.
*/
void ScreenManagerC::loadingScreenUpdate()
{
	mCurrentScreenState = ScreenState::GameScreen;
	mWasRendered = false;

	SoundManagerC::GetInstance()->playLoadingMusic();
}

/*
	Updates the components managed by the ScreenManager singleton.
	Checks to see if the game is over.
*/
void ScreenManagerC::gameScreenUpdate(DWORD milliseconds)
{
	PlayerManagerC::GetInstance()->update(milliseconds);

	if (PlayerManagerC::GetInstance()->mGameOver)
	{
		mCurrentScreenState = ScreenState::EndScreen;
		mWasRendered = false;

		SoundManagerC::GetInstance()->playWinSound();
	}
}

/*
	Checks to see if the first player pressed start and returns to the main menu if so.
*/
void ScreenManagerC::endScreenUpdate()
{
	if (mControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START && !(mPreviousControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START))
	{
		mButtonProgression = 0;
		mCurrentScreenState = ScreenState::StartScreen;
		mWasRendered = false;

		SoundManagerC::GetInstance()->playMenuSound();
	}
}

void ScreenManagerC::getControllerState()
{
	XInputGetState(0, &mControllerState);
}

/*
	Renders the starting screen as well as a different button set based on the currently selected button.
*/
void ScreenManagerC::renderStartScreen()
{
	renderComponent(mStartScreenTexture, -512.0f, 384.0f, 512.0f, -384.0f);
	
	switch (mButtonProgression)
	{
	case 0:
		renderComponent(mStartScreenButtonTexture1, -320.0f, -24.0f, 320.0f, -384.0f, 2);
		break;
	case 1:
		renderComponent(mStartScreenButtonTexture2, -320.0f, -24.0f, 320.0f, -384.0f, 2);
		break;
	case 2:
		renderComponent(mStartScreenButtonTexture3, -320.0f, -24.0f, 320.0f, -384.0f, 2);
		break;
	}

	mWasRendered = true;
}

void ScreenManagerC::renderControlScreen()
{
	renderComponent(mControlScreenTexture, -512.0f, 384.0f, 512.0f, -384.0f);
	mWasRendered = true;
}

void ScreenManagerC::renderLoadingScreen()
{
	renderComponent(mLoadingScreenTexture, -512.0f, 384.0f, 512.0f, -384.0f);
	mWasRendered = true;
}

/*
	Renders the main game background screen.
	Entering the main game requires initialization of the PlayerManagerC singleton.
*/
void ScreenManagerC::renderGameScreen()
{
	if (!mWasRendered)
	{
		PlayerManagerC::GetInstance()->init();
		SoundManagerC::GetInstance()->playSelectSound();
	}
	
	renderComponent(mGameScreenTexture, -512.0f, 384.0f, 512.0f, -384.0f);
	mWasRendered = true;

	PlayerManagerC::GetInstance()->render();
}

/*
	Renders the end screen as well as the winning player's number and sprite.
*/
void ScreenManagerC::renderEndScreen()
{
	renderComponent(mEndScreenTexture, -512.0f, 384.0f, 512.0f, -384.0f);

	int winnerId = PlayerManagerC::GetInstance()->mWinner;

	Coord2D spritePosition;
	spritePosition.x = -30.0f;
	spritePosition.y = 50.0f;

	mWinningPlayerSprite->render(spritePosition, winnerId, 0);

	spritePosition.x = -20.0f;
	spritePosition.y = 157.0f;

	mDigits->render(spritePosition, winnerId + 1, 0, false);
	mWasRendered = true;
}

/*
	Renders a component of the screen using the given texture and coordinates.
	Does not render a buffer around the sides of the texture based on a given buffer size.
*/
void ScreenManagerC::renderComponent(GLuint texture, float startX, float startY, float endX, float endY, int bufferPixels)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);

	startX *= horizontalRatio;
	startY *= verticalRatio;
	endX *= horizontalRatio;
	endY *= verticalRatio;

	float horizontalBuffer = bufferPixels / (endX - startX);
	float verticalBuffer = bufferPixels / (startY - endY);

	glColor4ub(0xff, 0xff, 0xff, 0xff);
	glTexCoord2f(horizontalBuffer, verticalBuffer);
	glVertex2f(startX, endY);
	glTexCoord2f(1.0f - horizontalBuffer, verticalBuffer);
	glVertex2f(endX, endY);
	glTexCoord2f(1.0f - horizontalBuffer, 1.0f - verticalBuffer);
	glVertex2f(endX, startY);
	glTexCoord2f(horizontalBuffer, 1.0f - verticalBuffer);
	glVertex2f(startX, startY);

	glEnd();
}

/*
	Loads the texture at the given path, returning an index that can be used by OpenGL to render.
*/
GLuint ScreenManagerC::loadTexture(char *path)
{
	return SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
}