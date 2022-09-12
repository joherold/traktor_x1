An experimental proof of concept for using the Traktor Kontrol X1 MK1 with Mixxx 2.4 under Ubuntu 22.04
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

The DJ controller Traktor Kontrol X1 MK1 < https://support.native-instruments.com/hc/en-us/articles/115004599389-TRAKTOR-KONTROL-X1-Troubleshooting-Guide > of Native Instruments is recognized as a generic input device under Ubuntu. This renders the controller unsuitable for Mixxx < https://mixxx.org/ > under Ubuntu, as Mixxx can only work with MIDI, HID or USB bulk devices.

Here, we create a virtual HID device via uhid, which will forward data from the X1 to Mixxx and vice versa. This approach is similar to this one < https://github.com/blaxpot/traktor-s4-mk1-midify > except for using HID instead of MIDI.

This code is an experimental proof of concept. It does not include any sanity checks, error handling, and is not coded in a reasonable way. Do not use!



Reading input via the kernel drivers
---

The X1 is recognized as a generic input device under Ubuntu. The caiaq drivers for this are included in the kernel < https://github.com/torvalds/linux/tree/master/sound/usb/caiaq >. Thus, one can read input from /dev/input/eventX. X can be determined by

    sudo cat /proc/bus/input/devices



Reading input and sending output via libusb
---

It is unfortunately not possible to send data via the kernel drivers to the X1 < https://www.native-instruments.com/forum/threads/audio-kontrol-1-on-linux.53898/ >. LEDs of older Native Instruments devices can be controlled via ALSA's amixer command. However, the X1 does not have a soundcard.

The Windows drivers take care of translating the data sent by the X1 to MIDI < https://bugs.launchpad.net/mixxx/+bug/1432442 > < https://djworx.com/kontrol-s8-follow-native-instruments/ >. Therefore, it is possible to send output to the bulk endpoints via libusb < https://github.com/libusb/libusb > or similar. See also this thread < https://github.com/openAVproductions/openAV-Ctlra/issues/94 > discussing this in context of the S4 MK1. However, to access the device via libusb, we have to detach the kernel drivers.

USB sniffing under Windows with Free USB Analyzer < https://freeusbanalyzer.com/ > while using the X1 with the Traktor Pro 3 software and the appropriate drivers < https://www.native-instruments.com/en/support/downloads/drivers-other-files/ > yielded that 32 bytes are send to endpoint 0x1 for LED control -- one for each LED plus a header -- and 24 bytes are read from endpoint 0x84 to get the state of the buttons and knobs. See the spreadsheet in this repository for details. After writing to the X1, one needs to read one byte from endpoint 0x81. Otherwise, the device blocks. Reading from the device does not seem to have this constraint.



Creating a virtual HID device with uhid
---

uhid is included in the kernel < https://github.com/torvalds/linux/blob/master/drivers/hid/uhid.c > and gives the possibility to create virtual HID devices < https://www.kernel.org/doc/Documentation/hid/uhid.txt >, which can then be accessed via /dev/hidrawX. Adapting the given uhid example < https://github.com/torvalds/linux/blob/master/samples/uhid/uhid-example.c >, we can send data to /dev/hidrawX. Listening is possible via

    sudo cat /dev/hidrawX

We used the HID description and the HID mapping < https://github.com/mixxxdj/mixxx/wiki/Native-Instruments-Traktor-Kontrol-Z1 > < https://github.com/xeruf/mixxx-controller-mappings > from the Kontrol Z1 as a basis for our virtual device. Please note that when working with the development version of Mixxx, the 2.4 branch of the Z1 mapping < https://github.com/xeruf/mixxx-controller-mappings/tree/2.4 > has to be used. The description of a connected Z1 can be obtained via

	sudo cat /sys/kernel/debug/hid/[...]/rdesc

To use this for the code of the uhid device, one needs to convert this to some kind of HEX format < https://eleccelerator.com/usbdescreqparser/ >.



Accessing the virtual device in Mixxx
----

It is necessary to add a udev rule for the virtual X1 such that Mixxx can access the device without root privileges. VendorID is 17CC and ProductID is 2305. However, rules specific for the virtual device do not seem to work. Therefore, the rather permissive rule < https://unix.stackexchange.com/questions/85379/dev-hidraw-read-permissions >

    # Virtual X1
    KERNEL=="hidraw*", SUBSYSTEM=="hidraw", TAG+="uaccess"

is added to the Mixxx access rules < https://github.com/mixxxdj/mixxx/blob/main/res/linux/mixxx-usb-uaccess.rules > and saved in /etc/udev/rules.d/. A restart might be necessary afterwards.

Access of Mixxx to hidrawX is only provided since using the hidraw backend of hidapi < https://github.com/mixxxdj/mixxx/pull/4054 >. Thus, a development version of Mixxx < https://github.com/mixxxdj/mixxx/blob/main/CHANGELOG.md > is necessary. This approach was last checked with commit f3a3e04 < https://github.com/mixxxdj/mixxx/commit/f3a3e04fe5a732d0a3e61b3bc2e2c3ec904a8630 >.

Compiling from source is well documented < https://github.com/mixxxdj/mixxx/wiki/Compiling-On-Linux >. As virtual uhid devices are created with interface = -1, we need to remove this sanity check in line 48 of src/controllers/hid/hidenumerator.cpp < https://github.com/mixxxdj/mixxx/blob/main/src/controllers/hid/hidenumerator.cpp > before compiling.

Mappings for the controllers need to be placed into ~/.mixxx/controllers. Mixxx can then open the device after we assigned the HID mapping in the preferences. The HID mapping in this repository showcases a few basic functions but is far from being usable.



Remarks
---

Mixxx actually supports USB bulk devices such as the Hercules DJ Control < https://manual.mixxx.org/2.4/de/hardware/controllers/hercules_djcontrol_mp3_e2.html >. Thus, deactivating the caiaq drivers and using this interface could be a much cleaner way to connect the X1 MK1 to Mixxx.

If you would like to use this code to experiment yourself, compile the code with gcc

    gcc -o virtual_x1 virtual_x1.c -lusb-1.0

Then start the virtual device as root

    sudo ./virtual_x1

and start Mixxx afterwards. Please note that currently some keyboard input is necessary before the virtual device starts to work.



Bugs and missing features
---

* Use asynchronous USB traffic instead of synchronous < https://libusb.sourceforge.io/api-1.0/libusb_io.html >.
* Remove the need to give keyboard input for the uhid script to start.
* Activate LEDs of shift and hotcue button when pressed.
* Rewrite all code in a clean fashion.
