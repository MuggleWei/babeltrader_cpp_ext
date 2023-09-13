#include "tcp_client_handle.h"
#include "tcp_client_session.h"
#include <thread>

TcpClientHandle::TcpClientHandle()
	: session_(nullptr)
	, last_timer_trigger_(0)
	, evloop_(nullptr)
	, codec_chain_(nullptr)
	, dispatcher_(nullptr)
	, ssl_ctx_(nullptr)
{
}
TcpClientHandle::~TcpClientHandle()
{
}

void TcpClientHandle::OnAddCtx(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	char addr[MUGGLE_SOCKET_ADDR_STRLEN];
	muggle_socket_remote_addr(ctx->GetSocket(), addr, sizeof(addr), 0);

	LOG_INFO("success connect to %s", addr);

	TcpClientSession *session = new TcpClientSession();
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

	session_ = session;

	if (session->SSLConnect() == babeltrader::SSL_CONNECT_STATUS_ERROR) {
		char err_msg[512];
		session->GetSSL()->GetLastError(err_msg, sizeof(err_msg));
		LOG_ERROR("failed ssl connect: addr=%s, err_msg=%s", session->GetAddr(),
				  err_msg);

		session->Close();
		return;
	}

	if (session_->IsSSLReady()) {
		session_->OnConnection();
	}
}
void TcpClientHandle::OnMessage(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpClientSession *session = (TcpClientSession *)ctx->GetUserData();

	if (!session->IsSSLReady()) {
		if (session->SSLConnect() == babeltrader::SSL_CONNECT_STATUS_ERROR) {
			char err_msg[512];
			session_->GetSSL()->GetLastError(err_msg, sizeof(err_msg));
			LOG_ERROR("failed ssl connect: addr=%s, err_msg=%s",
					  session->GetAddr(), err_msg);

			session->Close();
			return;
		}
		if (!session->IsSSLReady()) {
			return;
		}
		LOG_INFO("ssl connection established");
		session->OnConnection();
	}

	if (!session->HandleMessage()) {
		LOG_WARNING("failed handle message: addr=%s", session->GetAddr());
		ctx->Shutdown();
		return;
	}
}
void TcpClientHandle::OnClose(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);
	MUGGLE_UNUSED(ctx);

	session_ = nullptr;

	Connect();
}
void TcpClientHandle::OnRelease(NetEventLoop *evloop, SocketContext *ctx)
{
	MUGGLE_UNUSED(evloop);

	TcpClientSession *session = (TcpClientSession *)ctx->GetUserData();
	if (session) {
		LOG_INFO("release %s", session->GetAddr());
		delete session;
	}
	ctx->SetUserData(nullptr);
}
void TcpClientHandle::OnTimer(NetEventLoop *evloop)
{
	MUGGLE_UNUSED(evloop);

	if (session_ == nullptr) {
		return;
	}

	if (!session_->IsSSLReady()) {
		return;
	}

	time_t curr_ts = time(NULL);
	time_t elapsed = curr_ts - last_timer_trigger_;
	if (elapsed < 3) {
		return;
	}
	last_timer_trigger_ = curr_ts;

	session_->OnTimer();
}

void TcpClientHandle::SetServerAddr(const char *host, const char *port)
{
	host_ = host;
	port_ = port;
}
void TcpClientHandle::SetEventLoop(NetEventLoop *evloop)
{
	evloop_ = evloop;
}
void TcpClientHandle::SetCodecChain(CodecChain *codec_chain)
{
	codec_chain_ = codec_chain;
}
void TcpClientHandle::SetDispatcher(Dispatcher *dispatcher)
{
	dispatcher_ = dispatcher;
}
void TcpClientHandle::SetSSLContext(SSLContext *ssl_ctx)
{
	ssl_ctx_ = ssl_ctx;
}

void TcpClientHandle::Connect()
{
	std::thread th([&] {
		while (true) {
			SocketContext *ctx =
				SocketUtils::TCPConnect(host_.c_str(), port_.c_str(), 3);
			if (ctx == nullptr) {
				std::this_thread::sleep_for(std::chrono::seconds(3));
				continue;
			}
			evloop_->AddContext(ctx);
			break;
		}
	});
	th.detach();
}
