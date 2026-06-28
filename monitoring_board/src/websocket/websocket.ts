import { Client, type IMessage } from '@stomp/stompjs'

export const WEBSOCKET_URL = 'ws://localhost:8080/ws-monitoring' 
export const SUBSCRIBE_CPU_INFO_TOPIC = '/topic/cpu_info'
export const SUBSCRIBE_SYSTEM_LIST_TOPIC = '/topic/system_list'
type TopicType = string;
type HandlerType = (_:string)=> void;
export const subscriptionMap = new Map<TopicType, HandlerType>();

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
export const connectWebSocket = (onConnectHandler:()=>void) => {
    // 웹소켓 연결 성공 시 콜백
    client.onConnect = (frame) => {
        console.log('✅ Spring 웹소켓 브로커 연결 성공!')
        
        subscriptionMap.forEach((handler,topic,_) => {
            client.subscribe(topic, message=> {
                if(message.body) {
                    handler(message.body)
                }
            })
        })
        onConnectHandler()
    }
    client.onDisconnect = () => {
        console.log('❌ 웹소켓 연결 종료')
    }
    
    client.activate()
}

export const requestLatestStatus = () => {
  if (client && client.connected) {
    // WebSocketConfig의 ApplicationDestinationPrefixes("/app") 설정에 의해
    // /app + 컨트롤러의 @MessageMapping 주소를 결합하여 전송합니다.
    client.publish({
      destination: '/app/request_system_list',
      body: JSON.stringify({ requester: 'VueFrontEnd', message: 'Give me status' })
    });
    console.log('서버로 수동 요청을 전송했습니다.');
  } else {
    console.warn('웹소켓이 연결되어 있지 않습니다.');
  }
};

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
