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
 *	$Id: getelem_f.c,v 6.5 2002/10/09 20:59:03 brbarret Exp $
 *
 *	Function:	- MPI_Get_elements F77 wrapper
 */

#include <lam_config.h>

#include <mpi.h>
#include <MPISYSF.h>
#include <mpisys.h>

void
mpi_get_elements_(st, t, cnt, ierr)

int	*st, *t, *cnt, *ierr;

{
	*ierr =	MPI_Get_elements((MPI_Status *) st, GETHDL(*t), cnt);
}
