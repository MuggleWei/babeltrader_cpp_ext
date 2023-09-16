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

void OpenSSLContext::Cleanup()
{
	if (ctx_) {
		SSL_CTX_free(ctx_);
		ctx_ = nullptr;
	}
}

static EVP_PKEY *openssl_load_enc_key(const char *pem_filepath,
									  const char *passphrase)
{
	BIO *bp = NULL;
	EVP_PKEY *key = NULL;
	int ret = 0;

	bp = BIO_new(BIO_s_file());
	if (bp == NULL) {
		LOG_ERROR("BIO_new(BIO_s_file()) failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto load_enc_key_exit;
	}

	ret = BIO_read_filename(bp, pem_filepath);
	if (ret <= 0) {
		LOG_ERROR("BIO_read_filename failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		goto load_enc_key_exit;
	}

	key = EVP_PKEY_new();
	if (key == NULL) {
		LOG_ERROR("failed allocate key structure");
		goto load_enc_key_exit;
	}

	if (PEM_read_bio_PrivateKey(bp, &key, NULL, (void *)passphrase) == NULL) {
		LOG_ERROR("PEM_read_bio_PrivateKey failed: %s",
				  ERR_reason_error_string(ERR_get_error()));
		EVP_PKEY_free(key);
		key = NULL;
		goto load_enc_key_exit;
	}

load_enc_key_exit:
	if (bp) {
		BIO_free(bp);
	}

	return key;
}

bool OpenSSLContext::InitServerCtxFromFile(const char *crt_filepath,
										   const char *key_filepath,
										   const char *passphrase)
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

	if (passphrase) {
		EVP_PKEY *pkey = openssl_load_enc_key(key_filepath, passphrase);
		if (pkey == NULL) {
			LOG_ERROR("failed load enc private key: key_filepath=%s",
					  key_filepath);
			Cleanup();
			return false;
		}

		if (SSL_CTX_use_PrivateKey(ctx_, pkey) != 1) {
			LOG_ERROR("failed use private key: key_filepath=%s, err=%s",
					  key_filepath,
					  ERR_reason_error_string(ERR_get_error()));
			EVP_PKEY_free(pkey);
			Cleanup();
			return false;
		}

		EVP_PKEY_free(pkey);
	} else {
		if (SSL_CTX_use_PrivateKey_file(ctx_, key_filepath, SSL_FILETYPE_PEM) <=
			0) {
			LOG_ERROR("failed set ssl private key: err=%s, crt filepath=%s",
					  ERR_reason_error_string(ERR_get_error()), crt_filepath);
			Cleanup();
			return false;
		}
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
