#include <linux/module.h>

#include <linux/fs.h>
#include <linux/io.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <asm/io.h>

#include "button_drv.h"

//use a structure to denote the whole iomux module
struct iomux {
    ...
};

struct imx6ull_gpio {
    ...
};

// virtual address of register to enable GPIO4
static volatile unsigned int *CCM_CCGR3;

// virtual address of register to enable GPIO5
static volatile unsigned int *CCM_CCGR1;

// virtual address of register to set GPIO5_IO03 (key0) as GPIO
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1;
// virtual address of register to set GPIO4_IO14 (key1) as GPIO
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B;
// virtual address of register to set GPIO4_IO19 (key2) as GPIO
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC;
// virtual address of register to set GPIO4_IO20 (key3) as GPIO
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_HSYNC;
// virtual address of register to set GPIO4_IO21 (key4) as GPIO
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_DATA00;
// virtual address of register to set GPIO4_IO22 (key5) as GPIO
static volatile unsigned int *IOMUXC_SW_MUX_CTL_PAD_CSI_DATA01;

// virtual address of register to set pull-up resistor for GPIO4_IO19 (key2)
static volatile unsigned int *IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC;
// virtual address of register to set pull-up resistor for GPIO4_IO20 (key3)
static volatile unsigned int *IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC;
// virtual address of register to set pull-up resistor for GPIO4_IO21 (key4)
static volatile unsigned int *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA00;
// virtual address of register to set pull-up resistor for GPIO4_IO22 (key5)
static volatile unsigned int *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA01;


static struct iomux *iomux;

static struct imx6ull_gpio *gpio4;
static struct imx6ull_gpio *gpio5;

static void board_imx6ull_button_init (int which) /* initialize the specific button */      
{
    if (!CCM_CCGR1 && !CCM_CCGR3)
    {
        //map the physical address of registers
        CCM_CCGR1 = ioremap(0x20C406C, 4);
        CCM_CCGR3 = ioremap(0x20C4074, 4);

        IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 = ioremap(0x229000C, 4);
		IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B        = ioremap(0x20E01B0, 4);
        IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC         = ioremap(0x20E01DC, 4);
        IOMUXC_SW_MUX_CTL_PAD_CSI_HSYNC         = ioremap(0x20E01E0, 4);
        IOMUXC_SW_MUX_CTL_PAD_CSI_DATA00        = ioremap(0x20E01E4, 4);
        IOMUXC_SW_MUX_CTL_PAD_CSI_DATA01        = ioremap(0x20E01E8, 4);

        IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC         = ioremap(0x20E0468, 4);
        IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC         = ioremap(0x20E046C, 4);
        IOMUXC_SW_PAD_CTL_PAD_CSI_DATA00         = ioremap(0x20E0470, 4);
        IOMUXC_SW_PAD_CTL_PAD_CSI_DATA01         = ioremap(0x20E0474, 4);

        iomux = ioremap(0x20e0000, sizeof(struct iomux));
        gpio4 = ioremap(0x20A8000, sizeof(struct imx6ull_gpio));
        gpio5 = ioremap(0x20AC000, sizeof(struct imx6ull_gpio));
    }
    
    if (which == 0)
    {
        //1. enable GPIO5: CG15, b[31:30] = 0b11
        *CCM_CCGR1 |= (3<<30);
        //2. set GPIO5_IO01 as GPIO: MUX_MODE, b[3:0] = 0b101
        *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER1 = 5;
        //3. set GPIO5_IO01 as input: GPIO5 GDIR, b[1] = 0b0
        gpio5->gdir &= ~(1<<1);
    }
    else{
        //1. enable GPIO4: CG6 b[13:12] = 0b11
        *CCM_CCGR3 |= (3<<12);
        if(which == 1)
        { 
            //2. set GPIO4_IO14 as GPIO: MUX_MODE, b[3:0] = 0b101
            *IOMUXC_SW_MUX_CTL_PAD_NAND_CE1_B = 5;
            //3. set GPIO4_IO14 as input: GPIO4 GDIR, b[14] = 0b0
            gpio4->gdir &= ~(1<<14);
        }
        else if(which == 2) //GPIO4_IO20 (key2)
        {
            //2. set GPIO4_IO19 as GPIO: MUX_MODE, b[3:0] = 0b101
            *IOMUXC_SW_MUX_CTL_PAD_CSI_VSYNC = 5;
            //set pull-up resistor for GPIO4_IO19
            //mask to clear bits 15-13 (make them `00`)
            *IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC &= ~(0x7 << 13);
            //enable pull: set bits 13 to `1`
            *IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC |= (0x1 << 13);
            //set bits 15-14 to `11` (22K Ohm Pull Up)
            *IOMUXC_SW_PAD_CTL_PAD_CSI_VSYNC |= (0x3 << 14);

            //3. set GPIO4_IO19 as input: GPIO4 GDIR, b[19] = 0b0
            gpio4->gdir &= ~(1<<19);
        }
        else if(which == 3)  //GPIO4_IO20 (key3)
        {
            *IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC = 5;
            *IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC &= ~(0x7 << 13);
            *IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC |= (0x1 << 13);
            *IOMUXC_SW_PAD_CTL_PAD_CSI_HSYNC |= (0x3 << 14);
            gpio4->gdir &= ~(1<<20);
        }
        else if(which == 4)
        {
            *IOMUXC_SW_MUX_CTL_PAD_CSI_DATA00 = 5;
            *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA00 &= ~(0x7 << 13);
            *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA00 |= (0x1 << 13);
            *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA00 |= (0x3 << 14);
            gpio4->gdir &= ~(1<<21);
        }
        else if(which == 5)
        {
            *IOMUXC_SW_MUX_CTL_PAD_CSI_DATA01 = 5;
            *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA01 &= ~(0x7 << 13);
            *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA01 |= (0x1 << 13);
            *IOMUXC_SW_PAD_CTL_PAD_CSI_DATA01 |= (0x3 << 14);
            gpio4->gdir &= ~(1<<22);
        }
        else{
            printk("illegal button number\n");
        }
    }
}

static int board_imx6ull_button_read (int which) /* read specific button*/
{
    //printk("%s %s line %d, button %d, 0x%x\n", __FILE__, __FUNCTION__, __LINE__, which, *GPIO1_DATAIN);
    if (which == 0)
        return (gpio5->psr & (1<<1)) ? 1 : 0; //gpio5->psr & (1<<1) ---> gpio5_io01
    else if (which == 1)
        return (gpio4->psr & (1<<14)) ? 1 : 0; //gpio4->psr & (1<<14) ---> gpio5_io14
    else if (which == 2)
        return (gpio4->psr & (1<<19)) ? 1 : 0; 
    else if (which == 3)
        return (gpio4->psr & (1<<20)) ? 1 : 0; 
    else if (which == 4)
        return (gpio4->psr & (1<<21)) ? 1 : 0; 
    else if (which == 5)
        return (gpio4->psr & (1<<22)) ? 1 : 0;
    else
        printk("illegal button number\n");
    return 0;
}
    
static struct button_operations my_buttons_ops = {
    .count = 6,
    .init = board_imx6ull_button_init,
    .read = board_imx6ull_button_read,
};

int board_imx6ull_button_drv_init(void)
{
    register_button_operations(&my_buttons_ops);
    return 0;
}

void board_imx6ull_button_drv_exit(void)
{
    unregister_button_operations();
}

module_init(board_imx6ull_button_drv_init);
module_exit(board_imx6ull_button_drv_exit);

MODULE_LICENSE("GPL");

