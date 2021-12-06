#include "error.h"

static inline const char * get_err_string(const uint8_t errcode) {

	switch (errcode) {
		case ERR_OK:
			return ERR_OK_STR;
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
		case ERR_GENE_NOT_ALIGNED:
			return ERR_GENE_NOT_ALIGNED_STR;
			break;
		default:
			return ERR_OK_STR;
			break;
	}
}
