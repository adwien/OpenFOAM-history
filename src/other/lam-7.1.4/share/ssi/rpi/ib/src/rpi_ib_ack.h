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
 * $Id: rpi_ib_ack.h,v 1.1 2003/11/11 18:39:48 vsahay Exp $
 */

#ifndef LAM_SSI_RPI_IB_ACK_H
#define LAM_SSI_RPI_IB_ACK_H

#include <mpisys.h>
#include <rpisys.h>


/*
 * public functions
 */

int lam_ssi_rpi_ib_send_ack_done(MPI_Request req, 
				 struct lam_ssi_rpi_ib_envl *env);

void lam_ssi_rpi_ib_ack_send_env_callback(MPI_Request req);

#endif /* LAM_SSI_RPI_IB_ACK_H */
