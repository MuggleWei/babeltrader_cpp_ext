#ifndef DEMO_MESSAGE_H_
#define DEMO_MESSAGE_H_

#include <stdint.h>

; // for avoid LSP complain
#pragma pack(push)
#pragma pack(1)

enum {
	DEMO_MSG_ID_NULL = 0,
	DEMO_MSG_ID_REQ_LOGIN,
	DEMO_MSG_ID_RSP_LOGIN,
	DEMO_MSG_ID_PING,
	DEMO_MSG_ID_PONG,
	DEMO_MSG_ID_REQ_SUM,
	DEMO_MSG_ID_RSP_SUM,
	MAX_DEMO_MSG_ID,
};

// DEMO_MSG_ID_REQ_LOGIN
typedef struct demo_msg_req_login
{
	uint32_t req_id;
	char user_id[16];
	char password[32];
} demo_msg_req_login_t;

// DEMO_MSG_ID_RSP_LOGIN
typedef struct demo_msg_rsp_login
{
	uint32_t req_id;
	uint8_t login_result;
} demo_msg_rsp_login_t;

// DEMO_MSG_ID_PING
typedef struct demo_msg_ping
{
	uint64_t sec;
	uint32_t nsec;
} demo_msg_ping_t;

// DEMO_MSG_ID_PONG
typedef struct demo_msg_pong
{
	uint64_t sec;
	uint32_t nsec;
} demo_msg_pong_t;

// DEMO_MSG_ID_REQ_SUM
typedef struct demo_msg_req_sum
{
	uint32_t req_id;
	uint32_t arr_len;
} demo_msg_req_sum_t;

// DEMO_MSG_ID_RSP_SUM
typedef struct mode_msg_rsp_sum
{
	uint32_t req_id;
	int32_t sum;
} demo_msg_rsp_sum_t;

#pragma pack(pop)

#endif // !DEMO_MESSAGE_H_
