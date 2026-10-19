// PORT: stand-in for the Apogee Sound System (AUDIO_WF.LIB: FX_* sound
// effects, MUSIC_* MIDI music). Everything succeeds and plays nothing.
// Voice handles are 0 ("no voice"), which the game treats as a sound that
// could not be started. Real audio output is a later milestone.
#include "compat.h"
#include "fx_man.h"
#include "music.h"

int MUSIC_ErrorCode = MUSIC_Ok;

char *FX_ErrorString(int ErrorNumber) { (void)ErrorNumber; return "Sound is not available in this build."; }
int  FX_SetupSoundBlaster(fx_blaster_config blaster, int *MaxVoices, int *MaxSampleBits, int *MaxChannels)
	{ *MaxVoices = 8; *MaxSampleBits = 16; *MaxChannels = 2; return FX_Ok; }
int  FX_Init(int SoundCard, int numvoices, int numchannels, int samplebits, unsigned mixrate) { return FX_Ok; }
int  FX_Shutdown(void) { return FX_Ok; }
int  FX_SetCallBack(void (*function)(unsigned long)) { return FX_Ok; }
void FX_SetVolume(int volume) { }
static int reversestereo = 0;
void FX_SetReverseStereo(int setting) { reversestereo = setting; }
int  FX_GetReverseStereo(void) { return reversestereo; }
void FX_SetReverb(int reverb) { }
void FX_SetReverbDelay(int delay) { }
int  FX_VoiceAvailable(int priority) { return 1; }
int  FX_PlayLoopedVOC(char *ptr, long loopstart, long loopend, int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval) { return 0; }
int  FX_PlayLoopedWAV(char *ptr, long loopstart, long loopend, int pitchoffset, int vol, int left, int right, int priority, unsigned long callbackval) { return 0; }
int  FX_PlayVOC3D(char *ptr, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval) { return 0; }
int  FX_PlayWAV3D(char *ptr, int pitchoffset, int angle, int distance, int priority, unsigned long callbackval) { return 0; }
int  FX_Pan3D(int handle, int angle, int distance) { return FX_Ok; }
int  FX_StopSound(int handle) { return FX_Ok; }
int  FX_StopAllSounds(void) { return FX_Ok; }

char *MUSIC_ErrorString(int ErrorNumber) { (void)ErrorNumber; return "Music is not available in this build."; }
int  MUSIC_Init(int SoundCard, int Address) { return MUSIC_Ok; }
int  MUSIC_Shutdown(void) { return MUSIC_Ok; }
void MUSIC_SetVolume(int volume) { }
void MUSIC_Continue(void) { }
void MUSIC_Pause(void) { }
int  MUSIC_StopSong(void) { return MUSIC_Ok; }
int  MUSIC_PlaySong(unsigned char *song, int loopflag) { return MUSIC_Ok; }
void MUSIC_RegisterTimbreBank(unsigned char *timbres) { }
