/*
Virtual HID device to use Traktor X1 with Mixxx under Ubuntu.

Based on the standard keyboard2mouse example of uhid. The libusb part has been taken from the bitmaptest code of GoaSkin < https://www.native-instruments.com/forum/threads/programming-the-guide-lights.320806/page-3 >.

NOTE: This an experimental proof-of-concept which does not include any sanity checks, error handling, or is coded in a reasonable way. Do not use!
*/

// Includes from uhid example.
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <linux/uhid.h>

// Includes for reading /dev/input/eventX
#include <linux/input.h>

// Includes for reading via libusb.
#include <libusb-1.0/libusb.h>

 // HID description for the Z1. Does this need to be adapted?
 static unsigned char rdesc[] = {
     0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
     0x09, 0x00,        // Usage (0x00)
     0xA1, 0x01,        // Collection (Application)
     0x09, 0x01,        //   Usage (0x01)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0x01,        //     Report ID (1)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x0F,  //     Logical Maximum (4095)
     0x75, 0x10,        //     Report Size (16)
     0x95, 0x0B,        //     Report Count (11)
     0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
     0x09, 0x05,        //     Usage (0x05)
     0x09, 0x05,        //     Usage (0x05)
     0x09, 0x05,        //     Usage (0x05)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x0F,  //     Logical Maximum (4095)
     0x75, 0x10,        //     Report Size (16)
     0x95, 0x03,        //     Report Count (3)
     0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x09, 0x02,        //     Usage (0x02)
     0x15, 0x00,        //     Logical Minimum (0)
     0x25, 0x01,        //     Logical Maximum (1)
     0x75, 0x01,        //     Report Size (1)
     0x95, 0x08,        //     Report Count (8)
     0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
     0xC0,              //   End Collection
     0x09, 0x80,        //   Usage (0x80)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0x80,        //     Report ID (-128)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x09, 0x81,        //     Usage (0x81)
     0x15, 0x00,        //     Logical Minimum (0)
     0x25, 0x7F,        //     Logical Maximum (127)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x15,        //     Report Count (21)
     0x91, 0x02,        //     Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0x09, 0xD0,        //   Usage (0xD0)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0xD0,        //     Report ID (-48)
     0x09, 0xD1,        //     Usage (0xD1)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x00,  //     Logical Maximum (255)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x20,        //     Report Count (32)
     0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0x09, 0xD0,        //   Usage (0xD0)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0xD1,        //     Report ID (-47)
     0x09, 0xD1,        //     Usage (0xD1)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x00,  //     Logical Maximum (255)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x20,        //     Report Count (32)
     0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0x09, 0xD0,        //   Usage (0xD0)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0xD2,        //     Report ID (-46)
     0x09, 0xD1,        //     Usage (0xD1)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x00,  //     Logical Maximum (255)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x20,        //     Report Count (32)
     0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0x09, 0xD0,        //   Usage (0xD0)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0xD8,        //     Report ID (-40)
     0x09, 0xD1,        //     Usage (0xD1)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x00,  //     Logical Maximum (255)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x20,        //     Report Count (32)
     0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0x09, 0xD0,        //   Usage (0xD0)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0xD9,        //     Report ID (-39)
     0x09, 0xD1,        //     Usage (0xD1)
     0x15, 0x00,        //     Logical Minimum (0)
     0x26, 0xFF, 0x00,  //     Logical Maximum (255)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x20,        //     Report Count (32)
     0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0x09, 0x01,        //   Usage (0x01)
     0xA1, 0x02,        //   Collection (Logical)
     0x85, 0xF0,        //     Report ID (-16)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x04,        //     Usage (0x04)
     0x09, 0x05,        //     Usage (0x05)
     0x09, 0x05,        //     Usage (0x05)
     0x09, 0x05,        //     Usage (0x05)
     0x15, 0x01,        //     Logical Minimum (1)
     0x25, 0x0A,        //     Logical Maximum (10)
     0x75, 0x08,        //     Report Size (8)
     0x95, 0x0E,        //     Report Count (14)
     0xB1, 0x02,        //     Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
     0xC0,              //   End Collection
     0xC0,              // End Collection
 };

// Store values of buttons and knobs.
static bool btn_down[15][4];
static char knob[6][2][2];

