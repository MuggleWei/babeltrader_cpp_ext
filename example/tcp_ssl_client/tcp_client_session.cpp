#include "tcp_client_session.h"

TcpClientSession::TcpClientSession()
	: dispatcher_(nullptr)
	, req_id_(0)
	, is_logined_(false)
{
}
TcpClientSession::~TcpClientSession()
{
}

void TcpClientSession::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}

void TcpClientSession::OnConnection()
{
	static const char *users[] = {
		"foo", "bar", "baz", "bill", "steve", "john"
	};

	BABELTRADER_CPP_NEW_STACK_MSG(DEMO_MSG_ID_REQ_LOGIN, demo_msg_req_login_t,
								  req);
	req->req_id = req_id_++;
	const char *user = users[rand() % (sizeof(users) / sizeof(users[0]))];
	strncpy(req->user_id, user, sizeof(req->user_id) - 1);
	strncpy(req->password, "123456", sizeof(req->password) - 1);

	LOG_INFO("req login: addr=%s, user_id=%s", GetAddr(), req->user_id);

	BABELTRADER_CPP_SEND_MSG(req);
}
void TcpClientSession::OnTimer()
{
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);

	BABELTRADER_CPP_NEW_STACK_MSG(DEMO_MSG_ID_PING, demo_msg_ping_t, req);
	req->sec = (uint64_t)ts.tv_sec;
	req->nsec = (uint32_t)ts.tv_nsec;

	LOG_TRACE("send req ping: addr=%s, sec=%llu, nsec=%09lu", GetAddr(),
			  (unsigned long long)req->sec, (unsigned long)req->nsec);

	BABELTRADER_CPP_SEND_MSG(req);

	if (is_logined_) {
		RequestSum();
	}
}

bool TcpClientSession::OnRead(void *data, uint32_t datalen)
{
	dispatcher_->Dispatch(this, data, datalen);
	return true;
}

void TcpClientSession::OnPong(msg_hdr_t *hdr, demo_msg_pong_t *msg)
{
	MUGGLE_UNUSED(hdr);

	LOG_TRACE("recv rsp pong: addr=%s, sec=%llu, nsec=%09lu", GetAddr(),
			  (unsigned long long)msg->sec, (unsigned long)msg->nsec);
	UpdateActiveTime(time(NULL));
}
void TcpClientSession::OnRspLogin(msg_hdr_t *hdr, demo_msg_rsp_login_t *msg)
{
	MUGGLE_UNUSED(hdr);

	if (!msg->login_result) {
		LOG_ERROR("login failed");
		Close();
		return;
	}
	is_logined_ = true;

	LOG_INFO("recv rsp login success");
}
void TcpClientSession::OnRspSum(msg_hdr_t *hdr, demo_msg_rsp_sum_t *msg)
{
	MUGGLE_UNUSED(hdr);

	LOG_INFO("recv rsp sum: "
			 "req_id=%lu, sum=%ld",
			 (unsigned long)msg->req_id, (long)msg->sum);
}

void TcpClientSession::RequestSum()
{
	const static uint32_t max_sum_len = 10;
	uint32_t cnt = ((uint32_t)rand()) % max_sum_len + 1;
	uint32_t datalen =
		sizeof(msg_hdr_t) + sizeof(demo_msg_req_sum_t) + sizeof(uint32_t) * cnt;

	char str[max_sum_len * 5 + 1];
	memset(str, 0, sizeof(str));
	char *p = str;

	char buf[4096];
	static_assert(sizeof(buf) >= sizeof(msg_hdr_t) +
									 sizeof(demo_msg_req_sum_t) +
									 sizeof(uint32_t) * max_sum_len,
				  "buffer not enough");
	msg_hdr_t *hdr = (msg_hdr_t *)buf;
	demo_msg_req_sum_t *req = (demo_msg_req_sum_t *)(hdr + 1);
	memset(hdr, 0, datalen);

	uint32_t *arr = (uint32_t *)(req + 1);
	for (uint32_t i = 0; i < cnt; i++) {
		arr[i] = (int32_t)((uint32_t)rand() % 201) - 100;
		int offset = snprintf(p, 5, "%d ", arr[i]);
		p += offset;
	}

	hdr->msg_id = DEMO_MSG_ID_REQ_SUM;
	req->req_id = req_id_++;
	req->arr_len = cnt;

	LOG_INFO("req sum: "
			 "req_id=%lu, req_array=[%s]",
			 (unsigned long)req->req_id, str);

	SendMessage(hdr, datalen);
}
