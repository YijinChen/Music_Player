
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

static int fd;

/*
 * ./button_test /dev/100ask_gpio_key
 *
 */
int main(int argc, char **argv)
{
	int val;
	struct pollfd fds[1];
	int timeout_ms = 5000;
	int ret;
	int	flags;
	int gpio_num, gpio_state;

	int i;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}


	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

	flags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	
    while (1)
    {
        /* 3. Read from the file */
        read(fd, &val, sizeof(val));  // Read the full "key" (which includes both GPIO number and state)

        /* 4. Extract GPIO number and state */
        gpio_num = val >> 8;  // Extract the upper byte (GPIO number)
        gpio_state = val & 0xFF;  // Extract the lower byte (GPIO state)

        /* 5. Print the result */
        printf("GPIO %d, State %d\n", gpio_num, gpio_state);
    }

	close(fd);
	
	return 0;
}


