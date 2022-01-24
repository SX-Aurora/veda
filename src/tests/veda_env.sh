#!/bin/bash

# This Testcase requires 2 VEs in the system
unset VEDA_VISIBLE_DEVICES
unset VE_NODE_NUMBER
unset _VENODELIST

idx=1

check() {
	eval "$1 1>/dev/null"
	test $? -eq $2 && echo -e "\e[32m#$idx passed\e[0m" || echo -e "\e[31m#$idx failed\e[0m"
	idx=$((idx+1))
}

sequence() {
	if [ $1 -eq 0 ] ; then
		check "./veda_cnt" 0
		check "./veda_env -1" 1
		check "./veda_env 0" 1
		check "./veda_env 1" 1
		check "./veda_env 2" 1
	elif [ $1 -eq 1 ] ; then
		check "./veda_cnt" 1
		check "./veda_env -1" 1
		check "./veda_env 0" 0
		check "./veda_env 1" 1
		check "./veda_env 2" 1
	elif [ $1 -eq 2 ] ; then
		check "./veda_cnt" 2
		check "./veda_env -1" 1
		check "./veda_env 0" 0
		check "./veda_env 1" 0
		check "./veda_env 2" 1
	else
		echo "FAILED!"
		exit 1
	fi

}

echo NO ENV
sequence 2

# VE_NODE_NUMBER
echo VE_NODE_NUMBER=0
export VE_NODE_NUMBER=0
sequence 1

echo VE_NODE_NUMBER=1
export VE_NODE_NUMBER=1
sequence 1

unset VE_NODE_NUMBER

# VEDA_VISIBLE_DEVICES
echo VEDA_VISIBLE_DEVICES=
export VEDA_VISIBLE_DEVICES=
sequence 2

echo VEDA_VISIBLE_DEVICES=0
export VEDA_VISIBLE_DEVICES=0
sequence 1

echo VEDA_VISIBLE_DEVICES=1
export VEDA_VISIBLE_DEVICES=1
sequence 1

echo VEDA_VISIBLE_DEVICES=0,1
export VEDA_VISIBLE_DEVICES=0,1
sequence 2

echo VEDA_VISIBLE_DEVICES=2
export VEDA_VISIBLE_DEVICES=2
sequence 0

echo VEDA_VISIBLE_DEVICES=0,1,2,3,4,5
export VEDA_VISIBLE_DEVICES=0,1,2,3,4,5
sequence 2

echo VEDA_VISIBLE_DEVICES=0.0,1.1
export VEDA_VISIBLE_DEVICES=0.0,1.1
sequence 1

unset VEDA_VISIBLE_DEVICES

# _VENODELIST
echo _VENODELIST=
export _VENODELIST=
sequence 0

echo _VENODELIST=0
export _VENODELIST=0
sequence 1

echo _VENODELIST=0 1
export "_VENODELIST=0 1"
sequence 2

echo _VENODELIST=1
export _VENODELIST=1
sequence 1

unset _VENODELIST

# _VENODELIST + VEDA_VISIBLE_DEVICES
echo _VENODELIST=, VEDA_VISIBLE_DEVICES=
export _VENODELIST=
export VEDA_VISIBLE_DEVICES=
sequence 0

echo _VENODELIST=0, VEDA_VISIBLE_DEVICES=0
export _VENODELIST=0
export VEDA_VISIBLE_DEVICES=0
sequence 1

echo _VENODELIST=0, VEDA_VISIBLE_DEVICES=1
export _VENODELIST=0
export VEDA_VISIBLE_DEVICES=1
sequence 0

echo _VENODELIST=1, VEDA_VISIBLE_DEVICES=0
export _VENODELIST=1
export VEDA_VISIBLE_DEVICES=0
sequence 1

echo _VENODELIST=0, VEDA_VISIBLE_DEVICES=1
export _VENODELIST=0
export VEDA_VISIBLE_DEVICES=1
sequence 0

echo _VENODELIST=1, VEDA_VISIBLE_DEVICES=1
export _VENODELIST=1
export VEDA_VISIBLE_DEVICES=1
sequence 0 1 1 1 1


echo _VENODELIST=0 1, VEDA_VISIBLE_DEVICES=0
export "_VENODELIST=0 1"
export VEDA_VISIBLE_DEVICES=0
sequence 1

echo _VENODELIST=0 1, VEDA_VISIBLE_DEVICES=1
export "_VENODELIST=0 1"
export VEDA_VISIBLE_DEVICES=1
sequence 1

echo _VENODELIST=0 1, VEDA_VISIBLE_DEVICES=0,1
export "_VENODELIST=0 1"
export VEDA_VISIBLE_DEVICES=0,1
sequence 2