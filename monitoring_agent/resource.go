package main

import (
	"fmt"
	"os"

	"github.com/shirou/gopsutil/cpu"
	"github.com/shirou/gopsutil/mem"
	"golang.org/x/sys/unix"
)

type CpuInfo struct {
	brand string
	core  uint8
}

type CpuStatus struct {
}

func GetCpuInfo() (CpuInfo, error) {
	info := CpuInfo{brand: "", core: 0}

	brand, err := unix.Sysctl("machdep.cpu.brand_string")
	info.brand = brand

	if err == nil {
		fmt.Printf("cpu brand name : %s\n", brand)
	} else {
		fmt.Printf("cpu brand name getting error: %s\n", err)
		return info, err
	}

	core, err := unix.SysctlUint32("hw.logicalcpu")
	if err == nil {
		info.core = uint8(core)
	} else {
		fmt.Printf("cpu core parsing error: %s\n", err)
		return info, err
	}
	return info, nil
}

func CpuInfoToStr(info CpuInfo) string {
	strInfo := ""
	strInfo = fmt.Sprintf("CPU_INFO|%s|%d\n", info.brand, info.core)
	return strInfo
}

func GetCpuStatus() string {
	virtualMemory, err := mem.VirtualMemory()
	if err != nil {
		return ""
	}
	return virtualMemory.String()
}

func GetResourceStatus() (string, error) {
	virtualMemory, err := mem.VirtualMemory()
	if err != nil {
		fmt.Errorf("get virtual memory error err : %s \n", err)
		return "", err
	}

	usedPercentMemory := virtualMemory.UsedPercent

	percents, err := cpu.Percent(0, false)
	if err != nil {
		fmt.Errorf("get cpu Percentage error err : %s \n", err)
		return "", err
	}

	cpuCount := len(percents)
	fmt.Printf("cpu Count : %d\n", cpuCount)
	totalCpu := 0.0
	for i := 0; i < len(percents); i++ {
		totalCpu += percents[i]
	}
	averageCpu := totalCpu / float64(cpuCount)
	hostName, err := os.Hostname()
	if err != nil {
		fmt.Errorf("get cpu HostName error err : %s \n", err)
		return "", err
	}
	message := fmt.Sprintf("DATA|%s|%f|%f\n", hostName, averageCpu, usedPercentMemory)

	fmt.Printf("%s\n", message)
	return message, nil
}
