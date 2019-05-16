#include "Framework.h"
#include "soundManager.h"


soundManager*soundManager::instance = NULL;

void soundManager::Create()
{
	if (instance == NULL)
		instance = new soundManager();
}

void soundManager::Delete()
{
	SAFE_DELETE(instance);
}

soundManager::soundManager()
	:_system(NULL), _sound(NULL), _channel(NULL)
{
}


soundManager::~soundManager()
{
}

HRESULT soundManager::init()
{
	//fmod½Ã½ºÅÛ µî·Ï
	System_Create(&_system);

	_system->init(TOTALSOUNDBUFFER, FMOD_INIT_NORMAL, NULL);

	_sound = new Sound*[TOTALSOUNDBUFFER];
	_channel = new Channel*[TOTALSOUNDBUFFER];

	ZeroMemory(_sound, sizeof(_sound));
	ZeroMemory(_channel, sizeof(_channel));

	return S_OK;
}

void soundManager::release()
{
}

void soundManager::update()
{
	_system->update();
}

void soundManager::addSound(string keyName, string soundName, bool bgm, bool loop)
{
	if (loop)
	{
		if (bgm)
		{
			//±ä À½¾ÇÀÏ¶§´Â createStreamÀ¸·Î 
			_system->createStream(soundName.c_str(), FMOD_LOOP_NORMAL, NULL, &_sound[_mTotalSounds.size()]);
		}
		else
		{
			_system->createSound(soundName.c_str(), FMOD_LOOP_NORMAL, NULL, &_sound[_mTotalSounds.size()]);
		}
	}
	else
	{
		if (bgm)
		{
			_system->createStream(soundName.c_str(), FMOD_DEFAULT, NULL, &_sound[_mTotalSounds.size()]);
		}
		else
		{
			_system->createSound(soundName.c_str(), FMOD_DEFAULT, NULL, &_sound[_mTotalSounds.size()]);
		}
	}
	_mTotalSounds.insert(make_pair(keyName, &_sound[_mTotalSounds.size()]));
}

void soundManager::play(string keyName, float volume)
{
	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;

	for (iter; iter != _mTotalSounds.end(); iter++, count++)
	{
		if (keyName == iter->first)
		{
			_system->playSound(FMOD_CHANNEL_FREE, _sound[count], false, &_channel[count]);

			_channel[count]->setVolume(volume);
			break;
		}
	}
}

void soundManager::stop(string keyName)
{
	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;

	for (iter; iter != _mTotalSounds.end();iter++, count++)
	{
		if (keyName == iter->first)
		{
			//³ë·¡¸¦ ¾Æ¾Ö Á¾·áÇÏ´Â °³³ä
			_channel[count]->stop();
			break;
		}
	}
}

void soundManager::pause(string keyName)
{
	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;

	for (iter; iter != _mTotalSounds.end(); iter++)
	{
		if (keyName == iter->first)
		{
			//³ë·¡¸¦ Àá½Ã¸ØÃã
			_channel[count]->setPaused(true);
			break;
		}
	}
}

void soundManager::resume(string keyName)
{
	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;

	for (iter; iter != _mTotalSounds.end(); iter++)
	{
		if (keyName == iter->first)
		{
			//³ë·¡¸¦ Àá½Ã¸ØÃã
			_channel[count]->setPaused(false);
			break;
		}
	}
}

void soundManager::Update()
{
	_system->update();
}

void soundManager::SetVolume(string keyName, float volume)
{
	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;
	float value = 0;
	for (iter; iter != _mTotalSounds.end(); iter++, count++)
	{
		if (keyName == iter->first)
		{
			_channel[count]->getVolume(&value);
			if (value == volume)break;

			_channel[count]->setVolume(volume);
			break;
		}
	}
}


bool soundManager::isPlaySound(string keyName)
{
	bool isPlay;

	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;

	for (iter; iter != _mTotalSounds.end(); iter++)
	{
		if (keyName == iter->first)
		{
			_channel[count]->isPlaying(&isPlay);
			break;
		}
	}

	return isPlay;
}

bool soundManager::isPauseSound(string keyName)
{
	bool isPause;

	arrSoundsIter iter = _mTotalSounds.begin();

	int count = 0;

	for (iter; iter != _mTotalSounds.end(); iter++)
	{
		if (keyName == iter->first)
		{
			_channel[count]->getPaused(&isPause);
			break;
		}
	}

	return isPause;
}
