/* collection of funcs for working with HP 8643A ROMs
 * (c) fenugrec 2021
 * GPLv3
 */

#include <assert.h>
#include <stdio.h>	//just for FILE
#include <stdint.h>
#include "stypes.h"

#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

#define ROMSIZE 512*1024UL	//changing this will break everything

/********** useful funcs ********/

/** parse & analyze ROM */
void parse_rom(FILE *i_file);

/** get file length but restore position */
uint32_t flen(FILE *hf);

/** Read uint32 at *buf bigendian
*/
uint32_t reconst_32(const uint8_t *buf);

/** Read uint16 at *buf bigendian
*/
uint16_t reconst_16(const uint8_t *buf);
