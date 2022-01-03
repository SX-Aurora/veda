#! /bin/bash

ve_get_online_node(){
	nodes=0
	nodes=`(/opt/nec/ve/bin/venumainfo | grep available | tr -s ' ' | awk '{sum+=$2}END {print sum}')` >& /dev/null
	echo $nodes
	return $nodes
}

ve_get_node_memory(){
	file_name="/sys/class/ve/ve$1/memory_size"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_l1d_cache(){
	file_name="/sys/class/ve/ve$1/cache_l1d"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_l1i_cache(){
	file_name="/sys/class/ve/ve$1/cache_l1i"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_model(){
	file_name="/sys/class/ve/ve$1/model"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_abi_version(){
	file_name="/sys/class/ve/ve$1/abi_version"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_fw_version(){
	file_name="/sys/class/ve/ve$1/fw_version"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_l2_cache(){
	file_name="/sys/class/ve/ve$1/cache_l2"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}

ve_get_node_llc_cache(){
	file_name="/sys/class/ve/ve$2/cache_llc"
	while read line
	do
		line1=$line
	done < $file_name
	is_numa=`(/opt/nec/ve/bin/venumainfo | grep -A1 "VE Node: $1" | tr -s ' ' | awk '{sum+=$2}END {print sum}')` >& /dev/null
	if [ $is_numa -eq 2 ]
	then
		line1=$(($line1 / 2))
	fi
	echo $line1
	return $line1
}

ve_get_node_current(){
	file_name="/sys/class/ve/ve$2/sensor_12"
	while read line
	do
		line1=$line
	done < $file_name

	is_numa=`(/opt/nec/ve/bin/venumainfo | grep -A1 "VE Node: $1" | tr -s ' ' | awk '{sum+=$2}END {print sum}')` >& /dev/null
	if [ $is_numa -eq 2 ]
	then
		line1=$(($line1 / 2))
	fi

	echo $line1
	return $line1
}
ve_get_node_current_edge(){
	file_name="/sys/class/ve/ve$2/sensor_13"
	while read line
	do
		line1=$line
	done < $file_name

	is_numa=`(/opt/nec/ve/bin/venumainfo | grep -A1 "VE Node: $1" | tr -s ' ' | awk '{sum+=$2}END {print sum}')` >& /dev/null
	if [ $is_numa -eq 2 ]
	then
		line1=$(($line1 / 2))
	fi

	echo $line1
	return $line1
}
ve_get_node_voltage(){
	file_name="/sys/class/ve/ve$1/sensor_8"
	while read line
	do
		line1=$line
	done < $file_name

	echo $line1
	return $line1
}
ve_get_node_voltage_edge(){
	file_name="/sys/class/ve/ve$1/sensor_9"
	while read line
	do
		line2=$line
	done < $file_name
	echo $line2

	return $line2
}
ve_get_node_base_clock(){
	file_name="/sys/class/ve/ve$1/clock_base"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}
ve_get_node_chip_clock(){
	file_name="/sys/class/ve/ve$1/clock_chip"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}
ve_get_node_memory_clock(){
	file_name="/sys/class/ve/ve$1/clock_memory"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}
ve_get_node_core(){
	start_core=`(/opt/nec/ve/bin/lscpu | grep -A24 "VE Node: $1" | grep "NUMA node$2 CPU(s)" | tr -s ' w' | cut -d':' -f2 | cut -d'-' -f1)` >& /dev/null
	end_core=`(/opt/nec/ve/bin/lscpu | grep -A24 "VE Node: $1" | grep "NUMA node$2 CPU(s)" | tr -s ' w' | cut -d':' -f2 | cut -d'-' -f2)` >& /dev/null
	core=$((${end_core} - ${start_core} + 1))
	echo $core
}
ve_get_pyhsical_core(){
	is_numa=`(/opt/nec/ve/bin/venumainfo | grep -A1 "VE Node: $1" | tr -s ' ' | awk '{sum+=$2}END {print sum}')` >& /dev/null
	if [ $is_numa -eq 2 ]
	then
		file_name="/sys/class/ve/ve$4/numa$2_cores"
	else
		file_name="/sys/class/ve/ve$4/cores_enable"
	fi
	while read line
	do
		line1=$line
	done < $file_name
	bit=1
	logical_core=$((${3} + 1))
	core_num=0
	for i in {0..32};
	do
		if [ $i -gt 31 ]
		then
			break;
		fi	
		active=$(($line1 & $bit))
		if [ $active -gt 0 ]
		then
			core_num=$((${core_num} + 1))	
			if [ $core_num -eq $logical_core ]
			then
				break;
			fi
		fi
		bit=$(($bit << 1))
	done
	return $i
}
ve_get_node_core_temp(){
	ve_get_pyhsical_core $1 $2 $3 $4
	physical_core=$?
	if [ $physical_core -gt 32 ]
	then
		return 0
	fi
	sensor="sensor_$((${physical_core}+ 14))"
	file_name="/sys/class/ve/ve$4/$sensor"
	while read line
	do
		echo $line
	done < $file_name
	return $line
}
"$@"
