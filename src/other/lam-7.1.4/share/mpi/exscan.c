/*
 * Copyright (c) 2001-2004 The Trustees of Indiana University.  
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
 * $Id: exscan.c,v 6.3 2004/03/02 03:43:29 jsquyres Exp $
 *
 *	Function:	- partial reduce from 0 to rank i - 1, at each rank i
 *                      - not applicable for rank 0
 *	Accepts:	- send buffer
 *			- receive buffer
 *			- count of elements
 *			- type of elements
 *			- operation to perform
 *			- communicator
 *	Returns:	- MPI_SUCCESS or error code
 */

#include <stdlib.h>

#include <lam_config.h>
#include <blktype.h>
#include <mpi.h>
#include <mpisys.h>

/*@

MPI_Exscan - Computes the exclusive scan (partial reductions) of data on
             a collection of processes

Input Parameters:
+ sbuf - starting address of send buffer (choice) 
. count - number of elements in input buffer (integer) 
. dtype - data type of elements of input buffer (handle) 
. op - operation (handle) 
- comm - communicator (handle) 

Output Parameter:
. rbuf - starting address of receive buffer (choice). Not significant for 
         rank 0. 

Note thst MPI does not define this collective operation for
intercommunicators.  Calling this function with an intercommunicator
will result in the MPI_ERR_COMM exception being invoked.

.N IMPI

.N fortran

.N collops

.N Errors
.N MPI_SUCCESS
.N MPI_ERR_COMM
.N MPI_ERR_COLL_NOT_IMPLEMENTED
.N MPI_ERR_IMPI
.N MPI_ERR_COUNT
.N MPI_ERR_TYPE
.N MPI_ERR_OP
.N MPI_ERR_BUFFER
.N MPI_ERR_BUFFER_ALIAS

.N ACK
@*/
int MPI_Exscan(void *sbuf, void *rbuf, int count, 
	       MPI_Datatype dtype, MPI_Op op, MPI_Comm comm)
{
        int err;
	lam_ssi_coll_exscan_fn_t func;

	lam_initerr_m();
	lam_setfunc_m(BLKMPIEXSCAN);
/*
 * Check for invalid arguments.
 */
	if (comm == MPI_COMM_NULL) {
	  return (lam_errfunc(comm, BLKMPIEXSCAN,
			      lam_mkerr(MPI_ERR_COMM, EINVAL)));
	}
	func = comm->c_ssi_coll.lsca_exscan;

	if (LAM_IS_INTER(comm)) {
	  return(lam_errfunc(comm, BLKMPIEXSCAN,
			     lam_mkerr(MPI_ERR_COMM, EINVAL)));
	}

	if (dtype == MPI_DATATYPE_NULL) {
	  return(lam_errfunc(comm, BLKMPIEXSCAN,
			     lam_mkerr(MPI_ERR_TYPE, EINVAL)));
	}

	if (count < 0) {
	  return(lam_errfunc(comm, BLKMPIEXSCAN,
			     lam_mkerr(MPI_ERR_COUNT, EINVAL)));
	}

	if (op == MPI_OP_NULL) {
	  return(lam_errfunc(comm, BLKMPIEXSCAN,
			     lam_mkerr(MPI_ERR_OP, EINVAL)));
	}

	if (func == NULL) {
	  return(lam_errfunc(comm, BLKMPIEXSCAN,
			     lam_mkerr(MPI_ERR_OTHER, ENOT_IMPLEMENTED)));
	}

	LAM_TRACE(lam_tr_cffstart(BLKMPIEXSCAN));

	/* Check for zero count case. */
	
	if (count == 0) {
	  LAM_TRACE(lam_tr_cffend(BLKMPIEXSCAN, -1, comm, dtype, count));
	  lam_resetfunc_m(BLKMPISCAN);
	  return MPI_SUCCESS;
	}

	/* Call the coll SSI to actually perform the barrier */

	if ((err = func(sbuf, rbuf, count, dtype, op, comm)) != MPI_SUCCESS)
	  return(lam_errfunc(comm, BLKMPIEXSCAN,
			     lam_mkerr(MPI_ERR_COMM, err)));

	/* All done */

	LAM_TRACE(lam_tr_cffend(BLKMPIEXSCAN, -1, comm, dtype, count));
	lam_resetfunc(BLKMPIEXSCAN);
	return MPI_SUCCESS;
}
