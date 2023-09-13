#include "openssl_connection.h"
#include "openssl/err.h"
#include "muggle/c/muggle_c.h"
#include <openssl/ssl.h>

NS_BABELTRADER_BEGIN

OpenSSLConnection::OpenSSLConnection(SSL *ssl)
	: SSLConnection()
	, ssl_(ssl)
{
}

OpenSSLConnection::~OpenSSLConnection()
{
	Cleanup();
}

bool OpenSSLConnection::Attach(Session *session)
{
	if (SSL_set_fd(ssl_, session->GetSocketContex()->GetSocket()) == 0) {
		return false;
	}
	session_ = session;
	return true;
}
int OpenSSLConnection::Read(void *buf, int len)
{
	int ret = SSL_read(ssl_, buf, len);
	if (ret <= 0) {
		int err_code = SSL_get_error(ssl_, ret);
		if (err_code != SSL_ERROR_WANT_READ) {
			char err_msg[512];
			GetLastError(err_msg, sizeof(err_msg));
			LOG_ERROR("failed ssl read: err_msg=%s", err_msg);

			session_->Close();
		}
	}

	return ret;
}
int OpenSSLConnection::Write(void *data, int datalen)
{
	int ret = SSL_write(ssl_, data, datalen);
	if (ret <= 0) {
		char err_msg[512];
		GetLastError(err_msg, sizeof(err_msg));
		LOG_ERROR("failed ssl write: err_msg=%s", err_msg);

		session_->Close();
	}

	return ret;
}
void OpenSSLConnection::Shutdown()
{
	SSL_shutdown(ssl_);
}
void OpenSSLConnection::Cleanup()
{
	if (ssl_) {
		SSL_shutdown(ssl_);
		SSL_free(ssl_);
		ssl_ = nullptr;
	}
}
void OpenSSLConnection::GetLastError(char *buf, unsigned long bufsize)
{
	snprintf(buf, bufsize, "%s", ERR_reason_error_string(ERR_get_error()));
}

eSSLConnectStatus OpenSSLConnection::Connect()
{
	if (is_established_) {
		return SSL_CONNECT_STATUS_READY;
	}

	int ret = SSL_connect(ssl_);
	if (ret == 0) {
		// FATAL ERROR
		return SSL_CONNECT_STATUS_ERROR;
	} else if (ret == 1) {
		is_established_ = true;
		return SSL_CONNECT_STATUS_READY;
	} else if (ret < 0) {
		int err_code = SSL_get_error(ssl_, ret);
		if (err_code == SSL_ERROR_WANT_READ) {
			// establishing
			return SSL_CONNECT_STATUS_ESTABLISHING;
		} else {
			return SSL_CONNECT_STATUS_ERROR;
		}
	} else {
		// FATAL ERROR
		return SSL_CONNECT_STATUS_ERROR;
	}

	return SSL_CONNECT_STATUS_READY;
}
eSSLConnectStatus OpenSSLConnection::Accept()
{
	if (is_established_) {
		return SSL_CONNECT_STATUS_READY;
	}

	int ret = SSL_accept(ssl_);
	if (ret == 0) {
		// FATAL ERROR
		return SSL_CONNECT_STATUS_ERROR;
	} else if (ret == 1) {
		is_established_ = true;
		return SSL_CONNECT_STATUS_READY;
	} else if (ret < 0) {
		int err_code = SSL_get_error(ssl_, ret);
		if (err_code == SSL_ERROR_WANT_READ) {
			// establishing
			return SSL_CONNECT_STATUS_ESTABLISHING;
		} else {
			return SSL_CONNECT_STATUS_ERROR;
		}
	} else {
		// FATAL ERROR
		return SSL_CONNECT_STATUS_ERROR;
	}

	return SSL_CONNECT_STATUS_READY;
}

NS_BABELTRADER_END
