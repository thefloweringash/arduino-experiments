/* Name: requests.h
 * Project: custom-class, a basic USB example
 * Author: Christian Starkjohann
 * Creation Date: 2008-04-09
 * Tabsize: 4
 * Copyright: (c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt), GNU GPL v3 or proprietary (CommercialLicense.txt)
 * This Revision: $Id$
 */

/* This header is shared between the firmware and the host software. It
 * defines the USB request numbers (and optionally data types) used to
 * communicate between the host and the device.
 */

#ifndef __REQUESTS_H_INCLUDED__
#define __REQUESTS_H_INCLUDED__

/* Request that the device sends back wValue and wIndex. This is used with
 * random data to test the reliability of the communication.
 */
#define USBRF_RQ_ECHO          0

/* Set the LED status. Control-OUT.
 * The requested status is passed in the "wValue" field of the control
 * transfer. No OUT data is sent. Bit 0 of the low byte of wValue controls
 * the LED.
 */
#define USBRF_RQ_SEND_CODE    1

#endif /* __REQUESTS_H_INCLUDED__ */
