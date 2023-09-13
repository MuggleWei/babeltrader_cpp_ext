#ifndef TCP_SERVER_SESSION_H_
#define TCP_SERVER_SESSION_H_

#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/ext/babeltrader_cpp_ext.h"
#include "demo_msg.h"

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

#define CALLBACK_DECALRE(funcname, msg_type)                              \
	static void s_##funcname(Session *session, msg_hdr_t *hdr, void *msg) \
	{                                                                     \
		TcpServerSession *server_session = (TcpServerSession *)session;   \
		server_session->funcname(hdr, (msg_type *)msg);                   \
	}                                                                     \
	void funcname(msg_hdr_t *hdr, msg_type *msg)

class TcpServerSession : public SSLSession {
public:
	TcpServerSession();
	virtual ~TcpServerSession();

	void SetDispatcher(Dispatcher *dispatcher);

	virtual bool OnRead(void *data, uint32_t datalen) override;

	CALLBACK_DECALRE(OnPing, demo_msg_ping_t);
	CALLBACK_DECALRE(OnLogin, demo_msg_req_login_t);
	CALLBACK_DECALRE(OnReqSum, demo_msg_req_sum_t);

	void SetUserID(const char *user_id);
	const std::string &GetUserID();

private:
	Dispatcher *dispatcher_;

	std::string user_id_;
	bool is_logined_;
};

#endif // !TCP_SERVER_SESSION_H_
