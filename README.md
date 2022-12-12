# The fpv.wtf driver installer
This application automatically installs several **DJI FPV System** related drivers on your **Windows PC**. It is required for using certain fpv.wtf published applications such as [butter](https://github.com/fpv-wtf/butter).

The full list is:

 - a [libusb-win32](https://sourceforge.net/projects/libusb-win32/) driver (and the required libusbK.dll) for the bootrom recovery interface
 - The [Google Kedacom Android KDB Interface](https://drivers.softpedia.com/get/MOBILES/Google/Google-Kedacom-KDB-Interface-Driver-11000-for-Windows-10.shtml) drivers for the fastboot 
 - A WinUSB driver for the Goggles RMVT Video Out interface for browser compatibility

### Usage
Simply run **driver_installer.exe** and wait a few minutes until you get a success message.

### Development
A Ubuntu 22 LTS environment was used. You'll need to at least `sudo apt install mingw-w64` for the cross-compiler, as well as various assorted development such as make.
To compile, run:
```
make
```
### Todo

 - Make subsequent runs clean up previously installed drivers
 - Handle plugged in devices like wdi-simple does
 - Enable a clean GH Actions build and release
