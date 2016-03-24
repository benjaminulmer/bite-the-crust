#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <cstdio>
#include <glm.hpp>
#include <queue>
#include <list>
#include <map>
#include <sigslot.h>

#include "Vehicle.h"

struct Sound3D
{
	FMOD::Channel * channel;
	PhysicsEntity * source;
};
struct VehicleSounds
{
	FMOD::Channel * engineIdleChannel;
	FMOD::Channel * engineRevChannel;
	bool brake;
};

class AudioEngine : public sigslot::has_slots<>
{
public:
	AudioEngine(void);
	~AudioEngine(void);

	void startBackgroundMusic();
	void playReloadSound(Vehicle * source);
	void playDryFireSound(Vehicle * source);
	void playCannonSound(Vehicle * source);
	void playBrakeSound(Vehicle * source);
	void playEngineIdleSound(Vehicle * source);
	void playEngineRevSound(Vehicle * source);
	FMOD::Channel * playSound(FMOD::Sound *, glm::vec3, PhysicsEntity *);
	void update(glm::mat4);

private:
	FMOD::System *fmodSystem;
	FMOD::Sound *backgroundMusic, *cannonSound, *brakeSound, *engineIdleSound, *engineRevSound, *reloadSound, *dryFireSound;
	FMOD::Channel *backgroundChannel, *engineChannel;
	FMOD_RESULT result;
	std::list<Sound3D*> playing;
	std::deque<Sound3D*> availablePointers;

	std::map<PhysicsEntity *, VehicleSounds> vehicleLoops;

	int numChannels;

	void initStreams();
	FMOD_VECTOR glmVec3ToFmodVec(glm::vec3);
	bool stillPlaying(FMOD::Channel *);
	void update3DPositions();
	Sound3D * getOpenChannel();

	inline void	errorCheck();
};