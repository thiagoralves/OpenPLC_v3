/*
 * (c) 2000 Jiri Baum
 *          Mario de Sousa
 *
 * Offered to the public under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */


#ifndef __PLC_H
#define __PLC_H

#include <stdio.h>  /* required for declaration of FILE */

#ifdef __cplusplus
extern "C" {
#endif


#include "types.h"
#include "log/log.h"
#include "conffile/conffile.h"
#include "cmm/cmm.h"
#include "gmm/gmm.h"
#include "synch/synch.h"
#include "period/period.h"
#include "state/state.h"
#include "rt/rt.h"


#define PLC_PT_NAME_MAX_LEN CMM_NAME_MAX_LEN

/*** GENERAL ***/
int plc_init(char const *module_name, int argc, char **argv);
int plc_done(void);

/*** SYNCHING ***/
int plc_scan_beg(void);
int plc_scan_end(void);

/* memory must be de-allocated with free() */
const char *plc_module_name(void);

/*
 * UTIL functions;
 */
int plc_print_usage(FILE *output);
/* prints the command line arguments the plc_init() function accepts */

/* not to be used by general plc modules. This function is used by the
 * plc_shutdown() function.
 */
int plc_init_try(char const *module_name, int argc, char **argv);


/* plc command line options leader */
#define CLO_LEADER "--PLC"

/* plc command line options */
#define CLO_plc_id             CLO_LEADER "plc_id="
#define CLO_loc_local          CLO_LEADER "local"
#define CLO_loc_isolate        CLO_LEADER "isolate"
#define CLO_loc_shared         CLO_LEADER "shared"
#define CLO_privmap_key        CLO_LEADER "local_map_key="
#define CLO_log_level          CLO_LEADER "debug="
#define CLO_log_file           CLO_LEADER "logfile="
#define CLO_config_file        CLO_LEADER "conf="
#define CLO_module_name        CLO_LEADER "module="
#define CLO_force_init         CLO_LEADER "force-init"


#ifdef __cplusplus
}
#endif

#endif /* __PLC_H */
