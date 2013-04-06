/*
 * IO Edgeport Driver tables
 *
 *	Copyright (C) 2001
 *	    Greg Kroah-Hartman (greg@kroah.com)
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 * 
 */

/* Devices that this driver supports */
static __u16	ionetworks_vendor_id	= USB_VENDOR_ID_ION;
static __u16	edgeport_4_id		= ION_DEVICE_ID_EDGEPORT_4;
static __u16	rapidport_4_id		= ION_DEVICE_ID_RAPIDPORT_4;
static __u16	edgeport_4t_id		= ION_DEVICE_ID_EDGEPORT_4T;
static __u16	edgeport_2_id		= ION_DEVICE_ID_EDGEPORT_2;
static __u16	edgeport_4i_id		= ION_DEVICE_ID_EDGEPORT_4I;
static __u16	edgeport_2i_id		= ION_DEVICE_ID_EDGEPORT_2I;
static __u16	edgeport_prl_id		= ION_DEVICE_ID_EDGEPORT_PARALLEL_PORT;
static __u16	edgeport_421_id		= ION_DEVICE_ID_EDGEPORT_421;
static __u16	edgeport_21_id		= ION_DEVICE_ID_EDGEPORT_21;
static __u16	edgeport_8dual_id	= ION_DEVICE_ID_EDGEPORT_8_DUAL_CPU;
static __u16	edgeport_8_id		= ION_DEVICE_ID_EDGEPORT_8;
static __u16	edgeport_2din_id	= ION_DEVICE_ID_EDGEPORT_2_DIN;
static __u16	edgeport_4din_id	= ION_DEVICE_ID_EDGEPORT_4_DIN;
static __u16	edgeport_16dual_id	= ION_DEVICE_ID_EDGEPORT_16_DUAL_CPU;
static __u16	edgeport_compat_id	= ION_DEVICE_ID_EDGEPORT_COMPATIBLE;
static __u16	edgeport_8i_id		= ION_DEVICE_ID_EDGEPORT_8I;


/* build up the list of devices that this driver supports */
struct usb_serial_device_type edgeport_4_device = {
	name:			"Edgeport 4",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_4_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type rapidport_4_device = {
	name:			"Rapidport 4",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&rapidport_4_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_4t_device = {
	name:			"Edgeport 4t",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_4t_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_2_device = {
	name:			"Edgeport 2",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_2_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		2,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_4i_device = {
	name:			"Edgeport 4i",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_4i_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_2i_device = {
	name:			"Edgeport 2i",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_2i_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		2,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_prl_device = {
	name:			"Edgeport Parallel",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_prl_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		1,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_421_device = {
	name:			"Edgeport 421",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_421_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		2,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_21_device = {
	name:			"Edgeport 21",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_21_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		2,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_8dual_device = {
	name:			"Edgeport 8 dual cpu",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_8dual_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_8_device = {
	name:			"Edgeport 8",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_8_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		8,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_2din_device = {
	name:			"Edgeport 2din",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_2din_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		2,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_4din_device = {
	name:			"Edgeport 4din",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_4din_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_16dual_device = {
	name:			"Edgeport 16 dual cpu",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_16dual_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		8,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_compat_id_device = {
	name:			"Edgeport Compatible",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_compat_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		4,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};

struct usb_serial_device_type edgeport_8i_device = {
	name:			"Edgeport 8i",
	idVendor:		&ionetworks_vendor_id,
	idProduct:		&edgeport_8i_id,
	needs_interrupt_in:	MUST_HAVE,
	needs_bulk_in:		MUST_HAVE,
	needs_bulk_out:		MUST_HAVE,
	num_interrupt_in:	1,
	num_bulk_in:		1,
	num_bulk_out:		1,
	num_ports:		8,
	open:			edge_open,
	close:			edge_close,
	throttle:		edge_throttle,
	unthrottle:		edge_unthrottle,
	startup:		edge_startup,
	shutdown:		edge_shutdown,
	ioctl:			edge_ioctl,
	set_termios:		edge_set_termios,
	write:			edge_write,
	write_room:		edge_write_room,
	chars_in_buffer:	edge_chars_in_buffer,
	break_ctl:		edge_break,
};



