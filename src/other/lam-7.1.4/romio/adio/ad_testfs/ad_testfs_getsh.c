/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* 
 *   $Id: ad_testfs_getsh.c,v 1.1 2003/03/25 22:40:27 nsanghvi Exp $    
 *
 *   Copyright (C) 2001 University of Chicago. 
 *   See COPYRIGHT notice in top-level directory.
 */

#include "ad_testfs.h"
#include "adioi.h"

void ADIOI_TESTFS_Get_shared_fp(ADIO_File fd, int size, 
				ADIO_Offset *shared_fp, 
				int *error_code)
{
    int myrank, nprocs;

    *error_code = MPI_SUCCESS;

    MPI_Comm_size(fd->comm, &nprocs);
    MPI_Comm_rank(fd->comm, &myrank);
    FPRINTF(stdout, "[%d/%d] ADIOI_TESTFS_Get_shared_fp called on %s\n", 
	    myrank, nprocs, fd->filename);
}
