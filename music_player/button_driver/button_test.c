
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./button_test /dev/100ask_button0
 *
 */
int main(int argc, char **argv)
{
	int fd;
	int val;
	int gpio_num, gpio_key;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}

    while (1)
    {
        /* 3. Read from the file */
        read(fd, &val, sizeof(val));  // Read the full "key" (which includes both GPIO number and state)

        /* 4. Extract GPIO number and state */
        gpio_num = val >> 8;  // Extract the upper byte (GPIO number)
        gpio_key = val & 0xFF;  // Extract the lower byte (GPIO g_key)
		//printf("GPIO %d, State %d\n", gpio_num, gpio_state);
		int key = 0;

		if (gpio_key == 1)
		{
			switch (gpio_num)
			{
			case 129:
				key = 1;
				break;
			case 110:
				key = 2;
				break;
			case 115:
				key = 3;
				break;
			case 116:
				key = 4;
				break;
			case 117:
				key = 5;
				break;
			case 118:
				key = 6;
				break;
			default:
				break;
			}
		}
		if (gpio_key == 2)
		{
			switch (gpio_num)
			{
			case 129:
				key = 7;
				break;
			case 110:
				key = 8;
				break;
			case 115:
				key = 9;
				break;
			case 116:
				key = 10;
				break;
			case 117:
				key = 11;
				break;
			case 118:
				key = 12;
				break;
			default:
				break;
			}
		}
		/* Only print the pressed button when it's a valid button press */
        if (key != 0)
        {
            printf("Pressed button: %d\n", key);
        }
    }
	
	close(fd);
	
	return 0;
}


