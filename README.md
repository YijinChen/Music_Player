# MusicPlayer

This project implements a music player on an embedded device that can be controlled via hardware buttons and a remote Qt application. The music player supports the following functions:

1. Start playback
2. Stop playback
3. Suspend playback
4. Resume playback
5. Previous track
6. Next track
7. Volume up
8. Volume down
9. Change mode — sequence/random/circle
10. Connect to server

---

## Operating Environment

To run this project, you will need:
- A hardware development board with a Linux OS (tested on an i.MX6ULL board)
- An Ubuntu server (tested on Amazon Cloud, though any Ubuntu virtual machine works)
- A macOS/Windows computer with Qt Creator installed

### 1. Ubuntu Server Setup

#### 1.1 Configure the Cross-Compilation Toolchain (using `libtool`)

```bash
sudo apt install libtool-bin
libtool --version

git clone https://github.com/crosstool-ng/crosstool-ng
cd crosstool-ng
./bootstrap

ct-ng menuconfig
ct-ng clean
ct-ng build
```

**Note:** When using the `ct-ng menuconfig` command, ensure you select the appropriate library versions based on your development board. Alternatively, you can use **Buildroot** instead of `libtool`. However, I encountered issues with Buildroot as my i.MX6ULL board uses **GLIBC_2.34**, and Buildroot did not support this version of glibc, so I used `libtool`.

#### 1.2 Modify User Configuration to Add Cross-Compilation Toolchain

Add the cross-compiler to your environment:

```bash
vim ~/.bashrc
export ARCH=arm
export CROSS_COMPILE=CROSS_COMPILE_TOOLCHAIN
export PATH=CROSS_COMPILE_PATH:$PATH
source ~/.bashrc
```

Replace:
- **`CROSS_COMPILE_TOOLCHAIN`** with the name of your cross-compile toolchain.
- **`CROSS_COMPILE_PATH`** with the directory of the cross-compile toolchain.

**Example:**

```bash
vim ~/.bashrc
export ARCH=arm
export CROSS_COMPILE=arm-unknown-linux-gnueabihf-
export PATH=$HOME/x-tools/arm-unknown-linux-gnueabihf/bin:$PATH
source ~/.bashrc
```

#### 1.3 Test the Cross-Compile Toolchain

Verify that the cross-compilation toolchain is properly configured by running:

```bash
arm-unknown-linux-gnueabihf-gcc -v
```

#### 1.4 Compile the Linux Kernel

Download the Linux Kernel that corresponds to the version running on your development board. For example, if you're using Linux 4.9.88:

```bash
make mrproper
make imx_v7_defconfig
make zImage
make dtbs
make modules
```

### 2. Embedded Linux Development Board (i.MX6ULL)

Your development board must be connected to the Internet to receive packages from the Ubuntu server.

#### 2.1 Cross-Compile `json-c`

1. Download `json-c` from GitHub:

```bash
unzip json-c-json-c-0.13.zip
cd json-c-json-c-0.13/
./configure --host=arm-unknown-linux-gnueabihf --prefix=/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/arm-unknown-linux-gnueabihf
make
sudo PATH=$PATH make install
```

#### 2.2 Cross-Compile `alsa-lib`

1. Download `alsa-lib` from the website, extract it, and configure:

```bash
wget http://www.alsa-project.org/files/pub/lib/alsa-lib-xxx.tar.bz2
tar -vxjf alsa-lib-xxx.tar.bz2
cd alsa-lib-xxx

./configure --host=arm-unknown-linux-gnueabihf --prefix=/usr/lib/alsa-lib
PATH=$PATH:/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/bin make
sudo -E PATH=$PATH:/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/bin make install
```

2. Transfer the compiled library to the i.MX6ULL using `scp`:

```bash
scp -i -r ubuntu@<IP_ADDRESS>:/usr/lib/alsa-lib /usr/lib/alsa-lib
scp -i -r ubuntu@<IP_ADDRESS>:/usr/share/alsa /usr/share/alsa
```

#### 2.3 Cross-Compile `alsa-utils`

1. Download and configure `alsa-utils`:

