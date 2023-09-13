#include "openssl_context.h"
#include "babeltrader/cpp/ext/ssl/internal/openssl_connection.h"
#include "openssl/err.h"
#include "muggle/c/muggle_c.h"

NS_BABELTRADER_BEGIN

OpenSSLContext::OpenSSLContext()
	: ctx_(nullptr)
{
}
OpenSSLContext::~OpenSSLContext()
{
	Cleanup();
}

void OpenSSLContext::Cleanup()
{
	if (ctx_) {
		SSL_CTX_free(ctx_);
		ctx_ = nullptr;
	}
}

SSLConnection *OpenSSLContext::NewConnection()
{
	SSL *ssl = SSL_new(ctx_);
	if (ssl == nullptr) {
		LOG_ERROR("failed new ssl");
		return nullptr;
	}

	OpenSSLConnection *conn = new OpenSSLConnection(ssl);
	if (conn == nullptr) {
		LOG_ERROR("failed new ssl connection");
		SSL_free(ssl);
		return nullptr;
	}

	return conn;
}

bool OpenSSLContext::InitServerCtxFromFile(const char *key_filepath,
										   const char *crt_filepath)
{
	if (ctx_) {
		LOG_ERROR("repeated init ssl context");
		return false;
	}

	ctx_ = SSL_CTX_new(TLS_server_method());
	if (ctx_ == nullptr) {
		LOG_ERROR("failed create ssl context: err=%s",
				  ERR_reason_error_string(ERR_get_error()));
		return false;
	}

	if (SSL_CTX_use_certificate_chain_file(ctx_, crt_filepath) <= 0) {
		LOG_ERROR("failed set ssl certificate chain file: "
				  "err=%s, crt filepath=%s",
				  ERR_reason_error_string(ERR_get_error()), crt_filepath);
		Cleanup();
		return false;
	}

	if (SSL_CTX_use_PrivateKey_file(ctx_, key_filepath, SSL_FILETYPE_PEM) <=
		0) {
		LOG_ERROR("failed set ssl private key: err=%s, crt filepath=%s",
				  ERR_reason_error_string(ERR_get_error()), crt_filepath);
		Cleanup();
		return false;
	}

	return true;
}

bool OpenSSLContext::InitClientCtx(const char *ca_filepath)
{
	if (ctx_) {
		LOG_ERROR("repeated init ssl context");
		return false;
	}

	ctx_ = SSL_CTX_new(TLS_client_method());
	if (ctx_ == nullptr) {
		LOG_ERROR("failed create ssl context: err=%s",
				  ERR_reason_error_string(ERR_get_error()));
		return false;
	}

	if (ca_filepath && ca_filepath[0] != '\0') {
		if (!SSL_CTX_load_verify_locations(ctx_, ca_filepath, NULL)) {
			LOG_ERROR("failed set verify CA: err=%s, ca filepath=%s",
					  ERR_reason_error_string(ERR_get_error()), ca_filepath);
			Cleanup();
			return false;
		}
	}

	return true;
}

NS_BABELTRADER_END
