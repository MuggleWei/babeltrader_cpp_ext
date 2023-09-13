/******************************************************************************
 *  @file         ssl_context.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrder cpp extension: ssl context
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_SSL_CONTEXT_H_
#define BABELTRADER_CPP_EXT_SSL_CONTEXT_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/ext/ssl/ssl_connection.h"
#include "babeltrader/cpp/session.h"

NS_BABELTRADER_BEGIN

class SSLContext {
public:
	BABELTRADER_CPP_EXT_EXPORT
	SSLContext();
	BABELTRADER_CPP_EXT_EXPORT
	virtual ~SSLContext();

	BABELTRADER_CPP_EXT_EXPORT
	virtual SSLConnection *NewConnection();
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_SSL_CONTEXT_H_
