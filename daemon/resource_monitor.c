#include <sys/types.h>
#include <sys/sysctl.h>
#include <stdlib.h>
#include <stdio.h>

#include "resource_monitor.h"

bool try_get_cpu_brand(cpu_brand_type* cpu_brand)
{
    if (sysctlbyname("machdep.cpu.brand_string", NULL, &cpu_brand->size, NULL, 0) != 0) {
        perror("CPU 모델명 수집 실패");
        return false;
    }

    cpu_brand->name = (char *)malloc(sizeof(char) * cpu_brand->size);
    if(!cpu_brand->name)
    {
        perror("cpu brand name 문자열 할당 실패");
        return false;
    }

    if (sysctlbyname("machdep.cpu.brand_string", cpu_brand->name, &cpu_brand->size, NULL, 0) != 0) {
        perror("CPU 모델명 수집 실패");
        return false;
    }
    return true;
}

bool try_get_core_num(int32_t *num_cores)
{
    size_t num_cores_size = sizeof(*num_cores);
    if (sysctlbyname("hw.ncpu", num_cores, &num_cores_size, NULL, 0) != 0) {
        perror("CPU 코어 수 수집 실패");
        return false;
    }
    return true;
}

bool try_get_cpu_info(cpu_info_type *cpu_info)
{
    if(!try_get_cpu_brand(&cpu_info->cpu_brand))
    {
        perror("CPU 모델명 수집 실패");
        return false;
    }

    if(!try_get_core_num(&cpu_info->core))
    {
        perror("CPU 코어 수 수집 실패");
        return false;
    }
    return true;
}

bool release_cpu_brand(cpu_brand_type *cpu_brand)
{
    if(cpu_brand->name)
    {
        free(cpu_brand->name);
        return true;
    }
    return false;
}

bool release_cpu_info(cpu_info_type *cpu_info)
{
    if(!release_cpu_brand(&cpu_info->cpu_brand))
    {
        return false;
    }
    return true;
}

void print_cpu_info()
{
   cpu_info_type cpu_info;
   if(!try_get_cpu_info(&cpu_info))
   {
        perror("cpu info error\n");
        return;
   }
   printf("model:%s, core:%d\n", cpu_info.cpu_brand.name, cpu_info.core);
}

bool try_convert_to_server_str(cpu_info_type *cpu_info, char **out_string, size_t* out_len)
{
    *out_len = snprintf(NULL, 0, "CPU_INFO|%s|%d\n", cpu_info->cpu_brand.name, cpu_info->core);

    if(out_len == 0)
    {
        perror("Getting String Length Error\n");
        return false;
    }
    ++(*out_len);
    *out_string = (char *)malloc(sizeof(char)*(*out_len));

    if(!*out_string)
    {
        perror("Getting String memory Error\n");
        return false;
    }

    snprintf(*out_string, *out_len, "CPU_INFO|%s|%d\n", cpu_info->cpu_brand.name, cpu_info->core);

    return true;
}