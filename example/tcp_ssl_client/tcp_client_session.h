#ifndef TCP_CLIENT_SESSION_H_
#define TCP_CLIENT_SESSION_H_

#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/ext/babeltrader_cpp_ext.h"
#include "demo_msg.h"

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

#define CALLBACK_DECALRE(funcname, msg_type)                              \
	static void s_##funcname(Session *session, msg_hdr_t *hdr, void *msg) \
	{                                                                     \
		TcpClientSession *client_session = (TcpClientSession *)session;   \
		client_session->funcname(hdr, (msg_type *)msg);                   \
	}                                                                     \
	void funcname(msg_hdr_t *hdr, msg_type *msg)

class TcpClientSession : public SSLSession {
public:
	TcpClientSession();
	virtual ~TcpClientSession();

	void SetDispatcher(Dispatcher *dispatcher);

	void OnConnection();
	void OnTimer();

	virtual bool OnRead(void *data, uint32_t datalen) override;

	CALLBACK_DECALRE(OnPong, demo_msg_pong_t);
	CALLBACK_DECALRE(OnRspLogin, demo_msg_rsp_login_t);
	CALLBACK_DECALRE(OnRspSum, demo_msg_rsp_sum_t);

private:
	void RequestSum();

private:
	Dispatcher *dispatcher_;

	uint32_t req_id_;
	bool is_logined_;
};

#endif // !TCP_CLIENT_SESSION_H_
