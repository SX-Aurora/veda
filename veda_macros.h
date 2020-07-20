#pragma once

#define CVEDA(ERR)	{ VEDAresult err = ERR; if(err != VEDA_SUCCESS) { return err; }}
