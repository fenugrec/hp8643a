/* collection of funcs for working with Tektronix TDS7xx ROMs (5xx/6xx probably also)
 * (c) fenugrec 2018
 * GPLv3
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stypes.h"
#include "hplib.h"

uint32_t reconst_32(const uint8_t *buf) {
	// ret 4 bytes at *buf with SH endianness
	// " reconst_4B"
	return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

uint16_t reconst_16(const uint8_t *buf) {
	return buf[0] << 8 | buf[1];
}

void write_32b(uint32_t val, uint8_t *buf) {
	//write 4 bytes at *buf with SH endianness
	// "decomp_4B"
	buf += 3;	//start at end
	*(buf--) = val & 0xFF;
	val >>= 8;
	*(buf--) = val & 0xFF;
	val >>= 8;
	*(buf--) = val & 0xFF;
	val >>= 8;
	*(buf--) = val & 0xFF;

	return;
}


// hax, get file length but restore position
u32 flen(FILE *hf) {
	long siz;
	long orig;

	if (!hf) return 0;
	orig = ftell(hf);
	if (orig < 0) return 0;

	if (fseek(hf, 0, SEEK_END)) return 0;

	siz = ftell(hf);
	if (siz < 0) siz=0;
	if ((long long) siz == (long long) UINT32_MAX) siz = 0;

	if (fseek(hf, orig, SEEK_SET)) return 0;
	return (u32) siz;
}

static u16 sum16(const u8 *buf, unsigned len) {
	len &= ~1;	//16-bit align
	u16 cks = 0;
	for (; len > 0; len -= 2) {
		u16 tmp = reconst_16(buf);
		//printf("tmp %X\n", (unsigned) tmp);
		cks += tmp;
		buf += 2;
	}
	return cks;
}

static void check_checksum(const u8 *rom) {
	u16 cks, expect;
	u16 tmp;

	tmp = sum16(&rom[0], 0x400);
	cks = tmp + sum16(&rom[0x0408], (0x20000 - 0x0408));

	expect = reconst_16(&rom[0x0400]);
	printf("block 0 sum16 : %04X [ ", (unsigned) cks);
	if (cks == expect) {
		printf("OK ]\n");
	} else {
		printf("!!! ]\n");
	}

	cks = sum16(&rom[0x20000], 0x20000);
	expect = reconst_16(&rom[0x0402]);
	printf("block 1 sum16 : %04X [ ", (unsigned) cks);
	if (cks == expect) {
		printf("OK ]\n");
	} else {
		printf("!!! ]\n");
	}
	cks = sum16(&rom[0x40000], 0x20000);
	expect = reconst_16(&rom[0x0404]);
	printf("block 2 sum16 : %04X [ ", (unsigned) cks);
	if (cks == expect) {
		printf("OK ]\n");
	} else {
		printf("!!! ]\n");
	}

	cks = sum16(&rom[0x60000], 0x10000);
	expect = reconst_16(&rom[0x0406]);
	printf("block 3 sum16 : %04X [ ", (unsigned) cks);
	if (cks == expect) {
		printf("OK ]\n");
	} else {
		printf("!!! ]\n");
	}
	return;
}


void parse_rom(FILE *i_file) {
	u32 file_len;

	rewind(i_file);
	file_len = flen(i_file);
	if (file_len != ROMSIZE) {
		printf("wrong ROM size (%lu != 512k)\n", (unsigned long) file_len);
		return;
	}

	u8 *src = malloc(file_len);
	if (!src) {
		printf("malloc choke\n");
		return;
	}

	/* load whole ROM */
	if (fread(src,1,file_len,i_file) != file_len) {
		printf("trouble reading\n");
		free(src);
		return;
	}

	check_checksum(src);
	free(src);
	return;
}
