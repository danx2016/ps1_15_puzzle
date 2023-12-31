#ifndef _GAME_H
#define _GAME_H

#define GAME_RES_FILENAME_FONT_BIN_BIG "\\ASSETS\\FONT\\FONT_0.BIN;1"
#define GAME_RES_FILENAME_FONT_TIM_BIG "\\ASSETS\\FONT\\FONT_0.TIM;1"

#define GAME_RES_FILENAME_FONT_BIN_SMALL "\\ASSETS\\FONT\\FONT_1.BIN;1"
#define GAME_RES_FILENAME_FONT_TIM_SMALL "\\ASSETS\\FONT\\FONT_1.TIM;1"

#define GAME_RES_FILENAME_IMAGE_TILESET "\\ASSETS\\IMAGES\\FIFTPUZZ.TIM;1"

#define GAME_RES_FILENAME_ALPHA_TITLE "\\ASSETS\\IMAGES\\TITLE.TIM;1"
#define GAME_RES_FILENAME_ALPHA_MASK_TITLE "\\ASSETS\\IMAGES\\TITLEMSK.TIM;1"

#define GAME_RES_FILENAME_MUSIC_XM_TITLE "\\ASSETS\\MUSICS\\BUBBMACH.XM;1"
#define GAME_RES_FILENAME_MUSIC_VB_TITLE "\\ASSETS\\MUSICS\\BUBBMACH.VB;1"
#define GAME_RES_FILENAME_MUSIC_VH_TITLE "\\ASSETS\\MUSICS\\BUBBMACH.VH;1"

#define GAME_RES_FILENAME_MUSIC_XM_PLAYING "\\ASSETS\\MUSICS\\LOSTWITH.XM;1"
#define GAME_RES_FILENAME_MUSIC_VB_PLAYING "\\ASSETS\\MUSICS\\LOSTWITH.VB;1"
#define GAME_RES_FILENAME_MUSIC_VH_PLAYING "\\ASSETS\\MUSICS\\LOSTWITH.VH;1"

#define GAME_RES_FILENAME_MUSIC_VB_SFX "\\ASSETS\\MUSICS\\SFX.VB;1"
#define GAME_RES_FILENAME_MUSIC_VH_SFX "\\ASSETS\\MUSICS\\SFX.VH;1"

#define GAME_FONT_ID_BIG   0
#define GAME_FONT_ID_SMALL 1

#define GAME_TILESET_ID_BACKGROUND 0
#define GAME_TILESET_ID_TILESET    1
#define GAME_TILESET_ID_TITLE      2
#define GAME_TILESET_ID_FONT_BIG   3
#define GAME_TILESET_ID_FONT_SMALL 4

#define GAME_ALPHA_ID_TITLE 0

#define GAME_MUSIC_ID_TITLE 0
#define GAME_MUSIC_ID_PLAYING 1

#define GAME_SFX_ID_START   0
#define GAME_SFX_ID_MOVING  1
#define GAME_SFX_ID_CLEARED 2

extern void game_load_all_assets(void);
extern void game_entry_point(void);

#endif /* _GAME_H */