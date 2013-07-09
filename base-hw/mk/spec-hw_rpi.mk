#
# \brief  Build configurations specific to base-hw and Raspberry Pi
# \author Norman Feske
# \date   2013-04-05
#

# denote wich specs are also fullfilled by this spec
SPECS += hw platform_rpi

# set address where to link the text segment at
LD_TEXT_ADDR ?= 0x400000

# include implied specs
include $(call select_from_repositories,mk/spec-hw.mk)
include $(call select_from_repositories,mk/spec-platform_rpi.mk)