// Global variables for libusb out of laziness.
libusb_context* ctx = NULL;
libusb_device_handle* dev_handle = NULL;

static char idx[5]; // Configuration, interface, endpoints.
static int length_in = 24;
static int length_out = 32;
static unsigned char data_out[32];
static unsigned char data_in[24];

 // ----------------------------------------------------------------------------

 int traktor_usb_init(char* idx){

 	libusb_device **devs;
 	int r;
 	ssize_t cnt;
 	r = libusb_init(&ctx);
 	if (r < 0) {
 		printf("Init error %i \n", r);
 	}
 	libusb_set_option(ctx, 3);

 	cnt = libusb_get_device_list(ctx, &devs);
 	if (cnt < 0) {
 		printf("Get device error \n");
 	}
 	printf("%ld devices in list. \n", cnt);

 	dev_handle = libusb_open_device_with_vid_pid(ctx, 0x17cc, 0x2305);
 	if (dev_handle == NULL) {
 		printf("Cannot open device. \n");
 	}
     else {
         printf("Device opened. \n");
     }

 	libusb_free_device_list(devs, 1);

 	if (libusb_kernel_driver_active(dev_handle, idx[1]) == 1) {
 		printf("Kernel driver active. \n");

         if (libusb_detach_kernel_driver(dev_handle, idx[1]) == 0) {
             printf("Kernel driver detached! \n");
         }
 	}
 	r = libusb_claim_interface(dev_handle, idx[1]);
 	if (r < 0) {
 		printf("Cannot claim interface \n");
 	}
     else {
         printf("Claimed interface \n");
     }

     return 0;
 }

 int traktor_usb_bulk_write(char* idx, unsigned char* data_out, int length_out)
 {

    int r = 0;
    int actual;
    r = libusb_bulk_transfer(dev_handle, idx[2], data_out, length_out, &actual, 50);

    unsigned char tmp1[1];
 	r = libusb_bulk_transfer(dev_handle, idx[3], tmp1, 1, &actual, 50);

 }

 int traktor_usb_bulk_read(char* idx, unsigned char* data_in, int length_in)
 {

    int r = 0;
    int actual;
 	r = libusb_bulk_transfer(dev_handle, idx[4], data_in, length_in, &actual, 50);

 }

 int traktor_usb_close(char* idx)
 {

     int r;
 	r = libusb_release_interface(dev_handle, idx[1]);
 	if (r != 0) {
 		printf("Cannot release interface \n");
 	}
     else {
         printf("Released interface \n");
     }

 	libusb_close(dev_handle);
 	libusb_exit(ctx);

     return 0;

 }

 // ----------------------------------------------------------------------------

 static int set_knob_with_char(int m, int n, char c1, char c2)
 {

     // Little endian.
     knob[m][n][0] = c2;
     knob[m][n][1] = c1;

     return 0;
 }

 static int set_knob_with_int(int m, int n, int decnum)
 {

     // Taken from stackoverflow. Credits to be added.
     int higher_char, lower_char;
     higher_char = decnum / 256;
     lower_char = decnum % 256;
     knob[m][n][0] = lower_char;
     knob[m][n][1] = higher_char;

     return 0;

 }

 static int hex2bin(unsigned char hexnum, unsigned char* binnum) {

     // Taken from < https://stackoverflow.com/questions/4892579/how-to-convert-a-char-to-binary >.
     for (int i = 0; i < 8; ++i) {
         binnum[i] = ((hexnum >> i) & 1);
     }

    return 0;
 }

 static int set_encoder_with_char(int m, int n, char c, char pos)
 {

     // Convert char byte to binary and then split up the information for the two encoders.
     char binnum[8];
     hex2bin(c, binnum);

     switch(pos) {
         case 's':
             knob[m][n][0] = binnum[0] + binnum[1] * 2 + binnum[2] * 4 + binnum[3] * 8;
             break;
         case 'e':
             knob[m][n][0] = binnum[4] + binnum[5] * 2 + binnum[6] * 4 + binnum[7] * 8;
             break;
     }

     return 0;
 }

 // ----------------------------------------------------------------------------

static int uhid_write(int fd, const struct uhid_event *ev)
{
	ssize_t ret;

	ret = write(fd, ev, sizeof(*ev));
	if (ret < 0) {
		fprintf(stderr, "Cannot write to uhid: %m\n");
		return -errno;
	} else if (ret != sizeof(*ev)) {
		fprintf(stderr, "Wrong size written to uhid: %zd != %zu\n",
			ret, sizeof(ev));
		return -EFAULT;
	} else {
		return 0;
	}

}

