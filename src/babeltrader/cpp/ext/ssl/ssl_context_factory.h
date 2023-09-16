/******************************************************************************
 *  @file         ssl_context_factory.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp extesion: ssl context factory
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_SSL_CONTEXT_FACTORY_H_
#define BABELTRADER_CPP_EXT_SSL_CONTEXT_FACTORY_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/ext/ssl/ssl_context.h"

NS_BABELTRADER_BEGIN

class SSLContextFactory {
public:
	BABELTRADER_CPP_EXT_EXPORT
	SSLContextFactory();

	BABELTRADER_CPP_EXT_EXPORT
	virtual ~SSLContextFactory();

	/**
	 * @brief new ssl server context
	 *
	 * @param cert        certificate filepath
	 * @param key         key filepath
	 * @param passphrase  passphrase for key file, if NULL, don't use passphrase
	 *
	 * @return
	 *   - on success, return ssl context pointer
	 *   - on failed, return nullptr
	 */
	BABELTRADER_CPP_EXT_EXPORT
	virtual SSLContext *NewServerContext(const char *cert, const char *key,
										 const char *passphrase);

	/**
	 * @brief new ssl client context
	 *
	 * @param ca  CA filepath
	 *
	 * @return 
	 *   - on success, return ssl context pointer
	 *   - on failed, return nullptr
	 */
	BABELTRADER_CPP_EXT_EXPORT
	virtual SSLContext *NewClientContext(const char *ca);

	/**
	 * @brief get last error string
	 *
	 * @return last error string
	 */
	BABELTRADER_CPP_EXT_EXPORT
	const char *GetLastErrorString();

protected:
	char last_err_[1024];
};

BABELTRADER_CPP_EXT_EXPORT
SSLContextFactory *NewSSLContextFactory();

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_SSL_CONTEXT_FACTORY_H_
