# How-To Guide for using Highland Technology V120 config, user code examples, and device interface libraries to configure and access VME boards.

## Overview

The `vme_interface_library` provides for a consistent and simplified method to configure and provide an easy method to address and access the capabilities of the Highland Technology VME boards.  There are two main components, the V120 configurator (v120_config) and the board level interface libraries.  Additionally, user code examples are provided in this distribution.

In this release, the three instrumentation cards that are supported are: V450, V470, V545.  The interface library for the V375 will be available shortly, as well as most of the other Highland Technology VME cards in the near future.

---

## Documentation

The manpage for `v120_config` is located in man/man1, and the manpages for the V450, V470, and V545 VME interface libraries are located in man/man3.

---

## Download v120 driver and command line tools

Download the v120 driver and command line tools via "git clone":

```
    $ git clone https://github.com/highland-technology-inc/v120_driver v120_driver
    $ git clone https://github.com/highland-technology-inc/v120_clt v120_clt
```
Or access the URLs via a browser and under the green "Code" button, select "download zip".

Follow the directions on the URLs listed above for compiling and installing the driver and command line tools.

---

## Download v120_config, VME interface libraries, and user example code.

```
    $ git clone git@github.com:highland-technology-inc/vme_interface_library vme_interface_library
```

---

## Building v120_config, VME interface libraries, and user example code.

Edit `vme_interface_library/v120_dirs.mk` and modify `V120_DRIVER_DIR` and `V120_CLT_DIR` to point to the V120 driver and command line tools directories.
```
    $ cd vme_interface_library
    $ make clean all
```
This will recursively build `v120_config`, and everything in `lib`, and `user`.

---
## v120_config

The `v120_config` routine reads a configuration file, probes the boards defined in this file, and writes an output file.  The libraries will read the output file to determine how to map to the board specified.  The configuration file that is read will contain specifics about the V120 controller(s), and will list each board by type, board number, VME address, and V120 controller number.

### v120_config example:

Assuming we have one V120 for one VME chassis, and there are three cards in the chassis (V450, V470, and V545).  In this example, we will list two additional V545 cards (card 1 and 2) that are not present.  The syntax for the vmedevice lines is:
```
    vmedevice board_name[v120_controller_number][board_number] -base <vme_address>
```
In this simple example, there is one V120 controller.  The A16 region starts at 0xffff000 and is 64KB in size.  The A24 region starts at 0xff000000 and is 1MB in size, and the A32 region starts at 0xe0000000 and is 2MB in size.  All the boards listed here have addresses that fall inside of the A16 region.

input.conf:
```
    # -*- conf -*-
    repeater v120[0] -a16 0xffff0000 -s16 64k -a24 0xff000000 -s24 1m -a32 0xe0000000 -s32 2M
    vmedevice v545[0][0] -base 0xffff0000
    vmedevice v545[0][1] -base 0xffff1000
    vmedevice v545[0][2] -base 0xffff1800
    vmedevice v470[0][0] -base 0xffff0800
    vmedevice v450[0][0] -base 0xffff0a00
```
Then run v120_config:
```
    v120_config -i input.conf -o /tmp/output.conf
```

The contents of /tmp/output.conf are as follows.  The online option (-o) tags v545[0][1] and v545[0][2] boards as offline (false).
```
    repeater v120[0] -a16 0xffff0000 -s16 0x10000 -S16 MAX -sp16 264 -ep16 267 -a24 0xff000000 -s24 0x100000 -S24 MAX -sp24 200 -ep24 263 -a32 0xe0000000 -s32 0x200000 -S32 MAX -sp32 72 -ep32 199 -o true
    vmedevice v545[0][0] -base 0xffff0000 -a 16 -o true  # VME Synchro/Resolver/LVDT/RVDT Simulation/Acquisition Module
    vmedevice v545[0][1] -base 0xffff1000 -a 16 -o false # VME Synchro/Resolver/LVDT/RVDT Simulation/Acquisition Module
    vmedevice v545[0][2] -base 0xffff1800 -a 16 -o false # VME Synchro/Resolver/LVDT/RVDT Simulation/Acquisition Module
    vmedevice v470[0][0] -base 0xffff0800 -a 16 -o true  # VME Analog Output and Thermocouple Simulator Module
    vmedevice v450[0][0] -base 0xffff0a00 -a 16 -o true  # VME Analog Input and Thermocouple Measurement Module
```
### v120_config auto-scan example:

Alternatively, `v120_config` can auto-scan for all Highland Technology boards in the VME chassis.  All that is needed in the input.conf file, is the definition of the v120 controller.

/tmp/input.conf:
```
    # -*- conf -*-
    repeater v120[0] -a16 0xffff0000 -s16 64k -a24 0xff000000 -s24 1m -a32 0xe0000000 -s32 2M 
```
Then run v120_config, only scanning A16 space:
```
    v120_config -i /tmp/input.conf -o /tmp/output.conf -scan a16
```
The contents of /tmp/output.conf are as follows.  This chassis contains a V120 controller card, a V545 card, a V470 card, and a V450 card.
```
repeater v120[0] -a16 0xffff0000 -s16 0x10000 -S16 MAX -sp16 264 -ep16 267 -a24 0xff000000 -s24 0x100000 -S24 MAX -sp24 200 -ep24 263 -a32 0xe0000000 -s32 0x200000 -S32 MAX -sp32 72 -ep32 199 -o true
vmedevice v545[0][0] -base 0xffff0000 -a 16 -o true	# VME Synchro/Resolver/LVDT/RVDT Simulation/Acquisition Module
vmedevice v470[0][0] -base 0xffff0800 -a 16 -o true	# VME Analog Output and Thermocouple Simulator Module
vmedevice v450[0][0] -base 0xffff0a00 -a 16 -o true	# VME Analog Input and Thermocouple Measurement Module
```

The v120_config(1) manpage explains this all in detail.

## Interface libraries

For the initial release, there are three interface libraries provided (with many more to come soon).  They are:

1. get_vaddr_for_device
2. v450_utils
3. v470_utils
4. v545_utils

Each one of these is designed to provide the user easy access to the board capabilities and registers.

### Map to v545[0][0] example:
```
    #include "v545_regs_sdef.h"
    #include "v545_utils_proto.h"

    V545_REGS *pb;

    // pb will be a virtual address pointer to the base of the v545 board; controller = 0, board number = 0
    if ((pb = (V545_REGS *)get_vaddr_for_device("/tmp/output.conf", "v545[0][0]")) == (V545_REGS *)0)
      {
        ... print error ...
      }
```
### v545 library functions:
These functions are all described in detail in the v545_utils(3) manpage.
```
    // read/write registers
    v545_read_vximfr()
    v545_read_vxitype()
    v545_read_serial()
    v545_read_romid()
    v545_read_romrev()
    v545_read_dash()
    v545_read_state()
    v545_read_uled()
    v545_write_uled()
    v545_read_biss()
    v545_read_calid()
    v545_read_ycal()
    v545_read_dcal()
    v545_read_swin()
    v545_write_swout()
    v545_read_reg()
    v545_get_cal_date()
    v545_read_dds_block()
    v545_read_ccb_block()
    v545_read_xfmr_type()

    // flash operations
    v545_sprintf_checksum_flash()
    v545_load_flash()

    // macro operation
    v545_execute_macro()

    // configuration
    v545_config_synchro_input()
    v545_config_resolver_input()
    v545_config_synchro_output()
    v545_config_resolver_output()
    v545_config_lvdt_rvdt_input()
    v545_config_lvdt_rvdt_output()

    // read/write devices
    v545_read_synchro_resolver()
    v545_write_synchro_resolver()
    v545_read_lvdt_rvdt()
    v545_write_lvdt_rvdt()

    // read count value for position
    v545_read_synchro_resolver_position_counts()
    v545_read_lvdt_rvdt_position_counts()

    // override block operations
    v545_config_override_position_velocity()
    v545_config_override_switch()
    v545_config_override_watchdog()
    v545_config_override_swtrigger()
    v545_set_override_swtrigger()

    // cleanup
    v545_clear_function_block()
    v545_clear_function_blocks()
    v545_clear_active_override()
    v545_clear_override_block()
    v545_clear_override_blocks()
    v545_clear_regfile()

    // debug
    v545_get_debug_info()

    // used internally
    v545_write_function_code()
    v545_config_dds()
    v545_config_ccb()
    v545_is_a_valid_function_block_chnl()
    v545_is_a_valid_override_block_chnl()
    v545_override_get_regfile_index()
    v545_get_fb_type()
```
<p>&nbsp;</p>