static int create(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_CREATE;
	strcpy((char*)ev.u.create.name, "Traktor Kontrol X1");
	ev.u.create.rd_data = rdesc;
	ev.u.create.rd_size = sizeof(rdesc);
	ev.u.create.bus = BUS_USB;
	ev.u.create.vendor = 0x17cc;
	ev.u.create.product = 0x2305;
	ev.u.create.version = 0;
	ev.u.create.country = 0;

	return uhid_write(fd, &ev);
}

static void destroy(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_DESTROY;

	uhid_write(fd, &ev);
}

static void handle_output(struct uhid_event *ev)
{

    // Receive HID packets for the LEDs and forward these to the X1.
    // Note that reading returns gibberish, if too many write attempts are interspersed.
    traktor_usb_bulk_write(idx, ev->u.output.data, ev->u.output.size);

}

static int event(int fd)
{
	struct uhid_event ev;
	ssize_t ret;

	memset(&ev, 0, sizeof(ev));
	ret = read(fd, &ev, sizeof(ev));
	if (ret == 0) {
		fprintf(stderr, "Read HUP on uhid-cdev\n");
		return -EFAULT;
	} else if (ret < 0) {
		fprintf(stderr, "Cannot read uhid-cdev: %m\n");
		return -errno;
	} else if (ret != sizeof(ev)) {
		fprintf(stderr, "Invalid size read from uhid-dev: %zd != %zu\n",
			ret, sizeof(ev));
		return -EFAULT;
	}

	switch (ev.type) {
	case UHID_START:
		fprintf(stderr, "UHID_START from uhid-dev\n");
		break;
	case UHID_STOP:
		fprintf(stderr, "UHID_STOP from uhid-dev\n");
		break;
	case UHID_OPEN:
		fprintf(stderr, "UHID_OPEN from uhid-dev\n");
		break;
	case UHID_CLOSE:
		fprintf(stderr, "UHID_CLOSE from uhid-dev\n");
		break;
	case UHID_OUTPUT:
		fprintf(stderr, "UHID_OUTPUT from uhid-dev\n");
		handle_output(&ev);
		break;
	case UHID_OUTPUT_EV:
		fprintf(stderr, "UHID_OUTPUT_EV from uhid-dev\n");
		break;
	default:
		fprintf(stderr, "Invalid event from uhid-dev: %u\n", ev.type);
	}

	return 0;
}

