#include"alsa.h"


snd_pcm_hw_params_t *hw_params;
snd_pcm_sw_params_t *sw_params;
snd_pcm_t *playback_handle;
static char *device = "default";
static unsigned int rate = 44100;
ymsample	convertBuffer[BUFFERSIZE];
int nfds;
int err;

struct pollfd *pfds;

bool init_alsa ()
{

  if ((err = snd_pcm_open (&playback_handle, device, SND_PCM_STREAM_PLAYBACK,  0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n",
             device,
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
    fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_set_rate_near (playback_handle, hw_params, &rate, 0)) < 0) {
    fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params_set_channels (playback_handle, hw_params, 1)) < 0) {
    fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  snd_pcm_hw_params_free (hw_params);

  /* tell ALSA to wake us up whenever 4096 or more frames
     of playback data can be delivered. Also, tell
     ALSA that we'll start the device ourselves.
  */

  if ((err = snd_pcm_sw_params_malloc (&sw_params)) < 0) {
    fprintf (stderr, "cannot allocate software parameters structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  if ((err = snd_pcm_sw_params_current (playback_handle, sw_params)) < 0) {
    fprintf (stderr, "cannot initialize software parameters structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  if ((err = snd_pcm_sw_params_set_avail_min (playback_handle, sw_params, 4096)) < 0) {
    fprintf (stderr, "cannot set minimum available count (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  if ((err = snd_pcm_sw_params_set_start_threshold (playback_handle, sw_params, 0U)) < 0) {
    fprintf (stderr, "cannot set start mode (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  if ((err = snd_pcm_sw_params (playback_handle, sw_params)) < 0) {
    fprintf (stderr, "cannot set software parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  /* the interface will interrupt the kernel every 4096 frames, and ALSA
     will wake up this program very soon after that.
  */

  if ((err = snd_pcm_prepare (playback_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit (1);
  }

}


int
playback_callback (snd_pcm_sframes_t nframes)
{
  int err;

  /* ... fill buf with data ... */

  if ((err = snd_pcm_writei (playback_handle, convertBuffer, nframes)) < 0) {
    fprintf (stderr, "write failed (%s)\n", snd_strerror (err));
  }

  return err;
}
