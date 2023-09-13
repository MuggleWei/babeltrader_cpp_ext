#ifndef TCP_CLIENT_HANDLE_H_
#define TCP_CLIENT_HANDLE_H_

#include "muggle/cpp/muggle_cpp.h"
#include "babeltrader/cpp/ext/babeltrader_cpp_ext.h"
#include "tcp_client_session.h"

USING_NS_MUGGLE;
USING_NS_BABELTRADER;

class TcpClientHandle : public NetEventHandle {
public:
	TcpClientHandle();
	virtual ~TcpClientHandle();

	virtual void OnAddCtx(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnMessage(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnClose(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnRelease(NetEventLoop *evloop, SocketContext *ctx) override;
	virtual void OnTimer(NetEventLoop *evloop) override;

	void SetServerAddr(const char *host, const char *port);
	void SetEventLoop(NetEventLoop *evloop);
	void SetCodecChain(CodecChain *codec_chain);
	void SetDispatcher(Dispatcher *dispatcher);
	void SetSSLContext(SSLContext *ssl_ctx);

	void Connect();

private:
	std::string host_;
	std::string port_;

	TcpClientSession *session_;
	time_t last_timer_trigger_;

	NetEventLoop *evloop_;
	CodecChain *codec_chain_;
	Dispatcher *dispatcher_;
	SSLContext *ssl_ctx_;
};

#endif // !TCP_CLIENT_HANDLE_H_
