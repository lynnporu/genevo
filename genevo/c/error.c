#include "error.h"

#include <stdio.h>

uint8_t ERROR_LEVEL = 0;

const char * get_err_string(const err_status_t errcode) {

	switch (errcode) {
		case ERR_OK:
			return ERR_OK_STR;
			break;
		case ERR_CANNOT_MALLOC:
			return ERR_CANNOT_MALLOC_STR;
			break;
		case ERR_OUT_OF_BOUNDS:
			return ERR_OUT_OF_BOUNDS_STR;
		case ERR_FILE_CANNOT_OPEN:
			return ERR_FILE_CANNOT_OPEN_STR;
			break;
		case ERR_FILE_CANNOT_FSTAT:
			return ERR_FILE_CANNOT_FSTAT_STR;
			break;
		case ERR_FILE_CANNOT_MMAP:
			return ERR_FILE_CANNOT_MMAP_STR;
			break;
		case ERR_FILE_CANNOT_LSEEK:
			return ERR_FILE_CANNOT_LSEEK_STR;
			break;
		case ERR_FILE_CANNOT_WRITE:
			return ERR_FILE_CANNOT_WRITE_STR;
			break;
		case ERR_POOL_CORRUPT_TOO_SMALL:
			return ERR_POOL_CORRUPT_TOO_SMALL_STR;
			break;
		case ERR_POOL_CORRUPT_INITIAL:
			return ERR_POOL_CORRUPT_INITIAL_STR;
			break;
		case ERR_POOL_CORRUPT_METADATA_START:
			return ERR_POOL_CORRUPT_METADATA_START_STR;
			break;
		case ERR_POOL_CORRUPT_METADATA_END:
			return ERR_POOL_CORRUPT_METADATA_END_STR;
			break;
		case ERR_POOL_CORRUPT_END:
			return ERR_POOL_CORRUPT_END_STR;
			break;
		case ERR_GENM_CORRUPT_METADATA_START:
			return ERR_GENM_CORRUPT_METADATA_START_STR;
			break;
		case ERR_GENM_CORRUPT_METADATA_END:
			return ERR_GENM_CORRUPT_METADATA_END_STR;
			break;
		case ERR_GENM_CORRUPT_START:
			return ERR_GENM_CORRUPT_START_STR;
			break;
		case ERR_GENM_CORRUPT_END:
			return ERR_GENM_CORRUPT_END_STR;
			break;
		case ERR_GENM_CORRUPT_RESIDUE:
			return ERR_GENM_CORRUPT_RESIDUE_STR;
			break;
		case ERR_GENM_END_ITERATION:
			return ERR_GENM_END_ITERATION_STR;
			break;
		case ERR_GENE_NOT_ALIGNED:
			return ERR_GENE_NOT_ALIGNED_STR;
			break;
		case ERR_GENE_OGSB_TOO_LARGE:
			return ERR_GENE_OGSB_TOO_LARGE_STR;
			break;
		case ERR_GENE_WEIGHT_TOO_LARGE:
			return ERR_GENE_WEIGHT_TOO_LARGE_STR;
			break;
		default:
			return ERR_OK_STR;
			break;
	}
}
