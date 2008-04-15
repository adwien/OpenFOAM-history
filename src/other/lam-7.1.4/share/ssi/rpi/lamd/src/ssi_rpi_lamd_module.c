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
 * $Id: ssi_rpi_lamd_module.c,v 1.13 2003/07/11 19:48:18 jsquyres Exp $
 *
 */

#include <lam_config.h>
#include <lam-ssi-rpi-lamd-config.h>

#include <rpisys.h>
#include <lam-ssi.h>
#include <lam-ssi-rpi.h>
#include <lam-ssi-rpi-lamd.h>
#include <rpi_lamd.h>


/*
 * Public string showing the lamd module version number
 */
const char *lam_ssi_rpi_lamd_module_version =
  "LAM/MPI SSI rpi lamd module version " LAM_SSI_RPI_LAMD_VERSION;

/*
 * Instantiate the public struct with all of our public information
 * and pointers to our public functions in it
 */

const lam_ssi_rpi_1_0_0_t lam_ssi_rpi_lamd_module = {

  /* First, the lam_ssi_1_0_0_t struct containing meta information
     about the module itself */

  {
    /* SSI API version */

    1, 0, 0,

    /* Module kind name and version */

    "rpi",
    1, 0, 0,

    /* Module name and version */

    "lamd",
    LAM_SSI_RPI_LAMD_MAJOR_VERSION,
    LAM_SSI_RPI_LAMD_MINOR_VERSION,
    LAM_SSI_RPI_LAMD_RELEASE_VERSION,

    /* Module open and close functions */

    lam_ssi_rpi_lamd_open_module,
    NULL
  },

  /* RPI API function pointers */

  lam_ssi_rpi_lamd_query,
  lam_ssi_rpi_lamd_init
};
