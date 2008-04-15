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
 *	Copyright 1996 The Ohio	State University
 *	RBD/NJN
 *
 *	$Id: alltoallv_f.c,v 6.4 2002/10/09 20:59:00 brbarret Exp $
 *
 *	Function:	- MPI_Alltoallv	F77 wrapper
 */

#include <lam_config.h>

#include <mpi.h>
#include <MPISYSF.h>
#include <mpisys.h>

void 
mpi_alltoallv_(sb, sc, sd, st, rb, rc, rd, rt, c, ierr)

char	*sb, *rb;
int	*sc, *sd, *st, *rc, *rd, *rt, *c, *ierr;

{
	*ierr =	MPI_Alltoallv(BUF(sb), sc, sd, GETHDL(*st),
			      BUF(rb), rc, rd, GETHDL(*rt), GETHDL(*c));
}
