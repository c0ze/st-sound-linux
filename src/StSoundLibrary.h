/*-----------------------------------------------------------------------------

	ST-Sound ( YM files player library )

	Copyright (C) 1995-1999 Arnaud Carre ( http://leonard.oxg.free.fr )

	Main header to use the StSound "C" like API in your production.

-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------

	This file is part of ST-Sound

	ST-Sound is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	ST-Sound is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with ST-Sound; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------------*/


#ifndef __STSOUNDLIBRARY__
#define __STSOUNDLIBRARY__

#include "logging.h"
#include "YmTypes.h"

typedef	void			YMMUSIC;

bool playSong (char*);
void exit_all(int);
void toggleRepeatMode(void);
void toggleLoopMode(void);

typedef struct
{
	ymchar	*	pSongName;
	ymchar	*	pSongAuthor;
	ymchar	*	pSongComment;
	ymchar	*	pSongType;
	ymchar	*	pSongPlayer;
	yms32		musicTimeInSec;		// keep for compatibility
	yms32		musicTimeInMs;
} ymMusicInfo_t;

//---------------------------------------------------------------------
// To produce a WAV file.
//---------------------------------------------------------------------
#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746D66
#define ID_DATA 0x61746164

typedef struct
{
    ymu32		   RIFFMagic;
    ymu32   FileLength;
    ymu32   FileType;
    ymu32   FormMagic;
    ymu32   FormLength;
    ymu16  SampleFormat;
    ymu16  NumChannels;
    ymu32   PlayRate;
    ymu32   BytesPerSec;
    ymu16  Pad;
    ymu16  BitsPerSample;
    ymu32   DataMagic;
    ymu32   DataLength;
} WAVHeader;

#ifdef __cplusplus
extern "C"
{
#endif

// Create object
extern	YMMUSIC *		ymMusicCreate();

// Release object
extern	void			ymMusicDestroy(YMMUSIC *pMusic);

// Global settings
extern	void			ymMusicSetLowpassFiler(YMMUSIC *pMus,ymbool bActive);

// Functions
extern	ymbool			ymMusicLoad(YMMUSIC *pMusic,const char *fName);						// Method 1 : Load file using stdio library (fopen/fread, etc..)
extern	ymbool			ymMusicLoadMemory(YMMUSIC *pMusic,void *pBlock,ymu32 size);			// Method 2 : Load file from a memory block

extern	ymbool			ymMusicCompute(YMMUSIC *pMusic,ymsample *pBuffer,ymint nbSample);	// Render nbSample samples of current YM tune into pBuffer PCM 16bits mono sample buffer.

extern	void			ymMusicSetLoopMode(YMMUSIC *pMusic,ymbool bLoop);
extern	const char	*	ymMusicGetLastError(YMMUSIC *pMusic);
extern	int				ymMusicGetRegister(YMMUSIC *pMusic,ymint reg);
extern	void			ymMusicGetInfo(YMMUSIC *pMusic,ymMusicInfo_t *pInfo);
extern	void			ymMusicPlay(YMMUSIC *pMusic);
extern	void			ymMusicPause(YMMUSIC *pMusic);
extern	void			ymMusicStop(YMMUSIC *pMusic);

extern	void			ymMusicRestart(YMMUSIC *pMusic);

extern	ymbool			ymMusicIsSeekable(YMMUSIC *pMusic);
extern	ymu32			ymMusicGetPos(YMMUSIC *pMusic);
extern	void			ymMusicSeek(YMMUSIC *pMusic,ymu32 timeInMs);

#ifdef __cplusplus
}
#endif


#endif
