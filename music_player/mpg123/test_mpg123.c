#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

//arm-unknown-linux-gnueabihf-gcc -o test_mpg123 test_mpg123.c -lmpg123 -lasound

#define MUSIC_PATH "/usr/share/myir/Music/Ascended Vibrations.mp3"
#define MIXER_NAME "Headphone"  // Change this based on the actual control name for your device

// Function to play the music
void play_music() {
    char command[256];
    snprintf(command, sizeof(command), "mpg123 -a hw:0,0 \"%s\" &", MUSIC_PATH);
    printf("Executing command: %s\n", command);  // Debug print to verify the command
    system(command);  // Play the music using mpg123 in the background
}

// Function to stop the music
void stop_music() {
    system("killall mpg123");  // Kill all mpg123 processes to stop the music
}

// Function to set the volume
void set_volume(long volume_percent) {
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem;
    long min_volume, max_volume;

    // Open the mixer
    if (snd_mixer_open(&handle, 0) < 0) {
        fprintf(stderr, "Error: Unable to open mixer\n");
        return;
    }

    // Attach to the default card
    if (snd_mixer_attach(handle, "default") < 0) {
        fprintf(stderr, "Error: Unable to attach to default card\n");
        snd_mixer_close(handle);
        return;
    }

    // Register the mixer
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0) {
        fprintf(stderr, "Error: Unable to register mixer\n");
        snd_mixer_close(handle);
        return;
    }

    // Load mixer elements
    if (snd_mixer_load(handle) < 0) {
        fprintf(stderr, "Error: Unable to load mixer\n");
        snd_mixer_close(handle);
        return;
    }

    // Allocate memory for mixer simple element ID
    snd_mixer_selem_id_alloca(&sid);

    // Set the mixer element name (e.g., "Master")
    snd_mixer_selem_id_set_name(sid, MIXER_NAME);

    // Find the mixer element
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        fprintf(stderr, "Error: Unable to find mixer element\n");
        snd_mixer_close(handle);
        return;
    }

    // Get the volume range (min and max)
    snd_mixer_selem_get_playback_volume_range(elem, &min_volume, &max_volume);

    // Calculate the actual volume value based on the percentage
    long volume = (volume_percent * max_volume) / 100;

    // Set the volume
    if (snd_mixer_selem_set_playback_volume_all(elem, volume) < 0) {
        fprintf(stderr, "Error: Unable to set volume\n");
    }

    snd_mixer_close(handle);
}

// Function to get the current volume
long get_volume() {
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem;
    long min_volume, max_volume, current_volume;
    snd_mixer_selem_channel_id_t channel = SND_MIXER_SCHN_FRONT_LEFT;

    // Open the mixer
    if (snd_mixer_open(&handle, 0) < 0) {
        fprintf(stderr, "Error: Unable to open mixer\n");
        return -1;
    }

    // Attach to the default card
    if (snd_mixer_attach(handle, "default") < 0) {
        fprintf(stderr, "Error: Unable to attach to default card\n");
        snd_mixer_close(handle);
        return -1;
    }

    // Register the mixer
    if (snd_mixer_selem_register(handle, NULL, NULL) < 0) {
        fprintf(stderr, "Error: Unable to register mixer\n");
        snd_mixer_close(handle);
        return -1;
    }

    // Load mixer elements
    if (snd_mixer_load(handle) < 0) {
        fprintf(stderr, "Error: Unable to load mixer\n");
        snd_mixer_close(handle);
        return -1;
    }

    // Allocate memory for mixer simple element ID
    snd_mixer_selem_id_alloca(&sid);

    // Set the mixer element name (e.g., "Master")
    snd_mixer_selem_id_set_name(sid, MIXER_NAME);

    // Find the mixer element
    elem = snd_mixer_find_selem(handle, sid);
    if (!elem) {
        fprintf(stderr, "Error: Unable to find mixer element\n");
        snd_mixer_close(handle);
        return -1;
    }

    // Get the volume range (min and max)
    snd_mixer_selem_get_playback_volume_range(elem, &min_volume, &max_volume);

    // Get the current volume
    snd_mixer_selem_get_playback_volume(elem, channel, &current_volume);

    snd_mixer_close(handle);

    // Convert current volume to a percentage
    return (current_volume * 100) / max_volume;
}

int main() {
    int input;
    long volume;

    // Play the music in the background
    play_music();

    // Get the initial volume
    volume = get_volume();
    printf("Current volume: %ld%%\n", volume);

    while (1) {
        printf("Enter 1 to increase volume, 2 to decrease volume, 3 to stop music (Ctrl+C to exit): ");
        scanf("%d", &input);

        if (input == 1) {
            volume = get_volume();
            volume += 5;  // Increase volume by 5%
            if (volume > 100) volume = 100;  // Limit to 100%
            set_volume(volume);  // Set volume as a percentage
            printf("Volume increased to: %ld%%\n", volume);
        }
        else if (input == 2) {
            volume = get_volume();
            volume -= 5;  // Decrease volume by 5%
            if (volume < 0) volume = 0;  // Limit to 0%
            set_volume(volume);  // Set volume as a percentage
            printf("Volume decreased to: %ld%%\n", volume);
        }
        else if (input == 3) {
            stop_music();  // Stop the music
            printf("Music stopped.\n");
            break;  // Exit the loop and program after stopping the music
        }
        else {
            printf("Invalid input, please enter 1, 2, or 3.\n");
        }
    }

    return 0;
}