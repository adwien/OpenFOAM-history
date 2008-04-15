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
 *
 *	$Id: init_f.c,v 6.8 2003/05/07 20:13:41 brbarret Exp $
 *
 *	Function:	- MPI_Init F77 wrapper
 */

#include <lam_config.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>
#include <MPISYSF.h>
#include <mpisys.h>


void
mpi_init_(int *ierror)
{
	int		argc;
	char		*argv[2];
	char		**pargv;
/*
 * Do an MPI_Init().
 */
	argc = 1;
	argv[0] = getenv("LAM_MPI_PROCESS_NAME");
	if (argv[0] == NULL)
	  argv[0]	= "LAM_MPI_Fortran_program";
	argv[1] = NULL;
	pargv =	argv;

	lam_f77init = 1;
	*ierror	= MPI_Init(&argc, &pargv);
}
