package main

import (
	"fmt"
	"net"
	"os"
	"os/signal"
	"syscall"
	"time"
)

const connectAddr = "127.0.0.1:4567"

func RunClient() {

	// 1. 종료 시그널 설정 (Ctrl+C 처리를 우아하게)
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, syscall.SIGINT, syscall.SIGTERM)

	conn, err := net.Dial("tcp", connectAddr)

	if err != nil {
		fmt.Printf("error %s\n", err)
		return
	}

	defer conn.Close()

	ticker := time.NewTicker(5 * time.Second)
	defer ticker.Stop()
	cpuInfo, err := GetCpuInfo()
	if err != nil {
		fmt.Printf("Cpu Info Getting error %s\n", err)
		return
	}
	cpuInfoMsg := CpuInfoToStr(cpuInfo)
	n, err := conn.Write([]byte(cpuInfoMsg))

	if err != nil {
		fmt.Printf("Cpu Info Sending error %s\n", err)
		return
	}
	fmt.Printf("Size of sended packet : %d.\n", n)

	for {
		select {
		case <-ticker.C:
			fmt.Printf("Receive Tick\n")
			resourceStatus, err := GetResourceStatus()
			if err != nil {
				fmt.Printf("GetResourceStatus error %s\n", err)
				break
			}
			n, err := conn.Write([]byte(resourceStatus))
			if err != nil {
				fmt.Printf("Write resource status error %s\n", err)
				break
			}
			fmt.Printf("sended packet len : %d\n", n)
		case sig := <-sigChan:
			fmt.Printf("Sig interuptted : %v\n", sig)
			return
		}
	}

}
