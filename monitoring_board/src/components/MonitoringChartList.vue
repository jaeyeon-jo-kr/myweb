<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { requestLatestStatus, connectWebSocket, deactivateWebSocket, isConnected, SUBSCRIBE_SYSTEM_LIST_TOPIC, subscriptionMap, SUBSCRIBE_CPU_INFO_TOPIC } from '../websocket/websocket.ts'
import MonitoringChart from './MonitoringChart.vue'
interface SystemStatus {
    hostname:string
    cpuUsage:string
    memoryUsage:string
}
const connectedStatus = ref(false)
const hostNameList = ref<SystemStatus[]>([])

const updateSystemList = (messageBody:string) => {
    console.debug("Receive message : ", messageBody)

    const hostnames = messageBody.split('|')
    hostNameList.value = hostnames.map
        ((hostname) => {
            return {hostname: hostname, cpuUsage:'0', memoryUsage:'0'} as SystemStatus})
}


const updateSystemCharts = (messageBody:string) => {
    const metrics = JSON.parse(messageBody)
    hostNameList.value = hostNameList.value.map((system) => 
        {
            if(system.hostname === metrics.hostname)
            {
                return {
                    hostname:system.hostname,
                    cpuUsage:metrics.cpuUsage,
                    memoryUsage:metrics.memoryUsage
                }
            }else{
                return system
            }
        }
    )
}

onMounted(() => {
    subscriptionMap.set(SUBSCRIBE_SYSTEM_LIST_TOPIC, updateSystemList)
    subscriptionMap.set(SUBSCRIBE_CPU_INFO_TOPIC, updateSystemCharts)
    connectWebSocket(() => {
        connectedStatus.value = isConnected()
        requestLatestStatus()
    }) 
})

onUnmounted(() => {
    deactivateWebSocket()
})

</script>

<template>
    <div>
        <MonitoringChart v-for="({hostname, cpuUsage, memoryUsage}, index) in hostNameList"  
        :key="index" :hostname="hostname" :cpu-usage="cpuUsage" :memory-usage="memoryUsage"/>
    </div>
</template>

<style scoped>
</style>