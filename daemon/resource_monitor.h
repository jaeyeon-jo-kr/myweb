#ifndef __RESOURCE_MONITOR__
#define __RESOURCE_MONITOR__
// cpu
struct cpu_brand_type {
    char *name;
    size_t size;
};

typedef struct cpu_brand_type cpu_brand_type;

struct cpu_info_type
{
    cpu_brand_type cpu_brand;
    int32_t core;
};

typedef struct cpu_info_type cpu_info_type;

bool try_get_cpu_info(cpu_info_type *cpu_info);
bool release_cpu_info(cpu_info_type *cpu_info);
bool try_convert_to_server_str(cpu_info_type *cpu_info, char **out_string, size_t* out_len);
void print_cpu_info();
#endif