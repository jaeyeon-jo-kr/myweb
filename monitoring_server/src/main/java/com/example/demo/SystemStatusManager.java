package com.example.demo;

import java.util.Enumeration;
import java.util.concurrent.ConcurrentHashMap;

import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Component;

@Component
public class SystemStatusManager {

    private ConcurrentHashMap<String, DeviceStatus> deviceStatusMap;

    SystemStatusManager(){
       this.deviceStatusMap  = new ConcurrentHashMap<String, DeviceStatus>();
    }
    
    public synchronized DeviceStatus updateStatus(String hostname, double cpuUsage, double memoryUsage) {
        DeviceStatus deviceStatus = new DeviceStatus(hostname,cpuUsage,memoryUsage);
        deviceStatusMap.put(hostname, deviceStatus);
        return deviceStatus;
    }

    public synchronized String getSystemList(){
        StringBuilder stringBuilder = new StringBuilder();
        Enumeration<String> hostnames = deviceStatusMap.keys();
        
        hostnames.asIterator().forEachRemaining
            (hostname -> stringBuilder.append(hostname).append('|'));
        stringBuilder.deleteCharAt(stringBuilder.length() - 1);
        System.err.println("Get System List:" + stringBuilder.toString());
        return stringBuilder.toString();
    }

}