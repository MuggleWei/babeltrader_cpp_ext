#include "tcp_server_handle.h"
#include "babeltrader/cpp/ext/ssl/ssl_context_factory.h"
#include "tcp_server_session.h"

TcpServerHandle::TcpServerHandle()
	: last_timer_trigger_(0)
	, codec_chain_(nullptr)
	, dispatcher_(nullptr)
{
}
TcpServerHandle::~TcpServerHandle()
{
}

void TcpServerHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);
	if (ctx->GetSocketType() == MUGGLE_SOCKET_CTX_TYPE_TCP_LISTEN) {
		char local_addr[MUGGLE_SOCKET_ADDR_STRLEN];
		muggle_socket_local_addr(ctx->GetSocket(), local_addr,
								 sizeof(local_addr), 0);
		LOG_INFO("TCP Listen context add into evloop: listen addr=%s",
				 local_addr);
	}
}
void TcpServerHandle::OnConnect(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("session connection: addr=%s", addr);
	TcpServerSession *session = new TcpServerSession();

	ctx->SetUserData(session);

	session->SetSocketContext(ctx);
	session->InitBytesBuffer(8 * 1024 * 1024);
	session->SetCodecChain(codec_chain_);
	session->UpdateActiveTime(time(NULL));
	session->SetAddr(addr);
	session->SetDispatcher(dispatcher_);

	if (!session->SetupSSL(ssl_ctx_)) {
		LOG_ERROR("failed setup ssl");
		ctx->Shutdown();
		return;
	}

	ctx_set_.insert(ctx);

	if (session->SSLAccept() == babeltrader::SSL_CONNECT_STATUS_ERROR) {
		char err_msg[512];
		session->GetSSL()->GetLastError(err_msg, sizeof(err_msg));
		LOG_ERROR("failed ssl connect: addr=%s, err_msg=%s", session->GetAddr(),
				  err_msg);

		session->Close();
		return;
	}
}
void TcpServerHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerSession *session = (TcpServerSession *)ctx->GetUserData();

	if (!session->IsSSLReady()) {
		if (session->SSLAccept() == babeltrader::SSL_CONNECT_STATUS_ERROR) {
			char err_msg[512];
			session->GetSSL()->GetLastError(err_msg, sizeof(err_msg));
			LOG_ERROR("failed ssl connect: addr=%s, err_msg=%s",
					  session->GetAddr(), err_msg);

			session->Close();
			return;
		}
		if (!session->IsSSLReady()) {
			return;
		}
		LOG_INFO("ssl connection established");
	}

	if (!session->HandleMessage()) {
		LOG_WARNING("failed handle message: addr=%s", session->GetAddr());
		ctx->Shutdown();
		return;
	}
}
void TcpServerHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	Session *session = (Session *)ctx->GetUserData();
	if (session) {
		LOG_INFO("session disconnection: addr=%s", session->GetAddr());
	}
}
void TcpServerHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpServerSession *session = (TcpServerSession *)ctx->GetUserData();
	if (session) {
		LOG_INFO("session release: addr=%s", session->GetAddr());
		delete session;
	}
	ctx_set_.erase(ctx);
}
void TcpServerHandle::OnTimer(NetEventLoop *evloop)
{
	MUGGLE_UNUSED(evloop);

	time_t curr_ts = time(NULL);
	time_t elapsed = curr_ts - last_timer_trigger_;
	if (elapsed < 5) {
		return;
	}
	last_timer_trigger_ = curr_ts;

	for (SocketContext *ctx : ctx_set_) {
		TcpServerSession *session = (TcpServerSession *)ctx->GetUserData();

		if (!session->IsSSLReady()) {
			continue;
		}

		time_t elapsed = curr_ts - session->LastActiveTime();
		if (elapsed > 15) {
			ctx->Shutdown();
		}
	}
}

void TcpServerHandle::SetCodecChain(CodecChain *codec_chain)
{
	codec_chain_ = codec_chain;
}
void TcpServerHandle::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
void TcpServerHandle::SetSSLContext(SSLContext *ssl_ctx)
{
	ssl_ctx_ = ssl_ctx;
}
