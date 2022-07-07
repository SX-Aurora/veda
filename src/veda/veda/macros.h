#pragma once

#ifdef __cplusplus
#define CVEDA(ERR)	if(VEDAresult __VEDA_ERROR__ = (ERR)) return __VEDA_ERROR__
#else
#define CVEDA(ERR)	{ VEDAresult __VEDA_ERROR__ = (ERR); if(__VEDA_ERROR__ != VEDA_SUCCESS) { return __VEDA_ERROR__; }}
#endif

#define VEDA_STR(s) VEDA_STR_(s)
#define VEDA_STR_(s) #s

/** VPTR Format
 * 5Bit:  Device (32 devices max)
 * 22Bit: Idx    (4,194,304 indicies)
 * 37Bit: Offset (128GB)
 */

#define VEDA_BITS_DEVICE		0xF800000000000000ull
#define VEDA_BITS_IDX			0x07FFFFE000000000ull
#define VEDA_BITS_OFFSET		0x0000001FFFFFFFFFull
#define VEDA_SHIFT_DEVICE		59
#define VEDA_SHIFT_IDX			37
#define VEDA_SHIFT_OFFSET		0
#define VEDA_CNT_IDX			0x3FFFFF
#define VEDA_MAX_DEVICES		16

#define VEDA_GET_DEVICE(vptr)			(VEDAdevice)	(((uint64_t)vptr & VEDA_BITS_DEVICE) >> VEDA_SHIFT_DEVICE)
#define VEDA_GET_IDX(vptr)			(VEDAidx)	(((uint64_t)vptr & VEDA_BITS_IDX   ) >> VEDA_SHIFT_IDX   )
#define VEDA_GET_OFFSET(vptr)			(VEDAoffset)	(((uint64_t)vptr & VEDA_BITS_OFFSET) >> VEDA_SHIFT_OFFSET)

#define VEDA_SET_DEVICE(vptr, device)		(VEDAdeviceptr)((uint64_t)vptr | (((uint64_t)device << VEDA_SHIFT_DEVICE))	& VEDA_BITS_DEVICE)
#define VEDA_SET_IDX(vptr, idx)			(VEDAdeviceptr)((uint64_t)vptr | (((uint64_t)idx    << VEDA_SHIFT_IDX))		& VEDA_BITS_IDX   )
#define VEDA_SET_PTR(device, idx, offset)	VEDA_SET_IDX(VEDA_SET_DEVICE(offset & VEDA_BITS_OFFSET, device), idx)

#define VEDA_DEPRECATED(func) func __attribute__ ((deprecated))
