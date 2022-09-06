An experimental approach to use the Traktor Kontrol X1 MK1 with Mixxx 2.4 under Ubuntu 22.04
===

TOC
---
[Introduction](#introduction)  
[Reading input via the kernel drivers](#reading-input-via-the-kernel-drivers)  
[Reading input and sending output via libusb](#reading-input-and-sending-output-via-libusb)  
[Creating a virtual HID device with uhid](#creating-a-virtual-hid-device-with-uhid)  
[Accessing the virtual device in Mixxx](#accessing-the-virtual-device-in-mixxx)  
[Remarks](#remarks)
[Bugs and missing features](#bugs-and-missing-features)


Introduction
---

The DJ controller Traktor Kontrol X1 MK1 < https://support.native-instruments.com/hc/en-us/articles/115004599389-TRAKTOR-KONTROL-X1-Troubleshooting-Guide > of Native Instruments as a generic input device. This renders the controller unsuitable for Mixxx < https://mixxx.org/ > under Ubuntu, as Mixxx can only work with MIDI, HID or USB bulk devices.

We create a virtual HID device via uhid, which will forward input of the X1 to Mixxx. This approach is similar to this one < https://github.com/blaxpot/traktor-s4-mk1-midify > except for using HID instead of MIDI.

This code can be seen as an experimental proof-of-concept. It does not include any sanity checks, error handling, and is not coded in a reasonable way. Do not use!



Reading input via the kernel drivers
---

The X1 is recognized as a generic input device under Ubuntu. The caiaq drivers for this are included in the kernel < https://github.com/torvalds/linux/tree/master/sound/usb/caiaq >. Thus, one can read input from /dev/input/eventX. Which eventX has to be used can be determined by

    sudo cat /proc/bus/input/devices



Reading input and sending output via libusb
---

It is unfortunately not possible to send data via the kernel drivers < https://www.native-instruments.com/forum/threads/audio-kontrol-1-on-linux.53898/ >. LEDs of older Native Instruments devices can be controlled via ALSA's amixer command. However, the X1 does not have a soundcard.

According to this Mixxx issue < https://bugs.launchpad.net/mixxx/+bug/1432442 > and this interview < https://djworx.com/kontrol-s8-follow-native-instruments/ >, the Windows drivers take care of translating the NHL data sent by the X1 to MIDI. Therefore, it is possible to send output to the OUT bulk endpoints via libusb < https://github.com/libusb/libusb > or similar. See also this openAV-Ctrla issue discussing the NHL in context of the S4 MK1 < https://github.com/openAVproductions/openAV-Ctlra/issues/94 >. However, to access the device via libusb, we have to detach the kernel drivers.

USB sniffing under Windows with Free USB Analyzer < https://freeusbanalyzer.com/ > while using the X1 with the Traktor Pro 3 software and the appropriate drivers < https://www.native-instruments.com/en/support/downloads/drivers-other-files/ > yielded that 32 bytes are send to endpoint 0x1 for LED control -- one for each LED plus a header -- and 24 bytes are read from endpoint 0x84 to get the state of the buttons and knobs. See the spreadsheet in this repository for details. After writing to the X1, one needs to read one byte from endpoint 0x81. Otherwise, the device blocks. Reading from the device does not seem to have this constraint.



Creating a virtual HID device with uhid
---

uhid is included in the kernel < https://github.com/torvalds/linux/blob/master/drivers/hid/uhid.c > and gives the possibility to create virtual HID devices < https://www.kernel.org/doc/Documentation/hid/uhid.txt >, which can then be accessed via /dev/hidrawX. Adapting the given uhid example < https://github.com/torvalds/linux/blob/master/samples/uhid/uhid-example.c >, we can send data to /dev/hidrawX. Listening is possible via

    sudo cat /dev/hidrawX

We used the HID description and the HID mapping < https://github.com/mixxxdj/mixxx/wiki/Native-Instruments-Traktor-Kontrol-Z1 > < https://github.com/xeruf/mixxx-controller-mappings > from the Kontrol Z1 as a basis for our virtual device. Please note that when working with the development version of Mixxx, the 2.4 branch of the Z1 mapping  < https://github.com/xeruf/mixxx-controller-mappings/tree/2.4 > has to be used. The description of a connected Z1 can be obtained via

	sudo cat /sys/kernel/debug/hid/[...]/rdesc

To use this for the code of the uhid device, one needs to convert this to some kind of HEX format < https://eleccelerator.com/usbdescreqparser/ >.



Accessing the virtual device in Mixxx
----

It might be necessary to add the X1 to /etc/udev/hwdb.d/70-av-production.hwdb or to /usr/lib/udev/rules.d/69-mixxx-usb-uaccess.rules < https://raw.githubusercontent.com/mixxxdj/mixxx/main/res/linux/mixxx-usb-uaccess.rules >. VendorID is 17CC and ProductID is 2305.

Since access of Mixxx to hidrawX is only provided since the inclusion of hidapi < https://github.com/mixxxdj/mixxx/pull/4054 >, a development version of Mixxx < https://github.com/mixxxdj/mixxx/blob/main/CHANGELOG.md > is necessary. Compiling from source is well documented < https://github.com/mixxxdj/mixxx/wiki/Compiling-On-Linux >. As virtual uhid devices are created with interface = -1, we need to remove this sanity check in line 48 of hidenumerator.cpp < https://github.com/mixxxdj/mixxx/blob/main/src/controllers/hid/hidenumerator.cpp > before compiling.

Root privileges are currently necessary to open the virtual HID device when running Mixxx. The udev rules are probably not set correctly. Mappings for the controllers then need to be placed into res/controllers/ as .mixxx in the home directory is not read. Mixxx can open the device after we assigned the HID mapping in the preferences.



Remarks
---

Mixxx actually supports USB bulk devices such as the Hercules DJ Control < https://manual.mixxx.org/2.4/de/hardware/controllers/hercules_djcontrol_mp3_e2.html >. Thus, deactivating the caiaq drivers and using this interface could be a much cleaner way to connect the X1 MK1 to Mixxx.

If you would like to use this code for experimenting, compile the code with gcc

    gcc -o virtual_x1 virtual_x1.c -lusb-1.0

Then start the virtual device and Mixxx as root and in this order.



Bugs and missing features
---

* Use asynchronous USB traffic instead of synchronous one < https://libusb.sourceforge.io/api-1.0/libusb_io.html >.
* Revise scaling of knobs 6,0 and 6,1. Currently, large values are simply ignored.
* Remove the need to give keyboard input for the UHID script to start.
* Set LEDs to glow slightly in idle mode.
* Callbacks in HID mapping include a lot of unnecessary function(button) {...} calls.
* Rewrite all code in a clean fashion.
