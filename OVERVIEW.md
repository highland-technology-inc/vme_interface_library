# VME Interface Library Overview
## Executive Summary

The Highland Technology `vme_interface_library` library, in combination with the `v120_driver` and `v120_clt` products, provides for a complete framework for Linux based PCs to access VME boards on one or more VME chassis, via the Highland Technology PCIe to VME bridge [http://www.highlandtechnology.com/DSS/V120DS.shtml](URL) .  

This framework provides for a very easy method to:

 1. map to and programmatically directly access registers on the boards -- without requiring root access or the need for a kernel driver for each board type
 2. user code callable library functions that address most all capabilities of the boards
 3. provide extensive user code examples demonstrating the use of these callable library functions
 4. peruse extensive manpages for interface library function calls for each board type

All three of these products can be easily downloaded from [https://github.com/highland-technology-inc](URL) .

Presently the VME interface library has support for:

 1. V375 Arbitrary Waveform Generator module
 2. V450 Analog Input and Thermocouple Measurement module
 3. V470 Analog Output and Thermocouple Simulator module
 4. V545 Synchro/Resolver/LVDT/RVDT Simulation/Acquisition module

Support for additional boards will be provided upon request.

For more details, see the README.



