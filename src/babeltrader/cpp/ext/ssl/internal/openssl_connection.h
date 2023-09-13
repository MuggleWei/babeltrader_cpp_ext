/******************************************************************************
 *  @file         openssl_connection.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-09-13
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp extension: openssl connection
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_OPENSSL_CONNECTION_H_
#define BABELTRADER_CPP_EXT_OPENSSL_CONNECTION_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/ext/ssl/ssl_connection.h"
#include "openssl/ssl.h"

NS_BABELTRADER_BEGIN

class OpenSSLConnection : public SSLConnection {
public:
	OpenSSLConnection(SSL *ssl);
	virtual ~OpenSSLConnection();

	virtual bool Attach(Session *session) override;
	virtual int Read(void *buf, int len) override;
	virtual int Write(void *data, int datalen) override;
	virtual void Shutdown() override;
	virtual void Cleanup() override;
	virtual void GetLastError(int retcode, char *buf,
							  unsigned long bufsize) override;

	virtual bool Connect() override;
	virtual bool Accept() override;

private:
	SSL *ssl_;
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_OPENSSL_CONNECTION_H_
