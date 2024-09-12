KERN_DIR = /home/ubuntu/Linux-kernel-source/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=$(PWD) modules 
	$(CROSS_COMPILE)gcc -o button_test button_test.c 

clean:
	make -C $(KERN_DIR) M=$(PWD) modules clean
	rm -rf modules.order
	rm -f ledtest

# refers to drivers/char/ipmi/Makefile
# compile a.c, b.c to ab.ko:
# ab-y := a.o b.o
# obj-m += ab.o


obj-m	+= button_drv.o
obj-m	+= board_100ask_imx6ull.o
