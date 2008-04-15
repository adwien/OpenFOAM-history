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
 *	NJN/RBD
 *
 *	$Id: waitany_f.c,v 6.7 2002/10/09 20:59:10 brbarret Exp $
 *
 *	Function:	- MPI_Waitany F77 wrapper
 */

#include <lam_config.h>

#include <errno.h>
#include <stdlib.h>

#include <blktype.h>
#include <mpi.h>
#include <MPISYSF.h>
#include <mpisys.h>

void
mpi_waitany_(c, rqs, idx, st, ierr)

int			*c, *rqs, *idx, *st, *ierr;

{
	MPI_Request	*reqs;
	MPI_Status	*stat;
	int		i;

	if (*c > 0) {
		reqs = (MPI_Request *) malloc((*c) * sizeof(MPI_Request));
		if (reqs == 0) {
			lam_setfunc(BLKMPIWAITANY);
			*ierr = lam_err_comm(MPI_COMM_WORLD,
						MPI_ERR_OTHER, errno, "");
			return;
		}
	} else {
		reqs = 0;
	}

	for (i = 0; i < *c; ++i) {
		reqs[i] = GETHDL(rqs[i]);
	}

	if ((void *) st == lam_F_status_ignore) {
		stat = LAM_MPI_C_STATUS_IGNORE;
	}
	else if ((void *) st == lam_F_statuses_ignore) {
		lam_setfunc(BLKMPIWAITANY);
		*ierr = lam_err_comm(MPI_COMM_WORLD, MPI_ERR_ARG, 0,
					"MPI_STATUSES_IGNORE");
		return;
	}
	else {
		stat = (MPI_Status *) st;
	}
	
	*ierr = MPI_Waitany(*c, reqs, idx, stat);

	if (*ierr == MPI_SUCCESS && *idx >= 0) {
		if (reqs[*idx] == LAM_MPI_C_REQUEST_NULL) {
			rqs[*idx] = -1;
		}

		++(*idx);
	}

	free((char *) reqs);
}