```bash
wget http://www.alsa-project.org/files/pub/utils/alsa-utils-xxx.tar.bz2
tar -vxjf alsa-utils-xxx.tar.bz2
cd alsa-utils-xxx

./configure --host=arm-unknown-linux-gnueabihf --prefix=/usr/lib/alsa-utils --with-alsa-inc-prefix=/usr/lib/alsa-lib/include --with-alsa-prefix=/usr/lib/alsa-lib/lib
make
make install
```

2. Transfer the compiled files to the i.MX6ULL:

```bash
scp -i ubuntu@<IP_ADDRESS>:/usr/lib/alsa-utils/bin/* /bin
scp -i ubuntu@<IP_ADDRESS>:/usr/lib/alsa-utils/sbin/* /sbin
scp -i ubuntu@<IP_ADDRESS>:/usr/lib/alsa-utils/share/alsa /usr/share
```

#### 2.4 Cross-Compile `mpg123`

1. Download and compile `mpg123`:

```bash
wget https://www.mpg123.de/download/mpg123-x.xx.x.tar.bz2
tar -vxjf mpg123-x.xx.x.tar.bz2
cd mpg123-x.xx.x

./configure --host=arm-unknown-linux-gnueabihf --prefix=/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/arm-unknown-linux-gnueabihf --with-audio=alsa
make
make install
```

### 3. Personal Computer

1. Install **Qt Creator** on your macOS or Windows computer.
2. Set up the remote connection and project settings as needed to control the music player remotely.


---

## Hardware Driver Installation

### 1. Cross-Compile Driver Programs on Ubuntu

Compile the button and LED drivers on your Ubuntu server:

```bash
cd ../Music_Player/music_player/button_driver
make

cd ../Music_Player/music_player/led_driver
make
```

### 2. Transfer the Compiled Driver Files to the i.MX6ULL

Use `scp` to transfer the compiled driver files to your i.MX6ULL development board.

On the **development board**, run the following commands to copy files from the Ubuntu server:

```bash
scp -i ubuntu@<IP_ADDRESS>:../Music_Player/music_player/button_driver/gpio_key_drv.ko /root/music_player
scp -i ubuntu@<IP_ADDRESS>:../Music_Player/music_player/button_driver/button_test /root/music_player
scp -i ubuntu@<IP_ADDRESS>:../Music_Player/music_player/led_driver/led_drv.ko /root/music_player
scp -i ubuntu@<IP_ADDRESS>:../Music_Player/music_player/led_driver/ledtest /root/music_player
```

### 3. Install LED and Button Drivers on the i.MX6ULL

On the development board, install the drivers by running the following commands:

```bash
cd /root/music_player
insmod gpio_key_drv.ko
insmod led_drv.ko
```

### 4. Test Installed Drivers

To verify the LED and button drivers, execute the following commands:

```bash
cd /root/music_player
./ledtest /dev/myled on
./ledtest /dev/myled off
./button_test /dev/100ask_button0
```

---

## Project Execution

### 1. Compile Programs on Ubuntu

- **Compile the `music_server` program on Ubuntu**:
    ```bash
    cd ../Music_Player/music_server
    g++ *.cpp -o main -levent -ljsoncpp -Wall
    ```

- **Cross-compile the `music_player` program for the i.MX6ULL**:
    ```bash
    cd ../Music_Player/music_player
    arm-unknown-linux-gnueabihf-gcc *.c -o main -ljson-c -lpthread -lmpg123 -lasound
    ```

### 2. Transfer the Compiled `music_player` Program to the i.MX6ULL

On the **i.MX6ULL** development board, transfer the compiled `music_player` binary:

```bash
scp -i ubuntu@<IP_ADDRESS>:../Music_Player/music_player/main /root/music_player
```

### 3. Run Programs on Ubuntu, i.MX6ULL, and Qt Creator

- **On Ubuntu** (run the music server):
    ```bash
    cd ../Music_Player/music_server
    ./main
    ```

- **On i.MX6ULL** (run the music player):
    ```bash
    cd /root/music_player
    ./main
    ```

- **On Qt Creator**:
    Open and execute the program located in `/Music_Player/music_app`.

---






