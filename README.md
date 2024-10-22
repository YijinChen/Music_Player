MusicPlayer
This project realized a music player on embedded device, which can be controlled by both hardware buttons and remote QT application. The functions of music player include: 1.start playback, 2. stop playback, 3. suspend playback, 4.resume playback, 5.previous music, 6.next music, 7.volume up, 8.volume down, 9.change mode -- sequence/random/circle. 10.connect to server

----------------------------
Operating environment

To run this project, you need a hardware developing board with Linux os (I used a board based on imx6ull), a ubuntu server (I used a Amazon cloud server, a virtial machine with ubuntu also works), and a computer with can run QT (My computer is based on MacOS, but a windows computer is also ok).


Ubuntu Server
1. Configure cross-compile toolchain (by libtool)

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

Note: In command "ct-ng menuconfig", you should choose the corresponding library version as developing board. You can also use buildroot instead of libtool. However, my imx6ull is equipped with a linux os with GLIBC_2.34, I cannot choose version 2.34 of glibc in buildroot. So I used libtool.

2. Change user configuration to add cross-compile toolchain
```bash
vim ~/.bashrc
export ARCH=arm
export CROSS_COMPILE=CROSS_COMPILE_TOOLCHAIN
export PATH=CROSS_COMPILE_PATH:$PATH
source ~/.bashrc
```

Replace CROSS_COMPILE_TOOLCHAIN as the name of your cross-compile toolchain, and replace CROSS_COMPILE_PATH as the directory of cross-compile toolchain on your ubuntu.

Example:
```bash
vim ~/.bashrc 
export ARCH=arm
export CROSS_COMPILE=arm-unknown-linux-gnueabihf-
export PATH=$HOME/x-tools/arm-unknown-linux-gnueabihf/bin:$PATH
source ~/.bashrc
```

Test cross-compile toolchain:
```bash
arm-unknown-linux-gnueabihf-gcc -v
```

3. Compile Linux Kernel

download the Linux Kernel corresponding to the Linux version on your developing board, for me, it is Linux-4.9.88.
```bash
make mrproper
make imx_v7_defconfig
make zImage
make dtbs
make modules
```

Linux Developing Board
The Developing Board should connect to Internet to receive packages from ubuntu.

1. Cross-compile json-c
Download json-c-json-c-0.13.zip from github
```bash
unzip json-c-json-c-0.13.zip
cd json-c-json-c-0.13/
./configure --host=arm-unknown-linux-gnueabihf --prefix=/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/arm-unknown-linux-gnueabihf
make
sudo PATH=$PATH make install
```

2. Cross-compile alsa-lib

Download alsa-lib-xxxx.tar from website on ubuntu, decompressed it and configure it:
```bash
wget xxxx
tar - vxjf alsa-lib-xxx.tar.bz2
cd alsa-lib-xxx.tar.bz2

./configure --host=arm-unknown-linux-gnueabihf --prefix=/usr/lib/alsa-lib
PATH=$PATH:/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/bin make
sudo -E PATH=$PATH:/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/bin make install
```
Copy the installed alsa-lib to imx6ull with scp
```bash
scp -i -r ubuntu@xxx:/usr/lib/alsa-lib /usr/lib/alsa-lib
scp -i -r ubuntu@xxx:/usr/share/alsa /usr/share/alsa
```

3. Cross-compile alsa-utils

```bash
wget xxxx
tar - vxjf alsa-utils-xxx.tar.bz2
cd alsa-utils-xxx/

./configure --host=arm-unknown-linux-gnueabihf --prefix=/usr/lib/alsa-utils —-with-alsa-inc-prefix=/usr/lib/alsa-lib/include --with-alsa-prefix=/usr/lib/alsa-lib/lib
make
make install
```
Copy the installed alsa-lib to imx6ull with scp

```bash
scp -i ubuntu@xxx:/usr/lib/alsa-utils/bin/* /bin
scp -i ubuntu@xxx:/usr/lib/alsa-utils/sbin/* /sbin
scp -i ubuntu@xxx:/usr/lib/alsa-utils/share/alsa /usr/share
```

4. cross-compile mpg123

```bash
./configure --host=arm-unknown-linux-gnueabihf --prefix=/home/ubuntu/x-tools/arm-unknown-linux-gnueabihf/arm-unknown-linux-gnueabihf --with-audio=alsa
```



