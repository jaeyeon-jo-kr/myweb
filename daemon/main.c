#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdio.h>
#include "resource_monitor.h"
#include "socket_client.h"

int main(int argc, char const *argv[])
{
    print_cpu_info();
    run_client();
    return 0;
}