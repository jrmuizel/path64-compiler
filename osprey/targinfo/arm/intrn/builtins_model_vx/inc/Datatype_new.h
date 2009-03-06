/* Datatype_new.h */

#define GPVR_SIZE 8    /* GPVR size in bytes */

/* Unsigned Byte Packet (GPVR_SIZE unsigned bytes) */
typedef union gpvr {
  uint8  B[GPVR_SIZE];
  uint16 HW[GPVR_SIZE/2];
  uint32 W[GPVR_SIZE/4];
  long long LL;
} GPVR;

/* int128 : pairing 2 64bits registers */
/*
typedef struct int128 {
	long long high;
	long long low;
} INT128;
*/
