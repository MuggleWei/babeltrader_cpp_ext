#include "tcp_server_session.h"
#include "demo_msg.h"

#define CHECK_AUTH                          \
	if (!is_logined_) {                     \
		LOG_ERROR("request without login"); \
		Close();                            \
		return;                             \
	}
#define CHECK_MSG_STR_LEN(field)                                             \
	if (strlen(msg->field) >= sizeof(msg->field)) {                          \
		LOG_ERROR("message field '" #field "' length beyond limit: addr=%s", \
				  GetAddr());                                                \
		Close();                                                             \
		return;                                                              \
	}

TcpServerSession::TcpServerSession()
	: dispatcher_(nullptr)
	, is_logined_(false)
{
}
TcpServerSession::~TcpServerSession()
{
}

void TcpServerSession::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}

bool TcpServerSession::OnRead(void *data, uint32_t datalen)
{
	dispatcher_->Dispatch(this, data, datalen);
	return true;
}

void TcpServerSession::OnPing(msg_hdr_t *hdr, demo_msg_ping_t *msg)
{
	MUGGLE_UNUSED(hdr);

	LOG_TRACE("recv req ping: addr=%s, sec=%llu, nsec=%09lu", GetAddr(),
			 (unsigned long long)msg->sec, (unsigned long)msg->nsec);
	UpdateActiveTime(time(NULL));

	BABELTRADER_CPP_NEW_STACK_MSG(DEMO_MSG_ID_PONG, demo_msg_pong_t, rsp);
	rsp->sec = (uint64_t)msg->sec;
	rsp->nsec = (uint32_t)msg->nsec;
	BABELTRADER_CPP_SEND_MSG(rsp);
}
void TcpServerSession::OnLogin(msg_hdr_t *hdr, demo_msg_req_login_t *msg)
{
	MUGGLE_UNUSED(hdr);

	CHECK_MSG_STR_LEN(user_id);
	CHECK_MSG_STR_LEN(password);

	LOG_INFO("recv req login: addr=%s, user_id=%s, req_id=%lu", GetAddr(),
			 msg->user_id, (unsigned long)msg->req_id);

	// NOTE: assume login success
	user_id_ = msg->user_id;
	is_logined_ = true;

	BABELTRADER_CPP_NEW_STACK_MSG(DEMO_MSG_ID_RSP_LOGIN, demo_msg_rsp_login_t,
								  rsp);
	rsp->req_id = msg->req_id;
	rsp->login_result = 1;
	BABELTRADER_CPP_SEND_MSG(rsp);
}
void TcpServerSession::OnReqSum(msg_hdr_t *hdr, demo_msg_req_sum_t *msg)
{
	MUGGLE_UNUSED(hdr);

	CHECK_AUTH;

	if (msg->arr_len > 16) {
		LOG_ERROR("req sum array length beyond limit: addr=%s", GetAddr());
		Close();
		return;
	}

	LOG_DEBUG("recv req sum: addr=%s, user_id=%s, req_id=%lu", GetAddr(),
			 user_id_.c_str(), (unsigned long)msg->req_id);

	int32_t sum = 0;
	int32_t *arr = (int32_t *)(msg + 1);
	for (uint32_t i = 0; i < msg->arr_len; i++) {
		sum += arr[i];
	}

	BABELTRADER_CPP_NEW_STACK_MSG(DEMO_MSG_ID_RSP_SUM, demo_msg_rsp_sum_t, rsp);
	rsp->req_id = msg->req_id;
	rsp->sum = sum;
	BABELTRADER_CPP_SEND_MSG(rsp);
}

void TcpServerSession::SetUserID(const char *user_id)
{
	user_id_ = user_id;
}
const std::string &TcpServerSession::GetUserID()
{
	return user_id_;
}
