/*-----------------------------------------------------------------------------

  ST-Sound ( YM files player library )

  Copyright (C) 1995-1999 Arnaud Carre ( http://leonard.oxg.free.fr )



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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "StSoundLibrary.h"
#include "alsa.h"

YMMUSIC *pMusic;
ymMusicInfo_t info;

bool    songLoopMode = false;       //play the same song continously
bool    listLoopMode = false;       //play the songs in the list continously
bool    playListMode = false;       //I have no idea what this was
int     currentSong;
int     totalSongs;

ymu32 totalNbSample;
ymu32 nbTotal;
int   oldRatio;

struct termios oldSettings, newSettings;

int main(int argc, char* argv[])
{

  //--------------------------------------------------------------------------
  // Checks args.
  //--------------------------------------------------------------------------
  printf(	"stsoundlinux ym tune player.\n"
			"Using ST-Sound Library, under GPL license\n"
			"Copyright (C) 1995-1999 Arnaud Carre ( http://leonard.oxg.free.fr )\n"
			"Linux version by CoZe. (http://blog.coze.org)\n");

  if (argc < 2)
	{
      printf("Usage: stsoundlinux <ym music files>\n");
      //		printf("switches : -r for single track loop, -l for global loop mode.\n");
      printf("In program hot keys you can use while listening :\n");
      printf("Use n for next track in the list, p for previous track, q for quit.\n");
      printf("Use r for toggle repeat mode (single track loop) l for toggle global loop mode.\n");

      return -1;
	}


  /* setup termios for keyboard input */

  tcgetattr( fileno( stdin ), &oldSettings );
  newSettings = oldSettings;
  newSettings.c_lflag &= (~ICANON & ~ECHO);
  tcsetattr( fileno( stdin ), TCSANOW, &newSettings );
  fd_set set;
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = 1;

  //  log(logINFO) << "init alsa ";
  init_alsa ();

  totalSongs = argc-1;
  currentSong = 1;

  snd_pcm_sframes_t frames_to_deliver;
  int err;

  do {
    if (!playSong(argv[currentSong])) {
      exit_all (1);
      return -1;
    }
    do {

      /* wait till the interface is ready for data, or 1 second
         has elapsed.
      */

      if ((err = snd_pcm_wait (playback_handle, 1000)) < 0) {
        fprintf (stderr, "poll failed (%s)\n", strerror (errno));
        break;
      }

      /* find out how much space is available for playback data */

      if ((frames_to_deliver = snd_pcm_avail_update (playback_handle)) < 0) {
        if (frames_to_deliver == -EPIPE) {
          fprintf (stderr, "an xrun occured\n");
          break;
        } else {
          fprintf (stderr, "unknown ALSA avail update return value (%d)\n",
                   frames_to_deliver);
          break;
        }
      }

      frames_to_deliver = frames_to_deliver > BUFFERSIZE ? BUFFERSIZE : frames_to_deliver;

      /* deliver the data */

      totalNbSample += frames_to_deliver;
      const int ratio = (totalNbSample * 100) / nbTotal;
      printf("Rendering... (%d%%)\r",ratio);			oldRatio = ratio;
      if (playback_callback (frames_to_deliver) != frames_to_deliver) {
        fprintf (stderr, "playback callback failed\n");
        break;
      }

      /* keyboard input handling */

      FD_ZERO( &set );
      FD_SET( fileno( stdin ), &set );

      int res = select( fileno( stdin )+1, &set, NULL, NULL, &tv );

      if( res > 0 ) {
        char c;
        read( fileno( stdin ), &c, 1 );
        //         printf( "Input available  %c\n",c );
        if(c == 'n') { //skip to next song
          if (currentSong == totalSongs )
            currentSong = 1;
          else
            currentSong ++;
          if(!playSong(argv[currentSong])) {
            exit_all(1);
            return -1;
          }
        }
        if(c == 'p') { //skip to previous song
          if (currentSong == 1 )
            currentSong = totalSongs;
          else
            currentSong --;
          if(!playSong(argv[currentSong])) {
            exit_all(1);
            return -1;
          }
        }
        if(c == 'r') { //toggle repeat mode
          toggleRepeatMode();
        }
        if(c == 'l') { //toggle loop mode
          toggleLoopMode();
        }
        if(c == 'q') { //quit
          exit_all(1);
          return-1;
        }
      }
      else if( res < 0 ) {
        perror( "select error" );
        break;
      }
      else {
        //                        printf( "Select timeout\n" );
      }

    } while (ymMusicCompute(pMusic,convertBuffer,frames_to_deliver)) ;
	if(listLoopMode)
      if(currentSong == totalSongs)
        currentSong = 0;
	if(!songLoopMode)
      currentSong++;
	if(currentSong == 0) currentSong = 1;
  } while (currentSong < totalSongs+1);

  exit_all(1);

  return 0;
}

void exit_all(int state){
  tcsetattr( fileno( stdin ), TCSANOW, &oldSettings );
  snd_pcm_close (playback_handle);
  ymMusicDestroy(pMusic);
}

void toggleRepeatMode(void){
  songLoopMode = !songLoopMode;
  if(songLoopMode)
    printf("Repeat Mode On      \n");
  else
    printf("Repeat Mode Off      \n");
}

void toggleLoopMode(void){
  listLoopMode = !listLoopMode;
  if(listLoopMode)
    printf("Loop Mode On        \n");
  else
    printf("Loop Mode Off        \n");
}

bool playSong (char* songName){

  if(pMusic != NULL) ymMusicDestroy(pMusic);
  //--------------------------------------------------------------------------
  // Load YM music and creates WAV file
  //--------------------------------------------------------------------------

  pMusic = ymMusicCreate();

  if (ymMusicLoad(pMusic,songName))
	{
      // Get info about the current music.
      ymMusicGetInfo(pMusic,&info);
      printf( "Playing song number %d of %d\n",currentSong, totalSongs );
      printf("Playing file  \"%s\"\n",songName);
      printf("%s\n%s\n(%s)\n",info.pSongName,info.pSongAuthor,info.pSongComment);
      printf("Total music time: %d seconds.\n",info.musicTimeInSec);

      ymMusicSetLoopMode(pMusic,YMFALSE);			// Be sure there is no loop (to avoid a BIG wav file :-) )
      totalNbSample = 0;

      nbTotal = info.musicTimeInSec * 44100;
      oldRatio = -116;

      return true;
    }
  else
	{	// Error in loading music.
      printf("Error in loading file %s:\n%s\n",songName,ymMusicGetLastError(pMusic));
      return false;
	}
}
