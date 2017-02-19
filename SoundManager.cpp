/*
	SoundManager.cpp		written by Louis Hofer

	This file holds the implementation for functions prototyped in the SoundManager.h singleton class: SoundManagerC.
*/

#define _CRT_SECURE_NO_WARNINGS

#include "SoundManager.h"
#include "Windows.h"

SoundManagerC* SoundManagerC::sInstance = NULL;

/* Public functions */
SoundManagerC* SoundManagerC::CreateInstance()
{
	if (sInstance == NULL)
		sInstance = new SoundManagerC();

	return sInstance;
}

void SoundManagerC::init()
{

}

void SoundManagerC::shutdown()
{

}

/*
	Plays sounds at the given index into the Kirby animation sheet.
	Ends any sound being played by this process before potentially playing a voice sound or sound effect.
*/
void SoundManagerC::playKirbySound(int animationIndex)
{
	PlaySound(NULL, NULL, SND_ASYNC);

	if (strcmp(voiceSounds[animationIndex], ""))
	{
		char path[50];
		strcpy(path, voiceSoundDirectory);
		strcat(path, voiceSounds[animationIndex]);
		strcat(path, fileExtension);

		PlaySound(path, NULL, SND_FILENAME | SND_ASYNC);
	}
	
	if (strcmp(soundEffects[animationIndex], ""))
	{
		char path[50];
		strcpy(path, soundEffectDirectory);
		strcat(path, soundEffects[animationIndex]);
		strcat(path, fileExtension);

		PlaySound(path, NULL, SND_FILENAME | SND_ASYNC);
	}
}

void SoundManagerC::playMenuSound()
{
	PlaySound(menuSound, NULL, SND_FILENAME | SND_ASYNC);
}

void SoundManagerC::playSelectSound()
{
	PlaySound(selectSound, NULL, SND_FILENAME | SND_ASYNC);
}

void SoundManagerC::playLoadingMusic()
{
	PlaySound(loadingMusic, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
}

void SoundManagerC::playCloseMenuSound()
{
	PlaySound(closeMenuSound, NULL, SND_FILENAME | SND_ASYNC);
}

void SoundManagerC::playWinSound()
{
	PlaySound(winSound, NULL, SND_FILENAME | SND_ASYNC);
}