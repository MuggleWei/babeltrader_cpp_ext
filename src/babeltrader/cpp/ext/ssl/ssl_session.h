/******************************************************************************
 *  @file         ssl_session.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp extension: ssl session
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_SSL_SESSION_H_
#define BABELTRADER_CPP_EXT_SSL_SESSION_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/session.h"
#include "babeltrader/cpp/ext/ssl/ssl_context.h"
#include "babeltrader/cpp/ext/ssl/ssl_connection.h"

NS_BABELTRADER_BEGIN

class SSLSession : public Session {
public:
	BABELTRADER_CPP_EXT_EXPORT
	SSLSession();

	BABELTRADER_CPP_EXT_EXPORT
	virtual ~SSLSession();

	BABELTRADER_CPP_EXT_EXPORT
	bool SetupSSL(SSLContext *ctx);

	BABELTRADER_CPP_EXT_EXPORT
	SSLConnection *GetSSL();

	BABELTRADER_CPP_EXT_EXPORT
	int Write(void *data, uint32_t datalen) override;

	BABELTRADER_CPP_EXT_EXPORT
	virtual int Read(void *buf, size_t len) override;

private:
	SSLConnection *ssl_conn_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_SSL_SESSION_H_
