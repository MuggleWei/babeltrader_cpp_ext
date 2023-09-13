#include "ssl_session.h"
#include "muggle/c/muggle_c.h"

NS_BABELTRADER_BEGIN

SSLSession::SSLSession()
	: Session()
	, ssl_conn_(nullptr)
{
}

SSLSession::~SSLSession()
{
	if (ssl_conn_) {
		delete ssl_conn_;
		ssl_conn_ = nullptr;
	}
}

bool SSLSession::SetupSSL(SSLContext *ctx)
{
	SSLConnection *conn = ctx->NewConnection();
	if (conn == nullptr) {
		LOG_ERROR("failed setup ssl connection");
		return false;
	}

	if (!conn->Attach(this)) {
		LOG_ERROR("failed ssl connection attach to session");
		delete conn;
		return false;
	}

	ssl_conn_ = conn;

	return true;
}

SSLConnection *SSLSession::GetSSL()
{
	return ssl_conn_;
}

int SSLSession::Write(void *data, uint32_t datalen)
{
	return ssl_conn_->Write(data, datalen);
}

int SSLSession::Read(void *buf, size_t len)
{
	return ssl_conn_->Read(buf, len);
}

bool SSLSession::IsSSLReady()
{
	return GetSSL()->IsEstablished();
}

eSSLConnectStatus SSLSession::SSLAccept()
{
	return GetSSL()->Accept();
}

eSSLConnectStatus SSLSession::SSLConnect()
{
	return GetSSL()->Connect();
}

NS_BABELTRADER_END
