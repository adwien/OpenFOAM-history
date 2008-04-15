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
 *	RBD/NJN
 *
 *	$Id: bsendinit_f.c,v 6.5 2002/10/09 20:59:00 brbarret Exp $
 *
 *	Function:	- MPI_Bsend_init F77 wrapper
 */

#include <lam_config.h>

#include <blktype.h>
#include <mpi.h>
#include <MPISYSF.h>
#include <mpisys.h>
#include <rpisys.h>

void
mpi_bsend_init_(b, n, t, d, tag, c, r, ierr)

char	*b;
int	*n, *t, *d, *tag, *c, *r, *ierr;


{
	MPI_Request req;

	*ierr = MPI_Bsend_init(BUF(b), *n, GETHDL(*t),
			       *d, *tag, GETHDL(*c), &req);

	if (lam_F_make_hdl(r, ierr, req, BLKMPIBSINIT)) {
		req->rq_f77handle = *r;
	}
}
