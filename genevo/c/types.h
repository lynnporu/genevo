#include <stdint.h>

#define UINT_EVALUATOR(_SIZE) uint ## _SIZE ## _t
#define  INT_EVALUATOR(_SIZE)  int ## _SIZE ## _t

#define UINT(_SIZE) UINT_EVALUATOR(_SIZE)
#define  INT(_SIZE)  INT_EVALUATOR(_SIZE)

/*

All data structures is being dumped into the file using using network byte
order which is big-endian.

*/

#define hton16 htons
#define ntoh16 ntohs
#define hton32 htonl
#define ntoh32 ntohl
#define hton64(x) ((((uint64_t)hton32(x)) << 32) + hton32((x) >> 32))
#define ntoh64(x) ((((uint64_t)ntoh32(x)) << 32) + ntoh32((x) >> 32))


#define HTON_EVALUATOR(_SIZE) hton ## _SIZE
#define NTOH_EVALUATOR(_SIZE) ntoh ## _SIZE

#define HTON(_SIZE) HTON_EVALUATOR(_SIZE)
#define NTOH(_SIZE) NTOH_EVALUATOR(_SIZE)
