package main

import "fmt"

func main() {
	currentCpuInfo, err := GetCpuInfo()
	if err == nil {
		fmt.Printf("current Cpu Info :%s, core:%d\n", currentCpuInfo.brand, currentCpuInfo.core)
	} else {
		fmt.Printf("error : %s\n", err)
	}
	fmt.Printf("%s\n", GetCpuStatus())
	RunClient()
}
