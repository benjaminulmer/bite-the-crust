#include "AudioEngine.h"
const float DISTANCE_FACTOR = 5;
const int MAX_CHANNELS = 100;

AudioEngine::AudioEngine(void)
{
	fmodSystem = nullptr;
	backgroundChannel = nullptr;
	numChannels = 0;

	std::random_device rd;
	generator.seed(rd());

	result = FMOD::System_Create(&fmodSystem);      // Create the main system object.
	errorCheck();
	

	result = fmodSystem->init(MAX_CHANNELS, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	errorCheck();
	
	result = fmodSystem->set3DSettings(1.0, DISTANCE_FACTOR, 1.0);


	initStreams();
	startBackgroundMusic();
}

AudioEngine::~AudioEngine(void)
{
	// May want to have sounds read in through JSON, just hold a list (and have some DEFINES for indices)
	result = backgroundMusic->release();
    errorCheck();
	result = cannonSound->release();
	errorCheck();
    result = fmodSystem->close();
    errorCheck();
    result = fmodSystem->release();
    errorCheck();

	// Delete Sound3D pointers
	for(Sound3D * s : playing)
		delete(s);
	for(Sound3D * s : availablePointers)
		delete(s);
}

void AudioEngine::errorCheck()
{	
	if (result != FMOD_OK)
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));	
}

FMOD_VECTOR AudioEngine::glmVec3ToFmodVec(glm::vec3 vector)
{
	FMOD_VECTOR p = {vector.x, vector.y, vector.z};
	return p;
}

void AudioEngine::startBackgroundMusic()
{
	std::uniform_int_distribution<int> dist(0, backgroundSongs.size()-1);
	backgroundSongChoice = dist(generator);
    result = fmodSystem->playSound(backgroundSongs[backgroundSongChoice], 0, false, &backgroundChannel);
    errorCheck();
	result = backgroundChannel->setVolume(0.3);
	errorCheck();
}

Sound3D * AudioEngine::getOpenChannel()
{
	Sound3D * playingOn;

	if(availablePointers.empty())
	{
		if(numChannels < MAX_CHANNELS)
		{
			playingOn = new Sound3D();
			playingOn->channel = nullptr;
			numChannels++;
			printf("Number of channels: %d\n", numChannels);
		}
		else
		{
			printf("CANNOT PLAY SOUND: The maximum of %d channels are currently in use! Consider upping the limit!\n", MAX_CHANNELS);
			return nullptr;
		}
	}
	else
	{
		playingOn = availablePointers.front();
		availablePointers.pop_front();
	}

	return playingOn;
}

FMOD::Channel * AudioEngine::playSound(FMOD::Sound * sound, glm::vec3 pos, PhysicsEntity * source)
{
	Sound3D * playingOn = getOpenChannel();
	
	if(!playingOn)
		return nullptr;

	playingOn->source = source;

	result = fmodSystem->playSound(sound, 0, false, &playingOn->channel);
	playingOn->channel->setVolumeRamp(false); // For fixing popping noise at low volume.
	playingOn->channel->set3DAttributes(&glmVec3ToFmodVec(pos), 0);
	playingOn->channel->setPaused(false);

	playing.push_back(playingOn);
	errorCheck();

	return playingOn->channel;
}

void AudioEngine::playReloadSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();

	playSound(reloadSound, pos, source);
}

void AudioEngine::playDryFireSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();

	playSound(dryFireSound, pos, source);
}

void AudioEngine::playCannonSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();

	playSound(cannonSound, pos, source);
}

void AudioEngine::playBrakeSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();

	VehicleSounds playing = vehicleLoops[source];


	if(playing.brake)
		return;
	else
	{
		if(playing.engineRevChannel != nullptr)
			playing.engineRevChannel->setPaused(true);
		if(playing.engineIdleChannel != nullptr)
			playing.engineIdleChannel->setPaused(false);
		
		playing.brake = true;
		FMOD::Channel * brakeChannel = playSound(brakeSound, pos, source);
		brakeChannel->setVolume(0.3f);

		vehicleLoops[source] = playing;
	}
}


/*
* TODO: Work on state machine for car noises
*/
void AudioEngine::playEngineIdleSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();
	VehicleSounds playing = vehicleLoops[source];

	if(playing.engineRevChannel != nullptr)
		playing.engineRevChannel->setPaused(true);
	if(playing.engineIdleChannel != nullptr)
		playing.engineIdleChannel->setPaused(false);
	else
		playing.engineIdleChannel = playSound(engineIdleSound, pos, source);

	vehicleLoops[source] = playing;

}

void AudioEngine::playEngineRevSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();

	VehicleSounds playing = vehicleLoops[source];

	if(playing.engineRevChannel != nullptr)
		playing.engineRevChannel->setPaused(false);
	else
	{
		playing.engineRevChannel = playSound(engineRevSound, pos, source);
		playing.engineRevChannel->setVolume(0.3f);
	}
	//if(playing.engineIdleChannel != nullptr)
		//playing.engineIdleChannel->setPaused(true);
	

	vehicleLoops[source] = playing;
}

