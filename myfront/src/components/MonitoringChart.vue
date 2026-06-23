<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { LineChart } from 'vue-chart-3'
import { Chart, registerables } from 'chart.js'
import { type ChartData } from 'chart.js'
import { connectWebSocket, deactivateWebSocket, isConnected } from '../websocket/websocket.ts'
import { memoryUsage } from 'process'

Chart.register(...registerables)

const connectedStatus = ref(false)

// 1. 차트를 구성하는 데이터 구조 선언 ()
const chartData = ref<ChartData<'line'>>({
  labels: [], // x축: 시간 (예: 15:20:01, 15:20:02 ...)
  datasets: [
    {
      label: 'CPU 사용률 (%)',
      backgroundColor: 'rgba(54, 162, 235, 0.1)', // 선 아래 그라데이션 채우기용 색상
      borderColor: 'rgba(54, 162, 235, 1)',     // 메인 꺾은선 색상
      borderWidth: 2,
      pointRadius: 2, // 꺾이는 지점의 점 크기
      data: [],      // y축: 실시간 수치 (0 ~ 100)
      tension: 0.2   // 그래프 선의 부드러운 곡선 정도 (0이면 완전히 꺾은선)
    },
    {
      label: '메모리 사용률 (%)',
      backgroundColor: 'rgba(54, 162, 235, 0.1)', // 선 아래 그라데이션 채우기용 색상
      borderColor: 'rgba(244, 180, 0, 1)',     // 메인 꺾은선 색상
      borderWidth: 2,
      pointRadius: 2, // 꺾이는 지점의 점 크기
      data: [],      // y축: 실시간 수치 (0 ~ 100)
      tension: 0.2   // 그래프 선의 부드러운 곡선 정도 (0이면 완전히 꺾은선)
    }
  ]
})

const updateCpuUsage = (cpuUsage:any) => {
    if (!chartData.value.datasets[0]?.data) 
        return
    chartData.value.datasets[0].data.push(parseInt(cpuUsage))
    console.debug('datasets:', cpuUsage)

    if(!chartData.value.labels?.length)
        return
    if (chartData.value.labels.length > 20) {
        chartData.value.labels.shift()
        chartData.value.datasets[0].data.shift()
    }
}

const updateMemoryUsage = (memoryUsage:any) => {
    // y축 데이터 배열에 실시간 CPU 값 추가
    if (!chartData.value.datasets[1]?.data) 
        return
    chartData.value.datasets[1].data.push(parseInt(memoryUsage))

    // 🚨 패킷 다이어트 및 메모리 관리: 
    // 데이터가 무한히 쌓이면 브라우저가 멈추므로, 최근 20개 지표만 화면에 유지하고 오래된 것은 밀어냅니다(shift).
    if(!chartData.value.labels?.length)
        return
    if (chartData.value.labels.length > 20) {
        chartData.value.labels.shift()
        chartData.value.datasets[1].data.shift()
    }
}

const updateChart = (messageBody:string) => {
    const now = new Date()
    const timeStr:string = `${String(now.getHours()).padStart(2, '0')}:${String(now.getMinutes()).padStart(2, '0')}:${String(now.getSeconds()).padStart(2, '0')}`

    const metrics = JSON.parse(messageBody)
    console.debug('receive metrics' + messageBody)

    if (!chartData.value.labels) 
        chartData.value.labels = []
    chartData.value.labels.push(timeStr)

    updateCpuUsage(metrics.cpuUsage)
    updateMemoryUsage(metrics.memoryUsage)
}

const chartOptions = {
  responsive: true,
  maintainAspectRatio: false,
  scales: {
    y: {
      min: 0,
      max: 100, // CPU는 항상 0~100% 사이이므로 y축 범위를 고정합니다.
      ticks: {
        callback: (value:string) => value + '%' // y축 수치 뒤에 % 붙이기
      }
    }
  },
  plugins: {
    legend: {
      display: true,
      position: 'top'
    }
  },
  animation: {
    duration: 1000 // 1초마다 데이터가 바뀌므로 애니메이션 속도를 0.3초로 제한하여 부드럽게 연출
  }
}

// 라이프사이클 관리
onMounted(() => {
    // Chart.js에 필요한 내부 엔진(컴포넌트들)을 등록합니다.
    connectWebSocket(updateChart) 
    setTimeout(() => {connectedStatus.value = isConnected()}, 1000 )
})

onUnmounted(() => {
  deactivateWebSocket()
})
</script>

<template>
  <div class="chart-container">
    <div class="chart-header">
      <h2>🖥️ 실시간 CPU 사용률 관제</h2>
      <span class="status-badge" :class="{ online: connectedStatus }">
        {{ connectedStatus ? '● LIVE' : '○ DISCONNECTED' }}
      </span>
    </div>

    <div class="chart-wrapper">
      <LineChart :chartData="chartData" :options="chartOptions" />
    </div>
  </div>
</template>

<style scoped>
/* 대시보드 카드 테두리 및 섀도우 기본 인테리어 (정적) */
.chart-container {
  background: #ffffff;
  border: 1px solid #e2e8f0;
  border-radius: 12px;
  padding: 24px;
  box-shadow: 0 4px 6px -1px rgba(0, 0, 0, 0.1);
  max-width: 900px;
  margin: 20px auto;
}

.chart-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

h2 {
  font-size: 1.25rem;
  color: #1e293b;
  margin: 0;
}

/* 실시간 상태 불빛 디자인 (동적 바인딩 대응) */
.status-badge {
  font-size: 0.85rem;
  font-weight: bold;
  padding: 4px 10px;
  border-radius: 20px;
  background: #fee2e2;
  color: #ef4444;
  transition: all 0.3s ease;
}

/* 연결 완료 시 실시간으로 바뀔 클래스 모양 */
.status-badge.online {
  background: #dcfce7;
  color: #22c55e;
}

/* 차트 캔버스가 늘어날 운동장 크기 지정 */
.chart-wrapper {
  position: relative;
  height: 350px;
  width: 100%;
}
</style>