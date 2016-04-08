#include "AudioEngine.h"
const float DISTANCE_FACTOR = 5;
const int MAX_CHANNELS = 100;
FMOD_RESULT AudioEngine::result;

AudioEngine::AudioEngine()
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
	result = brakeSound->release();
	errorCheck();
	result = engineIdleSound->release();
	errorCheck();
	result = engineRevSound->release();
	errorCheck();
	result = reloadSound->release();
	errorCheck();
	result = dryFireSound->release();
	errorCheck();
	result = crashSound->release();
	errorCheck();
    result = fmodSystem->close();
    errorCheck();
    result = fmodSystem->release();
    errorCheck();
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
	result = backgroundChannel->setVolume(0.3f);
	errorCheck();
}

FMOD::Channel * AudioEngine::playSound(FMOD::Sound * sound, glm::vec3 pos, PhysicsEntity * source, float volume = 1)
{

	FMOD::Channel * playingOn = nullptr;

	result = fmodSystem->playSound(sound, 0, false, &playingOn);
	errorCheck();
	result = playingOn->setVolumeRamp(false); // For fixing popping noise at low volume.
	errorCheck();
	result = playingOn->set3DAttributes(&glmVec3ToFmodVec(pos), 0);
	errorCheck();
	result = playingOn->setPaused(false);
	errorCheck();
	result = playingOn->setVolume(volume);
	errorCheck();
	result = playingOn->setCallback(channelCallback);
	errorCheck();
	result = playingOn->setUserData((void*)this);
	errorCheck();

	currentlyPlaying[playingOn] = source;
	if(currentlyPlaying.size() > numChannels)
	{
		numChannels = currentlyPlaying.size();
		printf("%d/%d channels used\n", numChannels, MAX_CHANNELS);
	}

	return playingOn;
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

void AudioEngine::playCollisionSound(Vehicle * source)
{
	glm::vec3 pos = source->getPosition();

	playSound(crashSound, pos, source);
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
		FMOD::Channel * brakeChannel = playSound(brakeSound, pos, source, 0.3f);

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

	result = fmodSystem->createSound("res\\Audio\\crash.wav", FMOD_LOOP_OFF | FMOD_3D, 0, &crashSound);
    errorCheck();
}

void AudioEngine::update3DPositions()
{
		bool isPlaying = false;
		for(auto i : currentlyPlaying)
			i.first->set3DAttributes(&glmVec3ToFmodVec(i.second->getPosition()), nullptr);		
	
}

void AudioEngine::setNumListeners(int numPlayers)
{
	fmodSystem->set3DNumListeners(numPlayers);
}

void AudioEngine::update (std::vector<glm::mat4> listenerModelMatrices)
{
	for(int i = 0; i < listenerModelMatrices.size(); i++)
	{
		glm::vec3 forward(glm::normalize(listenerModelMatrices.at(i) * glm::vec4(0,0,-1,0)));
		glm::vec3 pos(listenerModelMatrices.at(i) * glm::vec4(0,0,0,1));
		glm::vec3 up(glm::normalize(listenerModelMatrices.at(i) * glm::vec4(0,1,0,0)));

		FMOD_VECTOR p = {pos.x, pos.y, pos.z};
		FMOD_VECTOR f = {forward.x, forward.y, forward.z};
		FMOD_VECTOR u = {up.x, up.y, up.z};

		fmodSystem->set3DListenerAttributes(i, &p, nullptr, &f, &u);
	}

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

FMOD_RESULT F_CALLBACK AudioEngine::channelCallback(FMOD_CHANNELCONTROL *chanControl, FMOD_CHANNELCONTROL_TYPE controlType, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *commandData1, void *commandData2)
{
	// We only care about when the sound ends
	if(callbackType != FMOD_CHANNELCONTROL_CALLBACK_END)
		return FMOD_OK;
	

	if (controlType == FMOD_CHANNELCONTROL_CHANNEL)
    {
        FMOD::Channel *channel = (FMOD::Channel *)chanControl;
        // Channel specific functions here...

		AudioEngine * engine;
		result = channel->getUserData((void**)(&engine));
		errorCheck();
		
		FMOD::Sound* sound;
		channel->getCurrentSound(&sound);
		if(sound == engine->brakeSound)
		{
			PhysicsEntity* source =  engine->currentlyPlaying[channel];
			engine->vehicleLoops[source].brake = false;
		}

		auto iter = engine->currentlyPlaying.find(channel);
		engine->currentlyPlaying.erase(iter);
    }
    else
    {
        FMOD::ChannelGroup *group = (FMOD::ChannelGroup *)chanControl;
        // ChannelGroup specific functions here... (but we don't use channelgroups)
    }

    // ChannelControl generic functions here...

    return FMOD_OK;
}