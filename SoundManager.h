#pragma once
/*
	SoundManager.h		written by Louis Hofer
	
	This is a singleton class built to manage the playing of sounds from anywhere in the game.
	It exposes public methods for playing specific sounds as well as sounds associated with an index into the Kirby animation sheet.
*/

class SoundManagerC
{
public:
	/* Public functions */
	static SoundManagerC *CreateInstance();
	static SoundManagerC *GetInstance() { return sInstance; };
	~SoundManagerC() {};

	void init();
	void shutdown();
	void playKirbySound(int animationIndex);
	void playMenuSound();
	void playSelectSound();
	void playLoadingMusic();
	void playCloseMenuSound();
	void playWinSound();

private:
	/* Private functions */
	SoundManagerC() {};

	/* Private data members */
	static SoundManagerC *sInstance;

	/* Private constant data */
	const char *loadingMusic = "Sounds/FinalDestination/last00L.dsp.wav";
	const char *menuSound = "Sounds/MenuSounds/main61.dsp.wav";
	const char *selectSound = "Sounds/SoundEffects/snd_se_Kirby_Appear01.wav";
	const char *closeMenuSound = "Sounds/MenuSounds/main60.dsp.wav";
	const char *winSound = "Sounds/MenuSounds/maind9L.dsp.wav";
	const char *voiceSoundDirectory = "Sounds/VoiceSounds/";
	const char *soundEffectDirectory = "Sounds/SoundEffects/";
	const char *fileExtension = ".wav";

	const char *voiceSounds[33] =
	{
		"snd_se_Kirby_Landing02","snd_se_Kirby_Landing02","","","","",
		"","","","","","",
		"","","","","","",
		"","","","snd_vc_kirby_Damage02","snd_vc_kirby_Damage02","",
		"","","","snd_vc_kirby_Appeal03","","",
		"","",""
	};

	const char *soundEffects[33] =
	{
		"snd_se_Kirby_Landing02","snd_se_Kirby_Landing02","","","","snd_se_Kirby_jump01",
		"snd_se_Kirby_jump01","snd_se_Kirby_dash_start","snd_se_Kirby_dash_start","snd_se_Kirby_smash_H01","snd_se_Kirby_smash_H01","snd_se_Kirby_smash_L01",
		"snd_se_Kirby_smash_L01","snd_se_Kirby_AttackDash","snd_se_Kirby_AttackDash","snd_se_Kirby_Attack100","snd_se_Kirby_Attack100","snd_se_Kirby_smash_S01",
		"snd_se_Kirby_smash_S01","snd_se_Kirby_AttackAir_L01","snd_se_Kirby_AttackAir_L01","","","snd_se_Kirby_Escape",
		"snd_se_Kirby_Escape","","","","snd_se_Kirby_Special_S01","snd_se_Kirby_Special_C3_H01",
		"snd_se_Kirby_Special_C3_H01","snd_se_Kirby_swing_l","snd_se_Kirby_swing_l"
	};
};