/*
 * Copyright (c) 2001-2002 The Trustees of Indiana University.  
 *                         All rights reserved.
 * Copyright (c) 1998-2001 University of Notre Dame. 
 *                         All rights reserved.
 * Copyright (c) 1994-1998 The Ohio State University.  
 *                         All rights reserved.
 * 
 * This file is part of the LAM/MPI software package.  For license
 * information, see the LICENSE file in the top level directory of the
 * LAM/MPI source distribution.
 * 
 * $HEADER$
 *	Ohio Trollius
 *	Copyright 1997 The Ohio State University
 *	RBD/GDB
 *
 *	$Id: trecv.c,v 6.5 2002/10/09 20:59:50 brbarret Exp $
 * 
 *	Function:	- transport layer receive
 */

#include <lam_config.h>
#include <etc_misc.h>
#include <portable.h>
#include <typical.h>
#include <net.h>

#include <unistd.h>

int
trecv(nhead)

struct nmsg		*nhead;		/* network message descriptor */

{
	struct nmsg	sync;		/* synchronization message */
	int		save_event;	/* saved message event */
	int		ret;		/* returned value */
/*
 * Receive "ready to send" message.
 */
	sync.nh_event = nhead->nh_event;
	sync.nh_type = nhead->nh_type;
	sync.nh_flags = DINT4DATA;
	sync.nh_length = 0;
	sync.nh_msg = (char *) 0;

	if (nrecv(&sync)) return(LAMERROR);
/*
 * Send "ready to receive" to sending party.  Include the event to be used.
 */
	sync.nh_node = sync.nh_data[0];
	sync.nh_event = sync.nh_data[1];
	sync.nh_type = 0;
	sync.nh_flags = DINT4DATA;
	sync.nh_data[0] = -lam_getpid();

	if (nsend(&sync)) return(LAMERROR);
/*
 * Receive the data message using the new event.
 */
	save_event = nhead->nh_event;
	nhead->nh_event = -lam_getpid();

	ret = nrecv(nhead);

	nhead->nh_event = save_event;
	return(ret);
}
