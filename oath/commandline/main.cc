#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include <initializer_list>

#include <usb.h>

extern "C" {
	#include "opendevice.h"
	#include "../firmware/usbconfig.h"
	#include "../firmware/requests.h"
}

uint16_t read_raw_id(std::initializer_list<uint8_t> l) {
	return *(l.begin() + 1) << 8 | *l.begin();
}

// http://tools.ietf.org/html/rfc6238 test secret
uint8_t secret[] = {
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', \
    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
};

int main(int argc, char **argv) {
	usb_init();

	usb_dev_handle *h;
	uint16_t vid = read_raw_id({ USB_CFG_VENDOR_ID });
	uint16_t pid = read_raw_id({ USB_CFG_DEVICE_ID });
	char vendor[] = {USB_CFG_VENDOR_NAME, 0};
	char product[] = {USB_CFG_DEVICE_NAME, 0};
	char serial[] = {USB_CFG_SERIAL_NUMBER, 0};

	int r = usbOpenDevice(&h, vid, vendor, pid, product, serial, NULL, NULL);
	if (r != 0) {
		perror("Couldn't open device");
		exit(1);
	}

	const char *action = argv[1];

	if (strcmp(action, "setkey") == 0) {
		int r = usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
		                        OATH_RQ_SET_KEY, 0, 0, (char*) secret, sizeof(secret), 5000);
		printf("setkey: r=%i\n", r);
	}
	else if (strcmp(action, "setlen") == 0) {
		const char *lenA = argv[2];
		uint8_t len = atoi(lenA);
		printf("Setting length to %i\n", len);
		usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
		                OATH_RQ_SET_LENGTH, len, 0, NULL, 0, 5000);
	}
	else if (strcmp(action, "typecode") == 0) {
		const char *counterA = argv[2];
		uint32_t counter = atoi(counterA);
		printf("Generating code with counter %i\n", counter);
		usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
		                OATH_RQ_TYPE_CODE, (counter >> 16), (counter & 0xffff), NULL, 0, 5000);
	}
	else if (strcmp(action, "settime") == 0) {
		timeval time;
		gettimeofday(&time, NULL);
		uint32_t time32 = time.tv_sec;
		printf("Setting time to %i\n", time32);
		usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
		                OATH_RQ_SET_TIME, (time32 >> 16), (time32 & 0xffff), NULL, 0, 5000);
	}
	else {
		fprintf(stderr, "No command!\n");
		exit(-1);
	}

	return 0;
}
