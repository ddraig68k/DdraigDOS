# DdraigDOS

DdraigDOS is a simple DOS like Operating system for my Y Ddraig computer. Initial development was all done under a basic low lovel monitor program which was great for initial testing of the hardware but becomes limiting when the hardware is working well.

<p align="center">
    <img src="/_assets/ddraigdos_screenshot.png" alt="DdraigDos"></img>
</p>

The main aim for DdraigDOS is to provide a simple way to run software from a disk rather than having to download code over the serial port. 

Currently the OS supports:
* Fat/Fat32 disk support
* ELF executable support
* PS/2 Keyboard or serial port console access
* Basic driver interface for expansion cards
* OS system calls implementation for disk and I/O access
* Newlib support for DOS apps
* Built in BASIC interpreter (EhBASIC)
* Basic file transfer support via serial port

In the current state the OS is very much in development. The driver interface only has a basic text mode support for the video cards, only monochrome is supported so far, but the display can adapt to the TMS9918, V9958, V9990 and VGA cards and display the correct resolutions. Video support was mostly a proof on concept to get support working, could be improved by implementing hardware support available on each card to more efficiently update the display. 

Keyboard and video card can be used with the computer, although all I/O is performed via the serial port as well. Serial connection uses 38400,8,N,1. 

Newlib is used for standard library support for writing applications and while it works well, even small applications can end up in the 50-60K range. Maybe a small app for modern systems but seems very large for what it should be. Longer term goal is to replace Newlib with a more simpler replacement with the goal of building apps, and possibly even the OS be self-hosted on the computer itself.

While the OS is still in an early state and still some occasional issues appear, it's been fairly stable for a while now. Updates may be slow but there's a lot still left to do.

## TODO:
* Drivers
  * Ethernet support
  * Audio support
  * Floppy drive support
  * PS/2 Mouse support
  * Improved serial driver (interrupt driven)
* Support multiple drives/partitions
* Improved video driver text mode support
* Replace newlib for a much lighter runtime
* Monitor funtions such as assember/disassembler 
* Improved console with colour support
* Basic scripting support (Batch files)
* Support for hosted development (Compilers/editors/etc..)
* Extended file transfer support
