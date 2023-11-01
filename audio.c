#include <libapi.h>
#include <libspu.h>

#include "audio.h"
#include "xmplay/xmplay.h"
#include "mem.h"

int32_t audio_music_volume = 32;
int32_t audio_sound_volume = 0x3fff;

#define SPU_MALLOC_MAX 128
static int8_t spu_malloc_rec [SPU_MALLOC_RECSIZ * (SPU_MALLOC_MAX + 1)];
static uint32_t rcnt2_evt;

static uint8_t *xm_header_addr;
static uint8_t *xm_addr;

static int32_t vab_ids[AUDIO_MAX_NUM_MUSICS];
static uint8_t *xms[AUDIO_MAX_NUM_MUSICS];

static int32_t sfx_vab_id;

// invoked every ~0.015 secs
static int32_t audio_play_next_sample(void)
{
    XM_SetMasterVol(0, audio_music_volume);
    XM_Update();
    return 1;
}

void audio_init(void)
{
	// init spu
	SpuInit();
	SpuSetCommonMasterVolume(0x3fff, 0x3fff);
	SpuInitMalloc (SPU_MALLOC_MAX, spu_malloc_rec);

    // xm player
	XM_OnceOffInit(XM_PAL);
	XM_SetMono();
	
	// allocate buffer for the xm header and set the address
	int32_t xm_header_size = XM_GetFileHeaderSize();
	xm_header_addr = (uint8_t*) mem_alloc(xm_header_size);
	XM_SetFileHeaderAddress(xm_header_addr);
	
	// allocate buffer for the song and set the address
	int32_t song_size = XM_GetSongSize();
	xm_addr = (uint8_t*) mem_alloc(song_size);
	XM_SetSongAddress(xm_addr);

    // setup the root counter 2 interruption handler to play 
    // xm musics at correct timing regardless of ntsc/pal video mode
    EnterCriticalSection();
    rcnt2_evt = OpenEvent(RCntCNT2, EvSpINT, EvMdINTR, audio_play_next_sample);
    EnableEvent(rcnt2_evt);
    SetRCnt(RCntCNT2, 0xffff, RCntMdINTR);
    ExitCriticalSection();
    StartRCnt(RCntCNT2);    
}

void audio_add_music(size_t music_id, uint8_t *vb, uint8_t *vh, uint8_t *xm)
{
    int32_t vab_id = XM_VABInit(vh, vb);
    vab_ids[music_id] = vab_id;
    xms[music_id] = xm;
}

void audio_play_music(size_t music_id)
{
    DisableEvent(rcnt2_evt);
    InitXMData(xms[music_id], 0, XM_UseXMPanning);
    XM_Init(vab_ids[music_id], 0, 0, 0, XM_Loop, -1, XM_Music, 0); 	
    EnableEvent(rcnt2_evt);
}

void audio_stop_music()
{
    XM_PlayStop(0);
}

void audio_set_sound(uint8_t *vb, uint8_t *vh)
{
    sfx_vab_id = XM_VABInit(vh, vb);
}

void audio_play_sound(size_t sound_id, int32_t pitch)
{
    int sample_addr = XM_GetSampleAddress(sfx_vab_id, sound_id);
    XM_PlaySample(sample_addr, AUDIO_SOUND_CHANNEL, audio_sound_volume, audio_sound_volume, pitch);
}

void audio_set_music_volume(int32_t music_volume)
{
    audio_music_volume = music_volume;
}

void audio_set_sound_volume(int32_t sound_volume)
{
    audio_sound_volume = sound_volume;
}
