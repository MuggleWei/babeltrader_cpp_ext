/******************************************************************************
 *  @file         openssl_context.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp extension: openssl context
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_OPENSSL_CONTEXT_H_
#define BABELTRADER_CPP_EXT_OPENSSL_CONTEXT_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/ext/ssl/ssl_context.h"
#include "openssl/ssl.h"

NS_BABELTRADER_BEGIN

class OpenSSLContext : public SSLContext {
public:
	OpenSSLContext();
	virtual ~OpenSSLContext();

	virtual SSLConnection *NewConnection() override;

	void Cleanup();

	bool InitServerCtxFromFile(const char *key_filepath,
							   const char *crt_filepath);

	bool InitClientCtx(const char *ca_filepath);

private:
	SSL_CTX *ctx_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_OPENSSL_CONTEXT_H_