void AudioEngine::initStreams()
{
	// This can probably be done in a less hard-coded fashion, but it's not too bad...
	FMOD::Sound *background;
	result = fmodSystem->createSound("res\\Audio\\Cyborg Ninja.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &background);
    errorCheck();
	backgroundSongs.push_back(background);
	result = fmodSystem->createSound("res\\Audio\\8BitIntro.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &background);
    errorCheck();
	backgroundSongs.push_back(background);
	result = fmodSystem->createSound("res\\Audio\\Ouroboros.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &background);
    errorCheck();
	backgroundSongs.push_back(background);
	result = fmodSystem->createSound("res\\Audio\\Wagon Wheel - Electronic.mp3", FMOD_LOOP_NORMAL | FMOD_2D, 0, &background);
    errorCheck();
	backgroundSongs.push_back(background);

	result = fmodSystem->createSound("res\\Audio\\cannon.wav", FMOD_3D, 0, &cannonSound);
    errorCheck();

	result = fmodSystem->createSound("res\\Audio\\reload.wav", FMOD_3D, 0, &reloadSound);
    errorCheck();

	result = fmodSystem->createSound("res\\Audio\\dryFire.wav", FMOD_3D, 0, &dryFireSound);
    errorCheck();

	result = fmodSystem->createSound("res\\Audio\\engineIdle.wav", FMOD_LOOP_NORMAL | FMOD_3D, 0, &engineIdleSound);
    errorCheck();

	result = fmodSystem->createSound("res\\Audio\\engineRev.wav", FMOD_LOOP_NORMAL | FMOD_3D, 0, &engineRevSound);
    errorCheck();

	result = fmodSystem->createSound("res\\Audio\\brake.wav", FMOD_LOOP_OFF | FMOD_3D, 0, &brakeSound);
    errorCheck();
}

bool AudioEngine::stillPlaying(FMOD::Channel * playingOn)
{
	FMOD::Sound * sound;


	unsigned int soundPos;
	unsigned int length;
	result = playingOn->getPosition(&soundPos, FMOD_TIMEUNIT_MS);
	errorCheck();
	result = playingOn->getCurrentSound(&sound);
	errorCheck();

	// TODO: Loops are not working as expected
	if(sound == engineIdleSound || sound == engineRevSound)
		return true;
	
	result = sound->getLength(&length, FMOD_TIMEUNIT_MS);
	errorCheck();
	bool paused;
	result = playingOn->getPaused(&paused);

	return (soundPos < length) && !paused;
}

void AudioEngine::update3DPositions()
{
		bool isPlaying = false;
		for(std::list<Sound3D*>::iterator i = playing.begin(); i != playing.end(); i++)
	{
			Sound3D * s = *i;
		
			if(!stillPlaying(s->channel))
			{
				FMOD::Sound * sound;
				result = s->channel->getCurrentSound(&sound);
				errorCheck();

				if(sound == brakeSound)
					vehicleLoops[s->source].brake = false;

				s->channel = nullptr;
				s->source = nullptr;
				availablePointers.push_back(s);
				playing.erase(i++);

			}
			else
			{
				s->channel->set3DAttributes(&glmVec3ToFmodVec(s->source->getPosition()), nullptr);
			}
	}
}

void AudioEngine::update(glm::mat4 listenerModelMatrix)
{
	glm::vec3 forward(glm::normalize(listenerModelMatrix * glm::vec4(0,0,-1,0)));
	glm::vec3 pos(listenerModelMatrix * glm::vec4(0,0,0,1));
	glm::vec3 up(glm::normalize(listenerModelMatrix * glm::vec4(0,1,0,0)));

	FMOD_VECTOR p = {pos.x, pos.y, pos.z};
	FMOD_VECTOR f = {forward.x, forward.y, forward.z};
	FMOD_VECTOR u = {up.x, up.y, up.z};

	fmodSystem->set3DListenerAttributes(0, &p, nullptr, &f, &u);

	update3DPositions();

	result = fmodSystem->update();
	errorCheck();

	unsigned int ms = 0;
    unsigned int lenms = 0;
	bool		 isPlaying = false;
    bool         paused = false;


	// This section is for errorchecking / data only and is not actually
	// necessary for playing the music. Should probably make a list of
	// sounds that are playing so we can check all of them
    if (backgroundChannel)
    {
        result = backgroundChannel->isPlaying(&isPlaying);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
        {
            errorCheck();
        }

        result = backgroundChannel->getPaused(&paused);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
        {
            errorCheck();
        }

        result = backgroundChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
        {
            errorCheck();
        }
               
        result = backgroundSongs[backgroundSongChoice]->getLength(&lenms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
        {
            errorCheck();
        }
    }

}