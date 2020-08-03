#pragma once

#ifdef __cplusplus
#define CVEDA(ERR)	while(VEDAresult __VEDA_ERROR__ = ERR) return __VEDA_ERROR__
#else
#define CVEDA(ERR)	{ VEDAresult __VEDA_ERROR__ = ERR; if(__VEDA_ERROR__ != VEDA_SUCCESS) { return __VEDA_ERROR__; }}
#endif
