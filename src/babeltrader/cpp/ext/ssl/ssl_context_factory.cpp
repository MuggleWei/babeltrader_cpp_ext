#include "ssl_context_factory.h"
#include <string.h>
#include "babeltrader/cpp/ext/ssl/internal/openssl_context_factory.h"

NS_BABELTRADER_BEGIN

SSLContextFactory::SSLContextFactory()
{
	memset(last_err_, 0, sizeof(last_err_));
}
SSLContextFactory::~SSLContextFactory()
{
}

SSLContext *SSLContextFactory::NewServerContext(const char *cert,
												const char *key,
												const char *passphrase)
{
	MUGGLE_UNUSED(key);
	MUGGLE_UNUSED(cert);
	MUGGLE_UNUSED(passphrase);
	return nullptr;
}
SSLContext *SSLContextFactory::NewClientContext(const char *ca)
{
	MUGGLE_UNUSED(ca);
	return nullptr;
}

const char *SSLContextFactory::GetLastErrorString()
{
	return last_err_;
}

SSLContextFactory *NewSSLContextFactory()
{
	return new OpenSSLContextFactory();
}

NS_BABELTRADER_END
