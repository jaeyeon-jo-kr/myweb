import { Client, type IMessage } from '@stomp/stompjs'

export const WEBSOCKET_URL = 'ws://localhost:8080/ws-monitoring' 
export const SUBSCRIBE_CPU_INFO_TOPIC = '/topic/cpu_info'
export const SUBSCRIBE_SYSTEM_LIST_TOPIC = '/topic/system_list'

const client = new Client({
        brokerURL: WEBSOCKET_URL,
        debug: function (str) {
            console.log('[STOMP 디버그]: ' + str) // 통신 오가는 로우 로그 확인용
        },
        reconnectDelay: 5000, // 연결 끊기면 5초마다 자동 재접속 시도
        heartbeatIncoming: 4000,
        heartbeatOutgoing: 4000
    }
)

// 3. 웹소켓 및 STOMP 연결 설정
export const connectWebSocket = (subscribeHandler:(_:string) => void) => {
    // 웹소켓 연결 성공 시 콜백
    client.onConnect = () => {
        console.log('✅ Spring 웹소켓 브로커 연결 성공!')
        client.subscribe(SUBSCRIBE_CPU_INFO_TOPIC, message => {
        if (message.body) {
             subscribeHandler(message.body)
        }})
        client.subscribe(SUBSCRIBE_SYSTEM_LIST_TOPIC, message => {
        if (message.body) {
             
        }})
    }
    client.onDisconnect = () => {
        console.log('❌ 웹소켓 연결 종료')
    }
    
    client.activate()
}

export const deactivateWebSocket = () => {
    if (client) {
        client.deactivate() // 컴포넌트가 꺼질 때 안전하게 커넥션 반납
    }
}

export const isConnected = ():boolean => {
    console.debug('check is connected', client.connected)
    if(client)
        return client.connected
    return false
}
