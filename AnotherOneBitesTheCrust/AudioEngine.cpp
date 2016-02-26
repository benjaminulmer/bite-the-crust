#include "AudioEngine.h"


AudioEngine::AudioEngine(void)
{
	fmodSystem = nullptr;
	backgroundChannel = nullptr;

	result = FMOD::System_Create(&fmodSystem);      // Create the main system object.
	errorCheck();
	

	result = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.
	errorCheck();
	

	startBackgroundMusic();
}

AudioEngine::~AudioEngine(void)
{
	result = backgroundMusic->release();
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

void AudioEngine::startBackgroundMusic()
{
    /*
        Play the sound.
    */
    result = fmodSystem->playSound(backgroundMusic, 0, false, &backgroundChannel);
    errorCheck();
}

void AudioEngine::initStreams()
{
	result = fmodSystem->createStream("res\\Audio\\8BitIntro.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &backgroundMusic);
    errorCheck();

	result = fmodSystem->createStream("res\\Audio\\cannon.wav", FMOD_3D, 0, &cannonSound);
    errorCheck();
}


void AudioEngine::update()
{
	result = fmodSystem->update();
	errorCheck();

	unsigned int ms = 0;
    unsigned int lenms = 0;
    bool         playing = false;
    bool         paused = false;


	// Hardcoded for background music right now
    if (backgroundChannel)
    {
        result = backgroundChannel->isPlaying(&playing);
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
               
        result = backgroundMusic->getLength(&lenms, FMOD_TIMEUNIT_MS);
        if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE))
        {
            errorCheck();
        }
    }	
}