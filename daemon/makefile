# 컴파일러 및 플래그 설정
CXX = gcc
CXXFLAGS = -Wall -O2 -std=c23 -g
LDFLAGS = -pthread  # 멀티스레딩을 위한 플래그

# 실행 파일 이름 설정
TARGET = monitor_daemon

# 소스 파일 및 오브젝트 파일 설정
# (실제 작성하시는 c 파일 이름으로 수정해서 사용하세요)
SRCS = main.c resource_monitor.c socket_client.c
OBJS = $(SRCS:.c=.o)

# 기본 타겟 (make 명령어 입력 시 실행)
all: $(TARGET)

# 실행 파일 빌드
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# 개별 소스 코드를 오브젝트 파일(.o)로 컴파일
%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 빌드된 파일 삭제 (make clean)
clean:
	rm -f $(OBJS) $(TARGET)

# 전체 다시 빌드 (make rebuild)
rebuild: clean all