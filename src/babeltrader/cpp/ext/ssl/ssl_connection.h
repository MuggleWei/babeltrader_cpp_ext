/******************************************************************************
 *  @file         ssl_connection.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp extension: ssl connection
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_SSL_CONNECTION_H_
#define BABELTRADER_CPP_EXT_SSL_CONNECTION_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/session.h"

NS_BABELTRADER_BEGIN

enum eSSLConnectStatus {
	SSL_CONNECT_STATUS_ESTABLISHING = 0,
	SSL_CONNECT_STATUS_READY,
	SSL_CONNECT_STATUS_ERROR,
};

class SSLConnection {
public:
	BABELTRADER_CPP_EXT_EXPORT
	SSLConnection();

	BABELTRADER_CPP_EXT_EXPORT
	virtual ~SSLConnection();

	BABELTRADER_CPP_EXT_EXPORT
	virtual bool IsEstablished();

	BABELTRADER_CPP_EXT_EXPORT
	virtual bool Attach(Session *session) = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual int Read(void *buf, int len) = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual int Write(void *data, int datalen) = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual void Shutdown() = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual void Cleanup() = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual void GetLastError(char *buf, unsigned long bufsize) = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual eSSLConnectStatus Connect() = 0;

	BABELTRADER_CPP_EXT_EXPORT
	virtual eSSLConnectStatus Accept() = 0;

protected:
	bool is_established_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_SSL_CONNECTION_H_
