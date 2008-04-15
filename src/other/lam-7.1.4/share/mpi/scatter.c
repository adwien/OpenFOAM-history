/*
 * Copyright (c) 2001-2003 The Trustees of Indiana University.  
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
 * $Id: scatter.c,v 6.11 2004/03/02 03:43:29 jsquyres Exp $
 *
 *	Function:	- scatters buffers from root in process rank order
 *	Accepts:	- send buffer
 *			- send count
 *			- send datatype
 *			- recv buffer
 *			- recv count
 *			- recv datatype
 *			- root
 *			- communicator
 *	Returns:	- MPI_SUCCESS or error code
 */

#include <lam_config.h>
#include <app_mgmt.h>
#include <blktype.h>
#include <mpi.h>
#include <mpisys.h>
#include <rpisys.h>

/*@

MPI_Scatter - Sends data from one task to all other tasks in a group

Input Parameters:
+ sbuf - address of send buffer (choice, significant 
only at 'root') 
. scount - number of elements sent to each process 
(integer, significant only at 'root') 
. sdtype - data type of send buffer elements (significant only at 'root') 
(handle) 
. rcount - number of elements in receive buffer (integer) 
. rdtype - data type of receive buffer elements (handle) 
. root - rank of sending process (integer) 
- comm - communicator (handle) 

Output Parameter:
. rbuf - address of receive buffer (choice) 

.N IMPI

.N fortran

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_COMM
.N MPI_ERR_COLL_NOT_IMPLEMENTED
.N MPI_ERR_IMPI
.N MPI_ERR_COUNT
.N MPI_ERR_TYPE
.N MPI_ERR_BUFFER
.N MPI_ERR_ROOT

.N ACK
@*/
int MPI_Scatter(void *sbuf, int scount, MPI_Datatype sdtype, 
		void *rbuf, int rcount, MPI_Datatype rdtype, 
		int root, MPI_Comm comm)
{
        int rank;
	int size;
	int err;
	lam_ssi_coll_scatter_fn_t func;

	lam_initerr();
	lam_setfunc(BLKMPISCATTER);
/*
 * Check for invalid arguments.
 */
	if (comm == MPI_COMM_NULL) {
	  return(lam_errfunc(comm, BLKMPISCATTER,
			     lam_mkerr(MPI_ERR_COMM, EINVAL)));
	}
	func = comm->c_ssi_coll.lsca_scatter;

	if (LAM_IS_INTRA(comm)) {
	  MPI_Comm_rank(comm, &rank);
	  MPI_Comm_size(comm, &size);
	  if ((root >= size) || (root < 0)) {
	    return(lam_errfunc(comm, BLKMPISCATTER,
			       lam_mkerr(MPI_ERR_ROOT, EINVAL)));
	  }
	  
	  if ((root == rank && sdtype == MPI_DATATYPE_NULL) ||
	      (rdtype == MPI_DATATYPE_NULL)) {
	    return(lam_errfunc(comm, BLKMPISCATTER,
			       lam_mkerr(MPI_ERR_TYPE, EINVAL)));
	  }
	  
	  if ((rcount < 0) || (rank == root && scount < 0)) {
	    return(lam_errfunc(comm, BLKMPISCATTER,
			       lam_mkerr(MPI_ERR_COUNT, EINVAL)));
	  }
	} else {
	  MPI_Comm_remote_size(comm, &size);
	  if (((root != MPI_PROC_NULL) && (rdtype == MPI_DATATYPE_NULL)) ||
	      (root == MPI_ROOT  && sdtype == MPI_DATATYPE_NULL)) {
	    return(lam_errfunc(comm, BLKMPIGATHER,
			       lam_mkerr(MPI_ERR_TYPE, EINVAL)));
	  }
	  if (!(((root < size) && (root >= 0)) 
		|| (root == MPI_ROOT) || (root == MPI_PROC_NULL))) {
	    return(lam_errfunc(comm, BLKMPIGATHER, 
			       lam_mkerr(MPI_ERR_ROOT, EINVAL)));
	  }
	}
	  
	if (func == NULL) {
	  return(lam_errfunc(comm, BLKMPISCATTER,
			     lam_mkerr(MPI_ERR_OTHER, ENOT_IMPLEMENTED)));
	}

	LAM_TRACE(lam_tr_cffstart(BLKMPISCATTER));

	/* Call the coll SSI to actually perform the operation */
	
	if ((err = func(sbuf, scount, sdtype, rbuf, rcount, rdtype,
			root, comm) != MPI_SUCCESS))
	  return(lam_errfunc(comm, BLKMPISCATTER,
			     lam_mkerr(MPI_ERR_COMM, err)));

	/* All done */

	LAM_TRACE(lam_tr_cffend(BLKMPISCATTER, root, comm, rdtype, rcount));
	lam_resetfunc(BLKMPISCATTER);
	return(MPI_SUCCESS);
}
