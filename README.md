# SneakyMouse

A virtual mouse driver for linux.

## Development environment

A few notes about the development environment:
Use the Vagrantbuildsystem repository to setup a virtual machine that (should)
be equipped to compile kernel modules. The build directory has a symbolic link
at ~/build. 

If you would like to get a compile\_commands.json file for IDE indexing such as for Clion or YouCompleteMe use the (BEAR)[https://github.com/rizsotto/Bear] utlity. This intercepts the build calls when building the linux kernel. Download the appropriate linux kernel for the Vagrantbuildsystem repository and build it 
through bear. 

NB: If you are on Debian you will need to disable the CONFIG\_MODULE\_SIG\_ALL, CONFIG\_MODULE\_SIG\_KEY and CONFIG\_SYSTEM\_TRUSTED\_KEYS settings in the config file. See https://lists.debian.org/debian-kernel/2016/04/msg00579.html.
