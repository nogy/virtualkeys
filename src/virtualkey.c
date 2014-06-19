/*
 * virtualkey.c
 *
 *  Created on: 07.06.2014
 *      Author: bbb
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argtable2.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/XInput.h>
#include <X11/extensions/XI.h>

struct arg_lit* cl_help;
struct arg_int* cl_press;
struct arg_int* cl_release;
struct arg_int* cl_toggle;
struct arg_lit* cl_display;
struct arg_end* cl_end;

#define DEBUG

#define err(...)	fprintf(stderr, __VA_ARGS__)
#ifdef DEBUG
#define dbg(...)	fprintf(stderr, __VA_ARGS__)
#else
#define dbg(...)	fprintf(stderr, "");
#endif

int handle_arguments(int argc, char** argv) {
	/* BEGIN argument tables */
	cl_help = arg_lit0("h", "help", "display help message");
	cl_press = arg_int0("p", "press", "<n>", "press a virtual key");
	cl_release = arg_int0("r", "release", "<n>", "release a virtual key");
	cl_toggle = arg_int0("t", "toggle", "<n>", "toggle a virtual key");
	cl_display = arg_lit0("d", "display", "waits for a key to be pressed and displays the keycode");
	cl_end = arg_end(20);
	void* argtable[] = {cl_help, cl_press, cl_release, cl_toggle, cl_display, cl_end};
	/* END argument tables */

	int nerr = arg_parse(argc, argv, argtable);
	if (nerr > 0 || cl_help->count>0 || argc < 2) {
		arg_print_errors(stderr, cl_end, "virtualkey");
		err("Syntax: virtualkey");
		arg_print_syntax(stderr, argtable, "\n");
		arg_print_glossary(stderr, argtable, " %-25s %s\n");
		return 1;
	}
	return 0;
}

int main(int argc, char** argv) {
	int ret;
	Display* disp;
	ret = handle_arguments(argc, argv);
	if (ret != 0)
		return ret;

	if (cl_press->count+cl_release->count+cl_toggle->count+cl_display->count > 1) {
		err("You can either press, release, toggle or display. No combinations are allowed\n");
		return 1;
	}

	disp = XOpenDisplay(NULL);
	if (disp == NULL) {
		err("Unable to open Display\n");
		return 1;
	}

	// Take control over XTest device
	XTestGrabControl(disp, True);

	// Press a key
	if (cl_press->count > 0) {
		dbg("pressing\n");
		XTestFakeKeyEvent(disp, *(cl_press->ival), True, 0);
	}

	// Release a key
	if (cl_release->count > 0) {
		dbg("releasing\n");
		XTestFakeKeyEvent(disp, *(cl_release->ival), False, 0);
	}

	// Display key matrix
	if (cl_display->count > 0) {
		int i, ndev;
		XDeviceInfo* xdi = XListInputDevices(disp, &ndev);
		for (i=0; i<ndev; i++) {
			if (strstr(xdi[i].name, "XTEST keyboard") != NULL) {
				dbg( "Keyboard found -> id=%i\n", (int)xdi[i].id);
				break;
			}
		}
		XDevice* xd = XOpenDevice(disp, xdi[i].id);
		XDeviceState* xds = XQueryDeviceState(disp,xd);
		XKeyboardState* xkbds = xds;
		for (i=0; i<32; i++) {
			dbg("i=%i - %02x\n", i, 0xFF & xkbds->auto_repeats[i]);
		}

		XFreeDeviceState(xds);
		XCloseDevice(disp, xd);
		XFreeDeviceList(xdi);
	}

	// Toggle a key
	if (cl_toggle->count >0) {
		int i, ndev;
		int keys, doPress;
		XDeviceInfo* xdi = XListInputDevices(disp, &ndev);
		for (i=0; i<ndev; i++) {
			if (strstr(xdi[i].name, "XTEST keyboard") != NULL) {
				dbg( "Keyboard found -> id=%i\n", (int)xdi[i].id);
				break;
			}
		}
		XDevice* xd = XOpenDevice(disp, xdi[i].id);
		XDeviceState* xds = XQueryDeviceState(disp,xd);
		XKeyboardState* xkbds = xds;

		keys = xkbds->auto_repeats[(*(cl_toggle->ival))/8];
		dbg( "Keys found: %02x\n", keys);
		doPress = (keys & (1<<(*(cl_toggle->ival))%8));
		dbg( "Should press: %i\n", doPress);
		XTestFakeKeyEvent(disp, *(cl_toggle->ival), doPress==0, 0);

		XFreeDeviceState(xds);
		XCloseDevice(disp, xd);
		XFreeDeviceList(xdi);
	}

	// Sync and release control
	XSync(disp, False);
	XTestGrabControl(disp, False);

	return 0;
}

