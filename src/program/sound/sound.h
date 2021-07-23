#ifndef __SOUND_H__
#define __SOUND_H__

#include <soloud_c.h>

typedef enum GaneSfx
{
    SFX_SELECT,
    SFX_SHOOT_HERO,
    SFX_BLIP,
    SFX_COUNT
} GaneSfx;

typedef enum GameSpeech
{
    SPEECH_SELECT_SHIP,
    SPEECH_GAME_OVER,
    SPEECH_COUNT
} GameSpeech;

typedef struct
{
    Soloud *soloud;
    Speech *speechs[SPEECH_COUNT];
    Sfxr *sfx[SFX_COUNT];
} Sound;

Sound soundCreate();
void soundDestroy(Sound this);
void soundPlaySfx(Sound this, GaneSfx id);
void soundPlaySpeech(Sound this, GameSpeech id);

#endif