static int send_event(int fd)
{
	struct uhid_event ev;

	memset(&ev, 0, sizeof(ev));
	ev.type = UHID_INPUT;
	ev.u.input.size = 30;
	ev.u.input.data[0] = 0x1;

    // Knobs. 2 bytes per knob.
	ev.u.input.data[1] = knob[0][0][0]; // 16
	ev.u.input.data[2] = knob[0][0][1]; // 16
	ev.u.input.data[3] = knob[0][1][0]; // 17
	ev.u.input.data[4] = knob[0][1][1]; // 17
	ev.u.input.data[5] = knob[1][0][0]; // 18
	ev.u.input.data[6] = knob[1][0][1]; // 18
	ev.u.input.data[7] = knob[1][1][0]; // 19
	ev.u.input.data[8] = knob[1][1][1]; // 19
	ev.u.input.data[9] = knob[2][0][0]; // 20
	ev.u.input.data[10] = knob[2][0][1]; // 20
    ev.u.input.data[11] = knob[2][1][0]; // 21
	ev.u.input.data[12] = knob[2][1][1]; // 21
	ev.u.input.data[13] = knob[3][0][0]; // 22
	ev.u.input.data[14] = knob[3][0][1]; // 22
	ev.u.input.data[15] = knob[3][1][0]; // 23
	ev.u.input.data[16] = knob[3][1][1]; // 23

    // Endless knobs. 2 bytes per knob.
	ev.u.input.data[17] = knob[4][0][0]; // 0 ... This knob seems to send 0 if not turned when using the caiaq drivers ... Is this intentional?
	ev.u.input.data[18] = knob[4][0][1]; // 0
	ev.u.input.data[19] = knob[4][1][0]; // 1
	ev.u.input.data[20] = knob[4][1][1]; // 1
	ev.u.input.data[21] = knob[5][0][0]; // 2
	ev.u.input.data[22] = knob[5][0][1]; // 2
	ev.u.input.data[23] = knob[5][1][0]; // 40
	ev.u.input.data[24] = knob[5][1][1]; // 40

    // Buttons.
	ev.u.input.data[25] = 0;
	ev.u.input.data[26] = 0;
	ev.u.input.data[27] = 0;
	ev.u.input.data[28] = 0;
	ev.u.input.data[29] = 0;

    // Setting binary flags for the 8 bits of each byte.
	if (btn_down[0][0]) // 284
		ev.u.input.data[25] = 0x1;
    if (btn_down[0][1]) // 288
		ev.u.input.data[25] = 0x2;
	if (btn_down[1][0]) // 285
		ev.u.input.data[25] = 0x4;
    if (btn_down[1][1]) // 289
		ev.u.input.data[25] = 0x8;
	if (btn_down[2][0]) // 286
		ev.u.input.data[25] = 0x10;
    if (btn_down[2][1]) // 290
		ev.u.input.data[25] = 0x20;
	if (btn_down[3][0]) // 287
		ev.u.input.data[25] = 0x40;
    if (btn_down[3][1]) // 291
		ev.u.input.data[25] = 0x80;
    if (btn_down[4][0]) // 280
        ev.u.input.data[26] = 0x1;
    if (btn_down[4][1]) // 292
		ev.u.input.data[26] = 0x2;
	if (btn_down[4][2]) // 281
		ev.u.input.data[26] = 0x4;
    if (btn_down[5][0]) // 265
		ev.u.input.data[26] = 0x8;
	if (btn_down[5][1]) // 264
		ev.u.input.data[26] = 0x10;
    if (btn_down[5][2]) // 294
		ev.u.input.data[26] = 0x20;
	if (btn_down[5][3]) // 293
		ev.u.input.data[26] = 0x40;
    if (btn_down[6][0]) // 282
		ev.u.input.data[26] = 0x80;
	if (btn_down[6][1]) // 295
		ev.u.input.data[27] = 0x1;
    if (btn_down[6][2]) // 283
		ev.u.input.data[27] = 0x2;
	if (btn_down[7][0]) // 276
		ev.u.input.data[27] = 0x4;
    if (btn_down[7][1]) // 259
		ev.u.input.data[27] = 0x8;
	if (btn_down[7][2]) // 268
		ev.u.input.data[27] = 0x10;
    if (btn_down[7][3]) // 275
		ev.u.input.data[27] = 0x20;
	if (btn_down[8][0]) // 258
		ev.u.input.data[27] = 0x40;
    if (btn_down[8][1]) // 277
		ev.u.input.data[27] = 0x80;
	if (btn_down[8][2]) // 274
		ev.u.input.data[28] = 0x1;
    if (btn_down[8][3]) // 269
		ev.u.input.data[28] = 0x2;
	if (btn_down[9][0]) // 257
		ev.u.input.data[28] = 0x4;
    if (btn_down[9][1]) // 278
		ev.u.input.data[28] = 0x8;
	if (btn_down[9][2]) // 273
		ev.u.input.data[28] = 0x10;
    if (btn_down[9][3]) // 270
		ev.u.input.data[28] = 0x20;
	if (btn_down[10][0]) // 256
		ev.u.input.data[28] = 0x40;
    if (btn_down[10][1]) // 279
		ev.u.input.data[28] = 0x80;
	if (btn_down[10][2]) // 272
		ev.u.input.data[29] = 0x1;
    if (btn_down[10][3]) // 271
		ev.u.input.data[29] = 0x2;

	return uhid_write(fd, &ev);
}


