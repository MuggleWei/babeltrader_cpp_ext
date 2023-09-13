#include "ssl_connection.h"

NS_BABELTRADER_BEGIN

SSLConnection::SSLConnection()
	: is_established_(false)
{
}

SSLConnection::~SSLConnection()
{
}

bool SSLConnection::IsEstablished()
{
	return is_established_;
}

NS_BABELTRADER_END
