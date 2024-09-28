KERN_DIR = /home/ubuntu/Linux-kernel-source/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=$(PWD) modules 
	$(CROSS_COMPILE)gcc -o ledtest ledtest.c 

clean:
	make -C $(KERN_DIR) M=$(PWD) modules clean
	rm -rf modules.order
	rm -f ledtest

obj-m	+= led_drv.o

