/******************************************************************************
 *  @file         openssl_context_factory.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp extension: internal openssl context factory
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_INTERNAL_OPENSSL_CONTEXT_FACTORY_H_
#define BABELTRADER_CPP_EXT_INTERNAL_OPENSSL_CONTEXT_FACTORY_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/ext/ssl/ssl_context_factory.h"

NS_BABELTRADER_BEGIN

class OpenSSLContextFactory : public SSLContextFactory {
public:
	OpenSSLContextFactory();
	virtual ~OpenSSLContextFactory();

	virtual SSLContext *NewServerContext(const char *key,
										 const char *cert) override;
	virtual SSLContext *NewClientContext(const char *ca) override;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_INTERNAL_OPENSSL_CONTEXT_FACTORY_H_
