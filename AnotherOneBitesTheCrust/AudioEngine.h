#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <cstdio>

class AudioEngine
{
public:
	AudioEngine(void);
	~AudioEngine(void);

	void startBackgroundMusic();
	void update();

private:
	FMOD::System *fmodSystem;
	FMOD::Sound *backgroundMusic, *cannonSound;
	FMOD::Channel *backgroundChannel, *cannonChannel;
	FMOD_RESULT result;

	inline void	errorCheck();
};

