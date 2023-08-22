/******************************************************************************
 *  @file         ssl_to_msg_decoder.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-22
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp ext ssl bytes to message decoder
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_SSL_TO_MSG_DECODER_H_
#define BABELTRADER_CPP_EXT_SSL_TO_MSG_DECODER_H_

#include "babeltrader/cpp/ext/macro.h"
#include "babeltrader/cpp/decoder.h"

NS_BABELTRADER_BEGIN

class SSLToMsgDecoder : public Decoder {
public:
	BABELTRADER_CPP_EXT_EXPORT
	SSLToMsgDecoder();

	BABELTRADER_CPP_EXT_EXPORT
	virtual ~SSLToMsgDecoder();

	// TODO:
};

NS_BABELTRADER_END

#endif // !BABELTRADER_CPP_EXT_SSL_TO_MSG_DECODER_H_
