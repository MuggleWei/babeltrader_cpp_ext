#include "openssl_connection.h"
#include <openssl/err.h>

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
	return true;
}
int OpenSSLConnection::Read(void *buf, int len)
{
	return SSL_read(ssl_, buf, len);
}
int OpenSSLConnection::Write(void *data, int datalen)
{
	return SSL_write(ssl_, data, datalen);
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
void OpenSSLConnection::GetLastError(int retcode, char *buf,
									 unsigned long bufsize)
{
	int err_code = SSL_get_error(ssl_, retcode);
	snprintf(buf, bufsize, "err_code=%d, err_msg=%s", err_code,
			 ERR_reason_error_string(ERR_get_error()));
}

bool OpenSSLConnection::Connect()
{
	if (is_established_) {
		return true;
	}

	int ret = SSL_connect(ssl_);
	if (ret == 0) {
		// FATAL ERROR
		return false;
	} else if (ret == 1) {
		is_established_ = true;
	} else if (ret < 0) {
		int err_code = SSL_get_error(ssl_, ret);
		if (err_code == SSL_ERROR_WANT_READ) {
			// establishing
			return true;
		} else {
			return false;
		}
	} else {
		// FATAL ERROR
		return false;
	}

	return true;
}
bool OpenSSLConnection::Accept()
{
	if (is_established_) {
		return true;
	}

	int ret = SSL_accept(ssl_);
	if (ret == 0) {
		// FATAL ERROR
		return false;
	} else if (ret == 1) {
		is_established_ = true;
	} else if (ret < 0) {
		int err_code = SSL_get_error(ssl_, ret);
		if (err_code == SSL_ERROR_WANT_READ) {
			// establishing
			return true;
		} else {
			return false;
		}
	} else {
		// FATAL ERROR
		return false;
	}

	return true;
}

NS_BABELTRADER_END
