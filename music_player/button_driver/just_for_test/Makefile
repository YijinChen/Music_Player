KERN_DIR =  /home/ubuntu/Linux-kernel-source/Linux-4.9.88

all:
	make -C $(KERN_DIR) M=`pwd` modules 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m += test_drv.o

