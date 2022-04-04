#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

int Get_VE_Node_Count()
{
	int available_node=0;
        FILE *fp = popen("./veda_utility.sh ve_get_online_node", "r");
        fscanf(fp,"%d" , &available_node);
        fclose(fp);
        return available_node;
}
int Get_VE_Node_Memory(int physical_node_id)
{
	int node_memory=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_memory %d", physical_node_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &node_memory);
        fclose(fp);
        return node_memory;
}
float Get_VE_Node_Current(int a_id, int physical_node_id)
{
	float node_current=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_current %d %d", a_id, physical_node_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%f" , &node_current);
        fclose(fp);
        return node_current/1000.0f;
}
float Get_VE_Node_Voltage(int physical_node_id)
{
	float node_voltage=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_voltage %d", physical_node_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%f" , &node_voltage);
        fclose(fp);
        return node_voltage/1000000.0f;
}
float Get_VE_Node_Current_Edge(int a_id, int physical_node_id)
{
	float node_current=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_current_edge %d %d", a_id, physical_node_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%f" , &node_current);
        fclose(fp);
        return node_current/1000.0f;
}
float Get_VE_Node_Voltage_Edge(int physical_node_id)
{
	float node_voltage=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_voltage_edge %d", physical_node_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%f" , &node_voltage);
        fclose(fp);
        return node_voltage/1000000.0f;
}


int Get_VEDA_device_core(int aveo_id, int numa_id)
{
	int number_of_cores=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_core %d %d", aveo_id, numa_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &number_of_cores);
        fclose(fp);
        return number_of_cores;
}
int Get_VEDA_device_base_clock(int aveo_id)
{
	int freq=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_base_clock %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &freq);
        fclose(fp);
        return freq;
}
int Get_VEDA_device_chip_clock(int aveo_id)
{
	int freq=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_chip_clock %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &freq);
        fclose(fp);
        return freq;
}
int Get_VEDA_device_memory_clock(int aveo_id)
{
	int freq=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_memory_clock %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &freq);
        fclose(fp);
        return freq;
}
float Get_VEDA_device_core_temp(int aveo_id, int numa_id, int core_id, int physical_id)
{
	int core_temp=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_core_temp %d %d %d %d", aveo_id, numa_id, core_id, physical_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &core_temp);
        fclose(fp);
        return core_temp/1000000.0f;
}
int Get_VEDA_device_l1d_cache(int aveo_id)
{
	int cache=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_l1d_cache %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &cache);
        fclose(fp);
        return cache;
}
int Get_VEDA_device_l1i_cache(int aveo_id)
{
	int cache=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_l1i_cache %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &cache);
        fclose(fp);
        return cache;
}
int Get_VEDA_device_l2_cache(int aveo_id)
{
	int cache=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_l2_cache %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &cache);
        fclose(fp);
        return cache;
}
int Get_VEDA_device_llc_cache(int aveo_id, int physical_id)
{
	int cache=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_llc_cache %d %d", aveo_id, physical_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &cache);
        fclose(fp);
        return cache;
}
int Get_VEDA_device_model(int aveo_id)
{
	int model=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_model %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &model);
        fclose(fp);
        return model;
}
int Get_VEDA_device_abi_version(int aveo_id)
{
	int model=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_abi_version %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &model);
        fclose(fp);
        return model;
}
int Get_VEDA_device_fw_version(int aveo_id)
{
	int model=0;
	char cmd[256];
	sprintf(cmd, "./veda_utility.sh ve_get_node_fw_version %d", aveo_id);
        FILE *fp = popen(cmd, "r");
        fscanf(fp,"%d" , &model);
        fclose(fp);
        return model;
}
