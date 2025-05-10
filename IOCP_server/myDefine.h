#pragma once

// 클라이언트 처리를 위한 작업자 스레드 개수.
// 4개로 일단 테스트
#define MAX_THREAD_CNT		4
#define MAX_ENGINE_CNT      2 // 언리얼, 유니티만 고려

#define INVALID_VALUE -1 // user 유효하지 않은 값 부여

#define MAX_BUFFER 65535 // 16 bit , 2byte

#define MAX_RECV 65535 // 0x8000
#define MAX_SEND 65535 //