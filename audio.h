#ifndef _AUDIO_H
#define _AUDIO_H

#include <stddef.h>
#include <stdint.h>

#define AUDIO_MAX_NUM_MUSICS 8
#define AUDIO_SOUND_CHANNEL 23

extern int32_t audio_music_volume; // 0~128
extern int32_t audio_sound_volume; // 0~0x3fff

extern void audio_init(void);

extern void audio_add_music(size_t music_id, uint8_t *vb, uint8_t *vh, uint8_t *xm);
extern void audio_play_music(size_t music_id);
extern void audio_stop_music();

extern void audio_set_sound(uint8_t *vb, uint8_t *vh);
extern void audio_play_sound(size_t sound_id, int32_t pitch);

extern void audio_set_music_volume(int32_t music_volume);
extern void audio_set_sound_volume(int32_t sound_volume);

#endif /* _AUDIO_H */