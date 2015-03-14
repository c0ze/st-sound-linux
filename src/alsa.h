#include <alsa/asoundlib.h>
#include <alsa/pcm.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include"YmTypes.h"


#define	BUFFERSIZE 1024

extern 	snd_pcm_t *playback_handle;
extern 	ymsample	convertBuffer[BUFFERSIZE];

bool init_alsa();
int	playback_callback (snd_pcm_sframes_t);
