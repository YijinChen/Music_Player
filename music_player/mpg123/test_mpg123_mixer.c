#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <poll.h>
#include <inttypes.h>  // For uint64_t

//arm-unknown-linux-gnueabihf-gcc -o test_mpg123_mixer test_mpg123_mixer.c -lmpg123 -lasound

#define MP3_FILE "/usr/share/myir/Music/Ascended Vibrations.mp3"

void set_volume(snd_mixer_t* mixer, const char* selem_name, long percent) {
    snd_mixer_selem_id_t* sid;
    snd_mixer_elem_t* elem;
    long min, max;
    int err;

    snd_mixer_selem_id_malloc(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    elem = snd_mixer_find_selem(mixer, sid);

    if (!elem) {
        fprintf(stderr, "Failed to find mixer element '%s'\n", selem_name);
    } else {
        err = snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        if (err != 0) {
            fprintf(stderr, "Error getting volume range: %s\n", snd_strerror(err));
        } else {
            long volume = min + (max - min) * percent / 100;
            err = snd_mixer_selem_set_playback_volume_all(elem, volume);
            if (err != 0) {
                fprintf(stderr, "Error setting volume: %s\n", snd_strerror(err));
            } else {
                printf("Volume set to %ld%%\n", percent);
            }
        }
    }
    snd_mixer_selem_id_free(sid);
}


int main() {
    mpg123_handle *mh;
    unsigned char *buffer;
    size_t buffer_size;
    size_t done;
    int err, volume_toggle = 0;
    long rate;
    int channels, encoding;

    // Initialize mpg123
    mpg123_init();
    mh = mpg123_new(NULL, &err);
    mpg123_open(mh, MP3_FILE);
    mpg123_format_none(mh);
    mpg123_format(mh, 44100, MPG123_STEREO, MPG123_ENC_SIGNED_16);
    mpg123_getformat(mh, &rate, &channels, &encoding);
    buffer_size = mpg123_outblock(mh);
    buffer = malloc(buffer_size);

    // ALSA setup
    snd_pcm_t *pcm;
    snd_pcm_hw_params_t *params;
    snd_pcm_open(&pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&params);
    snd_pcm_hw_params_any(pcm, params);
    snd_pcm_hw_params_set_access(pcm, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(pcm, params, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_channels(pcm, params, channels);
    snd_pcm_hw_params_set_rate_near(pcm, params, (unsigned int *)&rate, NULL);
    snd_pcm_hw_params(pcm, params);
    snd_pcm_prepare(pcm);

    // Timer setup
    int tfd = timerfd_create(CLOCK_MONOTONIC, 0);
    struct itimerspec itval;
    itval.it_interval.tv_sec = 5;  // Period of 5 seconds
    itval.it_interval.tv_nsec = 0;
    itval.it_value.tv_sec = 5;     // Initial expiration after 5 seconds
    itval.it_value.tv_nsec = 0;
    timerfd_settime(tfd, 0, &itval, NULL);

    // Poll setup
    struct pollfd fds[1];
    fds[0].fd = tfd;
    fds[0].events = POLLIN;

    // Mixer setup
    snd_mixer_t* mixer;
    snd_mixer_open(&mixer, 0);
    snd_mixer_attach(mixer, "default");
    snd_mixer_selem_register(mixer, NULL, NULL);
    snd_mixer_load(mixer);

    printf("Starting playback...\n");

    while (mpg123_read(mh, buffer, buffer_size, &done) == MPG123_OK) {
        snd_pcm_writei(pcm, buffer, done / 4);
        if (poll(fds, 1, 0) > 0) {
            uint64_t expirations;
            read(tfd, &expirations, sizeof(expirations)); // Clear the event
            volume_toggle = !volume_toggle;
            set_volume(mixer, "Headphone", volume_toggle ? 50 : 30);
        }
    }

    // Cleanup
    free(buffer);
    mpg123_close(mh);
    mpg123_delete(mh);
    mpg123_exit();
    snd_pcm_close(pcm);
    snd_mixer_close(mixer);
    close(tfd);

    return 0;
}
