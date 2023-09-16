#include "openssl_context_factory.h"
#include <string.h>
#include "openssl_context.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

NS_BABELTRADER_BEGIN

OpenSSLContextFactory::OpenSSLContextFactory()
{
}
OpenSSLContextFactory::~OpenSSLContextFactory()
{
}

SSLContext *OpenSSLContextFactory::NewServerContext(const char *cert,
													const char *key,
													const char *passphrase)
{
	OpenSSLContext *ctx = new OpenSSLContext();
	if (ctx == nullptr) {
		strncpy(last_err_,
				"failed allocate memory space for ssl server context",
				sizeof(last_err_) - 1);
		return nullptr;
	}

	if (!ctx->InitServerCtxFromFile(cert, key, passphrase)) {
		delete ctx;
		return nullptr;
	}

	return ctx;
}
SSLContext *OpenSSLContextFactory::NewClientContext(const char *ca)
{
	OpenSSLContext *ctx = new OpenSSLContext();
	if (ctx == nullptr) {
		strncpy(last_err_,
				"failed allocate memory space for ssl client context",
				sizeof(last_err_) - 1);
		return nullptr;
	}

	if (!ctx->InitClientCtx(ca)) {
		delete ctx;
		return nullptr;
	}

	return ctx;
}

NS_BABELTRADER_END
