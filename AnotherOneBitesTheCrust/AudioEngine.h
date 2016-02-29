#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <cstdio>
#include <glm.hpp>
#include <queue>
#include <list>

#include "PhysicsEntity.h"
struct Sound3D
{
	FMOD::Channel * channel;
	PhysicsEntity * source;
};

class AudioEngine
{
public:
	AudioEngine(void);
	~AudioEngine(void);

	void startBackgroundMusic();
	void playCannonSound(PhysicsEntity * playing);
	void update(glm::mat4);

private:
	FMOD::System *fmodSystem;
	FMOD::Sound *backgroundMusic, *cannonSound;
	FMOD::Channel *backgroundChannel, *cannonChannel;
	FMOD_RESULT result;
	std::list<Sound3D*> playing;
	std::deque<Sound3D*> availablePointers;

	int numChannels;

	void initStreams();
	FMOD_VECTOR glmVec3ToFmodVec(glm::vec3);
	void update3DPositions();
	Sound3D * getOpenChannel();

	inline void	errorCheck();
};

