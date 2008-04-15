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
 *	Copyright 1997 The Ohio	State University
 *	RBD/NJN
 *
 *	$Id: errcreate_f.c,v 6.5 2002/10/09 20:59:02 brbarret Exp $
 *
 *	Function:	- MPI_Errhandler_create	F77 wrapper
 */

#include <lam_config.h>

#include <blktype.h>
#include <mpi.h>
#include <MPISYSF.h>
#include <mpisys.h>

void
mpi_errhandler_create_(func, hdl, ierr)

void	 (*func) ();
int	*hdl, *ierr;

{
	MPI_Errhandler errhdl;

	*ierr =	MPI_Errhandler_create((MPI_Handler_function *) func, &errhdl);

	if (lam_F_make_hdl(hdl, ierr, errhdl, BLKMPIERRCREATE)) {
		errhdl->eh_f77handle = *hdl;
		errhdl->eh_flags |= LAM_LANGF77;
	}
}
