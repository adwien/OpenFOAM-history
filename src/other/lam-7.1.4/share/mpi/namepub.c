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
 * $Id: namepub.c,v 6.7 2002/12/11 19:15:13 jsquyres Exp $
 *
 *	Function:	- publish a port/service pair
 *	Accepts:	- service name
 *			- info
 *			- port name
 *	Returns:	- MPI_SUCCESS or error code
 */

#include <errno.h>

#include <blktype.h>
#include <mpi.h>
#include <mpisys.h>

/*@
    MPI_Publish_name - Publish a service name for dynamic process control

Input Parameters:
+ service_name - a service name to associate with the port
. info - implementation-specific information
- port_name - a port name from 'MPI_Open_port'

Notes: 

Currently, LAM/MPI ignores any info keys that are passed to this
function.  As such, info keys specific to a different implementation
can safely be passed to this function.  Additionally, the MPI constant
'MPI_INFO_NULL' can also be used.

The scope of the published name is the LAM universe -- MPI processes
running on any node included from the initial lamboot are capable of
retrieving a name published by 'MPI_Publish_name'.  Service names stay
in existence until they are either manually removed with
'MPI_Unpublish_name' or cleaned with the 'lamclean' command.  It is a
common mistake to forget to 'MPI_Unpublish_name' at the end of a run,
and therefore leave published names in the LAM runtime environment.

.N fortran

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_ARG
.N MPI_ERR_SERVICE

.seealso: MPI_Lookup_name, MPI_Unpublish_name

.N WEB
@*/
int
MPI_Publish_name(char *service_name, MPI_Info info, char *port_name)
{
	lam_initerr();
	lam_setfunc(BLKMPIPUBLISHNAME);
/*
 * Check arguments.
 */
	if (port_name == 0 || service_name == 0) {
		return(lam_errfunc(MPI_COMM_WORLD, BLKMPIPUBLISHNAME,
				lam_mkerr(MPI_ERR_ARG, EINVAL)));
	}
/*
 * Publish name.
 */
	if (lam_publish(service_name, port_name)) {
		return(lam_errfunc(MPI_COMM_WORLD, BLKMPIPUBLISHNAME,
				lam_mkerr(MPI_ERR_SERVICE, errno))); 
	}

        lam_resetfunc(BLKMPIPUBLISHNAME);
	return(MPI_SUCCESS);
}
