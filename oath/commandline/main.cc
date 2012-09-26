#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

#include <initializer_list>

#include <usb.h>

extern "C" {
	#include "opendevice.h"
}

#include "../firmware/usbconfig.h"
#include "../firmware/requests.h"
#include "parse.h"

uint16_t read_raw_id(std::initializer_list<uint8_t> l) {
	return *(l.begin() + 1) << 8 | *l.begin();
}

void usage(const char *name) {
	fprintf(stderr, "%s setkey [hex|base32]:FILE | setlen N | typecode N | settime | gettime\n", name);
}

struct parser_t {
	const char *name;
	int(*parse)(FILE *f, uint8_t *secret, const size_t secretLen);
};

static int parse_with_file(const char *filename, uint8_t *secret, const size_t secretLen,
                           const parser_t *parser)
{
	printf("Reading key from file: %s\n", filename);

	FILE *file = NULL;

	if (strcmp(filename, "-") == 0) {
		file = stdin;
	}
	else {
		file = fopen(filename, "r");
		if (!file) {
			perror("fopen");
			fprintf(stderr, "Couldn't open secret file, aborting\n");
			return -1;
		}
	}

	int r = parser->parse(file, secret, secretLen);

	fclose(file);
	return r;
}

static char *substr_dup(const char *src, size_t from, size_t to) {
	const size_t len = to - from;
	char *dest = reinterpret_cast<char*>(malloc(len + 1));
	dest[len] = 0;
	memcpy(dest, src, len);
	return dest;
}

const struct parser_t parsers[] = {
	{ "hex", parse_hex_key },
	{ "base32", parse_base32_key },
};

int main(int argc, char **argv) {
	#define CHECK_ARG_LEN(n) do { if (argc != n) { usage(argv[0]); goto error_cleanup_usb; } } while (0);

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

	if (argc < 2) {
		usage(argv[0]);
		goto error_cleanup_usb;
	}

	{
		const char *action = argv[1];

		if (strcmp(action, "setkey") == 0) {
			CHECK_ARG_LEN(3);

			uint8_t secret[20] = {0};

			const parser_t *parser = &parsers[0];

			const char *filename = argv[2];
			char *colonPos = strchr(argv[2], ':');
			if (colonPos) {
				char *type = substr_dup(filename, 0, colonPos - filename);
				filename = colonPos + 1;
				const parser_t *i = &parsers[0];
				const parser_t *end = &parsers[sizeof(parsers) / sizeof(*parsers)];
				while (i < end) {
					if (strcmp(i->name, type) == 0) {
						parser = i;
						break;
					}
					i++;
				}
				free(type);
				if (!parser) {
					fprintf(stderr, "Unknown string type: %s", type);
					exit(-1);
				}
			}
			else {
				parser = &parsers[0];
			}

			if (parse_with_file(filename, &secret[0], sizeof(secret), parser) < 0) {
				fprintf(stderr, "Failed to parse key\n");
				exit(-1);
			}

			printf("Parsed key: ");
			for (int i = 0; i < sizeof(secret); i++) {
				printf("%.2x", secret[i]);
			}
			printf("\n");

			int r = usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT,
			                        OATH_RQ_SET_KEY, 0, 0, (char*) secret, sizeof(secret), 5000);
			printf("setkey: r=%i\n", r);
		}
		else if (strcmp(action, "setlen") == 0) {
			CHECK_ARG_LEN(3);
			const char *lenA = argv[2];
			uint8_t len = atoi(lenA);
			printf("Setting length to %i\n", len);
			usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
			                OATH_RQ_SET_LENGTH, len, 0, NULL, 0, 5000);
		}
		else if (strcmp(action, "typecode") == 0) {
			CHECK_ARG_LEN(3);
			const char *counterA = argv[2];
			uint32_t counter = atoi(counterA);
			printf("Generating code with counter %i\n", counter);
			int r = usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
			                        OATH_RQ_TYPE_CODE, (counter >> 16), (counter & 0xffff), NULL, 0, 5000);
			printf("typecode result = %i\n", r);
		}
		else if (strcmp(action, "settime") == 0) {
			CHECK_ARG_LEN(2);
			timeval time;
			gettimeofday(&time, NULL);
			uint32_t time32 = time.tv_sec;
			printf("Setting time to %i\n", time32);
			int r = usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
			                        OATH_RQ_SET_TIME, (time32 >> 16), (time32 & 0xffff), NULL, 0, 5000);
			printf("settime result = %i\n", r);
		}
		else if (strcmp(action, "gettime") == 0) {
			CHECK_ARG_LEN(2);
			char timeBuffer[4] = {0};
			uint32_t time = 0;
			int r = usb_control_msg(h, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
			                        OATH_RQ_GET_TIME, 0, 0, timeBuffer, sizeof(timeBuffer), 5000);
			memcpy(&time, timeBuffer, sizeof(timeBuffer));
			printf("gettime time is %u (result = %i)\n", time, r);
		}
		else {
			fprintf(stderr, "Unknown command! Command was \"%s\"\n", action);
			usage(argv[0]);
			goto error_cleanup_usb;
		}
	}
	return 0;

error_cleanup_usb:
	fprintf(stderr, "Aborting! Cleaning up...\n");
	usb_close(h);
	return -1;
}
