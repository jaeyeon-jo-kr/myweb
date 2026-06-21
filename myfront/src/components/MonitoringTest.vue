<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { Client } from '@stomp/stompjs'
const isConnected = ref(false)
const metrics = ref(null)
// Spring Boot 웹소켓 설정에서 정한 엔드포인트와 구독 주소에 맞게 세팅합니다.
// 예시: 엔드포인트가 "/ws-monitoring", 구독 토픽이 "/topic/metrics" 일 때
const WEBSOCKET_URL = 'ws://localhost:8080/ws-monitoring' 
const SUBSCRIBE_TOPIC = '/topic/cpu_info'
const stompClient = new Client({
    brokerURL: WEBSOCKET_URL,
    debug: function (str) {
      console.log('[STOMP 디버그]: ' + str) // 통신 오가는 로우 로그 확인용
    },
    reconnectDelay: 5000, // 연결 끊기면 5초마다 자동 재접속 시도
    heartbeatIncoming: 4000,
    heartbeatOutgoing: 4000
  })
const connectWebSocket = () => {

  // 웹소켓 연결 성공 시 콜백
  stompClient.onConnect = () => {
    isConnected.value = true
    console.log('✅ Spring 웹소켓 브로커 연결 성공!')

    // Java 서버가 푸시해주는 토픽 주소를 구독(Subscribe)
    stompClient.subscribe(SUBSCRIBE_TOPIC, (message) => {
      if (message.body) {
        console.log('📩 실시간 데이터 수신:', message.body)
        
        // Java에서 JSON 형태로 쐈을 테니 파싱해서 가변 변수에 매핑
        metrics.value = JSON.parse(message.body)
      }
    })
  }

  // 웹소켓 연결 종료 또는 에러 시 콜백
  stompClient.onDisconnect = () => {
    isConnected.value = false
    console.log('❌ 웹소켓 연결 종료')
  }

  stompClient.activate() // 웹소켓 활성화
}

onMounted(() => {
  connectWebSocket()
})

onUnmounted(() => {
  if (stompClient) {
    stompClient.deactivate() // 컴포넌트가 꺼질 때 안전하게 커넥션 반납
  }
})
</script>


<template>
  <div style="padding: 20px; font-family: sans-serif;">
    <h2>📊 실시간 에이전트 모니터링 테스트</h2>
    
    <div style="margin-bottom: 20px;">
      상태: 
      <span :style="{ color: isConnected ? 'green' : 'red', fontWeight: 'bold' }">
        {{ isConnected ? '● 연결됨' : '○ 연결 끊김' }}
      </span>
    </div>

    <div v-if="metrics" style="background: #f4f4f4; padding: 15px; border-radius: 5px;">
      <h3>🖥️ 호스트명: {{ metrics.hostname }}</h3>
      <p>📈 CPU 사용률: <strong style="color: blue;">{{ metrics.cpuUsage }} %</strong></p>
      <p>💾 메모리 사용률: <strong style="color: purple;">{{ metrics.memoryUsage }} %</strong></p>
    </div>
    <div v-else style="color: gray;">
      Go 에이전트가 패킷을 던질 때까지 기다리는 중...
    </div>
  </div>
</template>

<style scoped>
</style>