static int read_input_via_libusb(int fd)
{

    // Read from X1 via libusb.
    traktor_usb_bulk_read(idx, data_in, length_in);

    // Read bytes 1 to 5 and convert to binary.
    char binbyte[5][8];
    hex2bin(data_in[1], binbyte[0]);
    hex2bin(data_in[2], binbyte[1]);
    hex2bin(data_in[3], binbyte[2]);
    hex2bin(data_in[4], binbyte[3]);
    hex2bin(data_in[5], binbyte[4]);

    btn_down[0][0] = binbyte[3][4];
    btn_down[0][1] = binbyte[4][0];
    btn_down[1][0] = binbyte[3][5];
    btn_down[1][1] = binbyte[4][1];
    btn_down[2][0] = binbyte[3][6];
    btn_down[2][1] = binbyte[4][2];
    btn_down[3][0] = binbyte[3][7];
    btn_down[3][1] = binbyte[4][3];

    btn_down[4][0] = binbyte[3][0];
    btn_down[4][1] = binbyte[4][4];
    btn_down[4][2] = binbyte[3][1];
    btn_down[5][0] = binbyte[1][1];
    btn_down[5][1] = binbyte[1][0];
    btn_down[5][2] = binbyte[4][6];
    btn_down[5][3] = binbyte[4][5];
    btn_down[6][0] = binbyte[3][2];
    btn_down[6][1] = binbyte[4][7];
    btn_down[6][2] = binbyte[3][3];

    btn_down[7][0] = binbyte[2][4];
    btn_down[7][1] = binbyte[0][3];
    btn_down[7][2] = binbyte[1][4];
    btn_down[7][3] = binbyte[2][3];
    btn_down[8][0] = binbyte[0][2];
    btn_down[8][1] = binbyte[2][5];
    btn_down[8][2] = binbyte[2][2];
    btn_down[8][3] = binbyte[1][5];
    btn_down[9][0] = binbyte[0][1];
    btn_down[9][1] = binbyte[2][6];
    btn_down[9][2] = binbyte[2][1];
    btn_down[9][3] = binbyte[1][6];
    btn_down[10][0] = binbyte[0][0];
    btn_down[10][1] = binbyte[2][7];
    btn_down[10][2] = binbyte[2][0];
    btn_down[10][3] = binbyte[1][7];

    set_knob_with_char(0, 0, data_in[16], data_in[17]);
    set_knob_with_char(0, 1, data_in[12], data_in[13]);
    set_knob_with_char(1, 0, data_in[20], data_in[21]);
    set_knob_with_char(1, 1, data_in[10], data_in[11]);
    set_knob_with_char(2, 0, data_in[22], data_in[23]);
    set_knob_with_char(2, 1, data_in[8], data_in[9]);
    set_knob_with_char(3, 0, data_in[18], data_in[19]);
    set_knob_with_char(3, 1, data_in[14], data_in[15]);

    set_encoder_with_char(4, 0, data_in[6], 's');
    set_encoder_with_char(4, 1, data_in[6], 'e');
    set_encoder_with_char(5, 0, data_in[7], 's');
    set_encoder_with_char(5, 1, data_in[7], 'e');

    send_event(fd);

    // LED output.
    data_out[8] = btn_down[0][0] * 0x7F;
    data_out[4] = btn_down[0][1] * 0x7F;
    data_out[7] = btn_down[1][0] * 0x7F;
    data_out[3] = btn_down[1][1] * 0x7F;
    data_out[6] = btn_down[2][0] * 0x7F;
    data_out[2] = btn_down[2][1] * 0x7F;
    data_out[5] = btn_down[3][0] * 0x7F;
    data_out[1] = btn_down[3][1] * 0x7F;

    data_out[29] = btn_down[4][1] * 0x7F;
    data_out[25] = btn_down[5][0] * 0x7F;
    data_out[26] = btn_down[5][1] * 0x7F;
    data_out[27] = btn_down[5][2] * 0x7F;
    data_out[28] = btn_down[5][3] * 0x7F;
    data_out[30] = btn_down[6][1] * 0x7F;
    // data_out[31] = btn_down[6][1] * 0x7F;

    data_out[18] = btn_down[7][0] * 0x7F;
    data_out[17] = btn_down[7][1] * 0x7F;
    data_out[16] = btn_down[7][2] * 0x7F;
    data_out[15] = btn_down[7][3] * 0x7F;
    data_out[20] = btn_down[8][0] * 0x7F;
    data_out[19] = btn_down[8][1] * 0x7F;
    data_out[14] = btn_down[8][2] * 0x7F;
    data_out[13] = btn_down[8][3] * 0x7F;
    data_out[22] = btn_down[9][0] * 0x7F;
    data_out[21] = btn_down[9][1] * 0x7F;
    data_out[12] = btn_down[9][2] * 0x7F;
    data_out[11] = btn_down[9][3] * 0x7F;
    data_out[24] = btn_down[10][0] * 0x7F;
    data_out[23] = btn_down[10][1] * 0x7F;
    data_out[10] = btn_down[10][2] * 0x7F;
    data_out[9] = btn_down[10][3] * 0x7F;

    return 0;

}

