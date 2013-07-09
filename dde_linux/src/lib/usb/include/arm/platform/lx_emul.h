/*
 * \brief  Platform specific part of the Linux API emulation
 * \author Sebastian Sumpf
 * \date   2012-06-18
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */
#ifndef _ARM__PLATFORM__LX_EMUL_H_
#define _ARM__PLATFORM__LX_EMUL_H_

/*************************
 ** asm-generic/sizes.h **
 *************************/

enum {
	SZ_1K = 0x00000400,
	SZ_4K = 0x00001000,
};

struct platform_device
{
	char            *name;
	int              id;
	struct device    dev;
	u32              num_resources;
	struct resource *resource;
};


/**********************
 ** linux/usb/ulpi.h **
 **********************/

enum {
	ULPI_FUNC_CTRL_RESET = (1 << 5),
	ULPI_FUNC_CTRL       = (1 << 2),
};

/*
 * Macros for Set and Clear
 * See ULPI 1.1 specification to find the registers with Set and Clear offsets
 */

#define ULPI_SET(a) (a + 1)


/*****************************
 ** linux/platform_device.h **
 *****************************/

#define module_platform_driver(__platform_driver) \
        module_driver(__platform_driver, platform_driver_register, \
                      platform_driver_unregister)

enum { PLATFORM_DEVID_AUTO = -2 };

struct platform_driver {
	int (*probe)(struct platform_device *);
	int (*remove)(struct platform_device *);
	void (*shutdown)(struct platform_device *);
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;
	const struct platform_device_id *id_table;
};

struct resource *platform_get_resource(struct platform_device *, unsigned, unsigned);
struct resource *platform_get_resource_byname(struct platform_device *, unsigned int, const char *);

int platform_get_irq(struct platform_device *, unsigned int);
int platform_get_irq_byname(struct platform_device *, const char *);

int platform_driver_register(struct platform_driver *);
int platform_device_register(struct platform_device *);
void platform_device_unregister(struct platform_device *);

struct platform_device *platform_device_alloc(const char *name, int id);
int platform_device_add_data(struct platform_device *pdev, const void *data,
                             size_t size);
int platform_device_add_resources(struct platform_device *pdev,
                                  const struct resource *res, unsigned int num);

int platform_device_add(struct platform_device *pdev);
int platform_device_del(struct platform_device *pdev);

int platform_device_put(struct platform_device *pdev);


#define to_platform_device(x) container_of((x), struct platform_device, dev)

/**********************
 ** asm/generic/io.h **
 **********************/

static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	return *(const volatile u32 __force *) addr;
}

static inline void __raw_writel(u32 b, volatile void __iomem *addr)
{
	*(volatile u32 __force *) addr = b;
}


static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	return *(const volatile u8 __force *) addr;
}

static inline void __raw_writeb(u8 b, volatile void __iomem *addr)
{
	*(volatile u8 __force *) addr = b;
}


/********************************
 ** linux/regulator/consumer.h **
 ********************************/

struct regulator { };
int    regulator_enable(struct regulator *);
int    regulator_disable(struct regulator *);
void   regulator_put(struct regulator *regulator);
struct regulator *regulator_get(struct device *dev, const char *id);


/*******************************************
 ** arch/arm/plat-omap/include/plat/usb.h **
 *******************************************/

int omap_usbhs_enable(struct device *dev);
void omap_usbhs_disable(struct device *dev);


/*****************
 ** linux/clk.h **
 *****************/

struct clk *clk_get(struct device *, const char *);
int    clk_enable(struct clk *);
void   clk_disable(struct clk *);
void   clk_put(struct clk *);
struct clk *devm_clk_get(struct device *dev, const char *id);
int    clk_prepare_enable(struct clk *);
void   clk_disable_unprepare(struct clk *);


/******************
 ** linux/gpio.h **
 ******************/

enum { GPIOF_OUT_INIT_HIGH = 0x2 };

bool gpio_is_valid(int);
void gpio_set_value_cansleep(unsigned, int);
int gpio_request_one(unsigned, unsigned long, const char *);

/****************
 ** linux/of.h **
 ****************/

#define of_match_ptr(ptr) NULL


/*********************
 ** linux/of_gpio.h **
 *********************/

int of_get_named_gpio(struct device_node *, const char *, int);


/******************
 ** linux/phy.h  **
 ******************/

enum {
	MII_BUS_ID_SIZE = 17,
	PHY_MAX_ADDR    = 32,
	PHY_POLL        = -1,
};


#define PHY_ID_FMT "%s:%02x"

struct mii_bus
{
	const char *name;
	char id[MII_BUS_ID_SIZE];
	int (*read)(struct mii_bus *bus, int phy_id, int regnum);
	int (*write)(struct mii_bus *bus, int phy_id, int regnum, u16 val);
	void *priv;
	int *irq;
};

struct phy_device
{
	int speed;
	int duplex;
	int link;
};

struct mii_bus *mdiobus_alloc(void);
int  mdiobus_register(struct mii_bus *bus);
void mdiobus_unregister(struct mii_bus *bus);
void mdiobus_free(struct mii_bus *bus);

int  phy_mii_ioctl(struct phy_device *phydev, struct ifreq *ifr, int cmd);
void phy_print_status(struct phy_device *phydev);
int  phy_ethtool_sset(struct phy_device *phydev, struct ethtool_cmd *cmd);
int  phy_ethtool_gset(struct phy_device *phydev, struct ethtool_cmd *cmd);
int  phy_start_aneg(struct phy_device *phydev);
void phy_stop(struct phy_device *phydev);
int  genphy_resume(struct phy_device *phydev);

struct phy_device * phy_connect(struct net_device *dev, const char *bus_id,
                                void (*handler)(struct net_device *),
                                phy_interface_t interface);
void phy_disconnect(struct phy_device *phydev);


/*******************************
 ** linux/usb/nop-usb-xceiv.h **
 *******************************/

struct nop_usb_xceiv_platform_data { int type; };


/*******************************
 ** linux/usb/samsung_usb_phy **
 *******************************/

enum samsung_usb_phy_type { USB_PHY_TYPE_HOST = 1 };


#endif /* _ARM__PLATFORM__LX_EMUL_H_ */
