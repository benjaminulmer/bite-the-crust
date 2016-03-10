#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <cstdio>
#include <glm.hpp>
#include <queue>
#include <list>
#include <map>
#include <sigslot.h>

#include "PhysicsEntity.h"

struct Sound3D
{
	FMOD::Channel * channel;
	PhysicsEntity * source;
};

class AudioEngine : public sigslot::has_slots<>
{
public:
	AudioEngine(void);
	~AudioEngine(void);

	void startBackgroundMusic();
	void playCannonSound(PhysicsEntity * playing);
	void playBrakeSound(PhysicsEntity * playing);
	void playEngineIdleSound(PhysicsEntity*);
	void playEngineRevSound(PhysicsEntity*);
	FMOD::Channel * playSound(FMOD::Sound *, glm::vec3, PhysicsEntity *);
	void update(glm::mat4);

private:
	FMOD::System *fmodSystem;
	FMOD::Sound *backgroundMusic, *cannonSound, *brakeSound, *engineIdleSound, *engineRevSound;
	FMOD::Channel *backgroundChannel, *engineChannel;
	FMOD_RESULT result;
	std::list<Sound3D*> playing;
	std::deque<Sound3D*> availablePointers;

	std::map<PhysicsEntity *, FMOD::Channel *> loopingSounds;

	int numChannels;

	void initStreams();
	FMOD_VECTOR glmVec3ToFmodVec(glm::vec3);
	bool stillPlaying(FMOD::Channel *);
	void update3DPositions();
	Sound3D * getOpenChannel();

	inline void	errorCheck();
};