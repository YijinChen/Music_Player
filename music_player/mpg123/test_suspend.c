#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <termios.h>

pid_t music_pid;
time_t start_time, suspend_time;
int suspended = 0;
struct termios old_terminal_settings;

void restore_terminal_settings() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_terminal_settings);
}

void handle_exit_signal(int signum) {
    if (music_pid != 0) {
        kill(music_pid, SIGKILL);
        wait(NULL);
    }
    restore_terminal_settings();
    printf("\nMusic player stopped due to signal %d. Exiting...\n", signum);
    exit(0);
}

void start_music(const char *music_path, int skip_frames) {
    music_pid = fork();
    if (music_pid == 0) {
        char skip_arg[20];
        sprintf(skip_arg, "-k %d", skip_frames);
        execl("/usr/bin/mpg123", "mpg123", "-q", skip_arg, music_path, NULL);
        perror("execl failed");
        exit(1);
    }
}

void resume_music(const char *music_path) {
    // Calculate the total time in seconds that the music played before suspension
    int total_skip_seconds = difftime(suspend_time, start_time);

    // Kill the old mpg123 process
    kill(music_pid, SIGKILL);
    wait(NULL);

    // Convert total time into frames (approx. 38.28 frames per second)
    double frame_rate = 38.25;
    int skip_frames = (int)(total_skip_seconds * frame_rate);

    // Start mpg123 again, skipping the calculated number of frames
    start_music(music_path, skip_frames);

    // Do not reset start_time, keep the original time the music first started
    suspended = 0;  // Mark that the music is no longer suspended
}

int main() {
    const char *music_path = "/usr/share/myir/Music/Ascended Vibrations.mp3";
    start_time = time(NULL);  // Record start time when music starts

    // Save current terminal settings and set cleanup on exit
    tcgetattr(STDIN_FILENO, &old_terminal_settings);
    atexit(restore_terminal_settings);
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);

    // Start music playback
    start_music(music_path, 0);  // No skipping at the start

    while (1) {
        int input;
        printf("Enter 1 to suspend, 2 to continue, 3 to quit: ");
        scanf("%d", &input);

        if (input == 1 && !suspended) {
            // Suspend music by sending SIGSTOP and record suspend time
            printf("Suspending music...\n");
            kill(music_pid, SIGSTOP);
            suspend_time = time(NULL);  // Record the time when music was suspended
            suspended = 1;
            printf("Music suspended.\n");
        } else if (input == 2 && suspended) {
            // Resume music by restarting mpg123 from the same position
            printf("Resuming music...\n");
            resume_music(music_path);
            printf("Music continued.\n");
        } else if (input == 3) {
            // Quit and kill the music process
            kill(music_pid, SIGKILL);
            wait(NULL);
            printf("Music player stopped.\n");
            break;
        } else {
            printf("Invalid input. Try again.\n");
        }
    }

    return 0;
}