// Legacy function for using the caiaq drivers.
static int read_input_via_caiaq(int fd, int device)
{

    struct input_event in_ev;
    read(device, &in_ev, sizeof(in_ev));
    printf("X1: %i %i %i\n", in_ev.type, in_ev.code, in_ev.value);

	switch (in_ev.code) {
		case 284: // Button 0,0
			btn_down[0][0] = in_ev.value;
			send_event(fd);
			break;
    	case 288: // Button 0,1
            btn_down[0][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 285: // Button 1,0
            btn_down[1][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 289: // Button 1,1
            btn_down[1][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 286: // Button 2,0
            btn_down[2][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 290: // Button 2,1
            btn_down[2][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 287: // Button 3,0
            btn_down[3][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 291: // Button 3,1
            btn_down[3][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 280: // Button 4,0 = Knob
            btn_down[4][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 292: // Button 4,1
            btn_down[4][1] = in_ev.value;
    		send_event(fd);
    		break;
        case 281: // Button 4,2 = Knob
            btn_down[4][2] = in_ev.value;
            send_event(fd);
            break;
    	case 265: // Button 5,0
            btn_down[5][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 264: // Button 5,1
            btn_down[5][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 294: // Button 5,2
            btn_down[5][2] = in_ev.value;
    		send_event(fd);
    		break;
    	case 293: // Button 5,3
            btn_down[5][3] = in_ev.value;
    		send_event(fd);
    		break;
    	case 282: // Button 6,0 = Knob
            btn_down[6][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 295: // Button 6,1
            btn_down[6][1] = in_ev.value;
    		send_event(fd);
    		break;
        case 283: // Button 6,2 = Knob
            btn_down[6][2] = in_ev.value;
            send_event(fd);
            break;
    	case 276: // Button 7,0
            btn_down[7][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 259: // Button 7,1
            btn_down[7][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 268: // Button 7,2
            btn_down[7][2] = in_ev.value;
    		send_event(fd);
    		break;
    	case 275: // Button 7,3
            btn_down[7][3] = in_ev.value;
    		send_event(fd);
    		break;
    	case 258: // Button 8,0
            btn_down[8][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 277: // Button 8,1
            btn_down[8][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 274: // Button 8,2
            btn_down[8][2] = in_ev.value;
    		send_event(fd);
    		break;
    	case 269: // Button 8,3
            btn_down[8][3] = in_ev.value;
    		send_event(fd);
    		break;
    	case 257: // Button 9,0
            btn_down[9][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 278: // Button 9,1
            btn_down[9][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 273: // Button 9,2
            btn_down[9][2] = in_ev.value;
    		send_event(fd);
    		break;
    	case 270: // Button 9,3
            btn_down[9][3] = in_ev.value;
    		send_event(fd);
    		break;
    	case 256: // Button 10,0
            btn_down[10][0] = in_ev.value;
    		send_event(fd);
    		break;
    	case 279: // Button 10,1
            btn_down[10][1] = in_ev.value;
    		send_event(fd);
    		break;
    	case 272: // Button 10,2
            btn_down[10][2] = in_ev.value;
    		send_event(fd);
    		break;
    	case 271: // Button 10,3
            btn_down[10][3] = in_ev.value;
    		send_event(fd);
    		break;
		case 16: // Knob 0,0
            // knob[0][0] = in_ev.value;
            set_knob_with_int(0, 0, in_ev.value);
			send_event(fd);
            break;
		case 17: // Knob 0,1
            // knob[0][1] = in_ev.value;
            set_knob_with_int(0, 1, in_ev.value);
            send_event(fd);
            break;
        case 18: // Knob 1,0
            // knob[1][0] = in_ev.value;
            set_knob_with_int(1, 0, in_ev.value);
            send_event(fd);
            break;
		case 19: // Knob 1,1
            // knob[1][1] = in_ev.value;
            set_knob_with_int(1, 1, in_ev.value);
            send_event(fd);
            break;
		case 20: // Knob 2,0
            // knob[2][0] = in_ev.value;
            set_knob_with_int(2, 0, in_ev.value);
            send_event(fd);
            break;
		case 21: // Knob 2,1
            // knob[2][1] = in_ev.value;
            set_knob_with_int(2, 1, in_ev.value);
            send_event(fd);
            break;
		case 22: // Knob 3,0
            // knob[3][0] = in_ev.value;
            set_knob_with_int(3, 0, in_ev.value);
            send_event(fd);
            break;
        case 23: // Knob 3,1
            // knob[3][1] = in_ev.value;
            set_knob_with_int(3, 1, in_ev.value);
            send_event(fd);
            break;
		case 0: // Knob 4,0
            // knob[4][0] = in_ev.value;
            set_knob_with_int(4, 0, in_ev.value);
            send_event(fd);
            break;
		case 1: // Knob 4,1
            // knob[4][1] = in_ev.value;
            set_knob_with_int(4, 1, in_ev.value);
            send_event(fd);
            break;
		case 2: // Knob 5,0
            // knob[5][0] = in_ev.value;
            set_knob_with_int(5, 0, in_ev.value);
            send_event(fd);
            break;
		case 40: // Knob 5,1
            // knob[5][1] = in_ev.value;
            set_knob_with_int(5, 1, in_ev.value);
            send_event(fd);
            break;
		default:
			fprintf(stderr, "Invalid input from element %i\n", in_ev.code);
	}

	return 0;
}

// -----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    // Legacy code for using the caiaq drivers.
    // -------------------------------------------------------------------------
    // // Adapt eventX or pass the path as an argument.
    // const char *devname = "/dev/input/event7";
    // if (argc >= 2) {
    //     devname = argv[1];
    // }
    // int device = open(devname, O_RDWR);
    // ioctl(device, EVIOCGRAB, 1); // Make device unavailable to others. Might not be necessary or even counterproductive.
    // -------------------------------------------------------------------------

    // Setting up the UHID device.
	int fd;
	const char *path = "/dev/uhid";
    struct pollfd pfds[2];
    int ret;
    struct termios state;

    ret = tcgetattr(STDIN_FILENO, &state);
    if (ret) {
        fprintf(stderr, "Cannot get tty state\n");
    } else {
        state.c_lflag &= ~ICANON;
        state.c_cc[VMIN] = 1;
        ret = tcsetattr(STDIN_FILENO, TCSANOW, &state);
        if (ret)
            fprintf(stderr, "Cannot set tty state\n");
    }

	fprintf(stderr, "Open uhid-cdev %s\n", path);
	fd = open(path, O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		fprintf(stderr, "Cannot open uhid-cdev %s: %m\n", path);
		return EXIT_FAILURE;
	}

	fprintf(stderr, "Create uhid device\n");
	ret = create(fd);
	if (ret) {
		close(fd);
		return EXIT_FAILURE;
	}

    // Initialize USB device with libusb.
    idx[0] = 0x0;
    idx[1] = 0x0;
    idx[2] = 0x01;
    idx[3] = 0x81;
    idx[4] = 0x84;
    data_out[0] = 0x0C;
    traktor_usb_init(idx);

    pfds[0].fd = STDIN_FILENO;
	pfds[0].events = POLLIN;
	pfds[1].fd = fd;
	pfds[1].events = POLLIN;

	while (1) {
		ret = poll(pfds, 2, -1);
		if (ret < 0) {
			fprintf(stderr, "Cannot poll for fds: %m\n");
			break;
		}
		if (pfds[0].revents & POLLHUP) {
			fprintf(stderr, "Received HUP on stdin\n");
			break;
		}
		if (pfds[1].revents & POLLHUP) {
			fprintf(stderr, "Received HUP on uhid-cdev\n");
			break;
		}
		if (pfds[0].revents & POLLIN) {
			ret = read_input_via_libusb(fd);
			// ret = read_input_via_caiaq(fd, device);
			if (ret)
				break;
		}
		if (pfds[1].revents & POLLIN) {
			ret = event(fd);
			if (ret)
				break;
		}
	}

    // Close USB device.
    traktor_usb_close(idx);

	fprintf(stderr, "Destroy uhid device\n");
	destroy(fd);
	// destroy(device);
	return EXIT_SUCCESS;
}
