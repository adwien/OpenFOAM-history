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
 *	$Id: regsyncack.c,v 1.2 2002/10/09 20:57:18 brbarret Exp $
 *
 *	Function:	- post a receive for a ping from the impid
 */

#include <lam_config.h>
#if LAM_WANT_IMPI

#include <impi-defs.h> 
#include <impi.h>
#include <mpi.h>
#include <mpisys.h>
#include <rpisys.h>


/*
 *	IMPI_Register_syncack
 *
 *	Function:	- register to receive a syncack ping from the impid
 *                      - impid will send a ping to ack the receipt of
 *                        a DATASYNC on the remote site
 *                      - hence, this function is only invoked for sends
 *      Accepts:        - rank in comm where the real message is from
 *                      - tag of real message
 *                      - comm of real message
 *                      - ptr to request to fill
 *      Returns:        - MPI_SUCCESS on success, err otherwise
 *                      - fills MPI_Request if this is an IMPI request,
 *                        sets to MPI_REQUEST_NULL otherwise 
 */
int
IMPI_Register_syncack(int rank, int tag, MPI_Comm comm, MPI_Request *req)
{
  int cid;
  int err;
  IMPI_Uint8 *drqid;

  *req = MPI_REQUEST_NULL;

  if (LAM_IS_IMPI(comm) &&
      lam_impid_proc != 0 &&
      (LAM_GPSCMP(&(comm->c_group->g_procs[rank]->p_gps),
		  &(lam_impid_proc->p_gps)) == 0 ||
       rank == MPI_ANY_SOURCE)) {

    /* Change the CID of the request so that we won't interfere with
       the user's real message */

    cid = comm->c_contextid;
    comm->c_contextid = lam_pt2impisyncack(cid);

    drqid = (IMPI_Uint8 *) malloc(sizeof(IMPI_Uint8));
    err = _mpi_req_build(drqid, 1, IMPI_TYPE_UINT8, rank, tag, comm, 
			 LAM_RQIRECV, req);
    if (err != MPI_SUCCESS) 
      return(err);

    (*req)->rq_marks |= (LAM_RQFDYNBUF | LAM_RQFMAND);
    (*req)->rq_hdlr = 0;

    err = _mpi_req_start(*req);
    if (err != MPI_SUCCESS) {
      *req = MPI_REQUEST_NULL;
      return err;
    }

    _mpi_req_add_m(*req);

    comm->c_contextid = cid;
  }

  return MPI_SUCCESS;
}

#endif /* LAM_WANT_IMPI */
