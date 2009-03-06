#define DATA_VALUE_MASK(dataWidth) (~((uint64)0) >> (64 - (dataWidth)))
#define CONSTANT_64(data) ((uint64)data)

#define setBit(objSize, value, pos, val) (((val) & 0x1) ? bitSet(objSize, (value), pos) : bitReset(objSize, (value), pos))
#define getBit(cast, Object, i) ( (cast)(((Object) >> (i)) & 0x1) )

#define bitSet(resSize, res, pos) ( (resSize)(((uint64)1 << (pos)) | res) )
#define bitReset(resSize, res, pos) ( (resSize)((~((uint64)1 << (pos))) & res) )

#define getBitRange(objSize, value, msb, lsb) ((objSize)(((value) >> lsb) & DATA_VALUE_MASK(msb-lsb+1)))

#define SignEx(resSize, valSize, value) ( (((value) >> (valSize-1)) & 0x1) ? \
(sint##resSize) ((CONSTANT_64(0xFFFFFFFFFFFFFFFF) << (valSize))|(value)) : (sint##resSize) \
((~(CONSTANT_64(0xFFFFFFFFFFFFFFFF) << (valSize)))&(value)) )

#define ZeroEx(resSize, valSize, value) ( (uint##resSize) \
((~(CONSTANT_64(0xFFFFFFFFFFFFFFFF) << (valSize))) & (value)) )
