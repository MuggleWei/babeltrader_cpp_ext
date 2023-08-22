/******************************************************************************
 *  @file         macro.h
 *  @author       Muggle Wei
 *  @email        mugglewei@gmail.com
 *  @date         2023-08-22
 *  @copyright    Copyright 2023 Muggle Wei
 *  @license      MIT License
 *  @brief        babeltrader cpp ext macro
 *****************************************************************************/

#ifndef BABELTRADER_CPP_EXT_MACRO_H_
#define BABELTRADER_CPP_EXT_MACRO_H_

#include "babeltrader/cpp/macro.h"
#include "babeltrader/cpp/ext/config.h"

#if MUGGLE_PLATFORM_WINDOWS && defined(BABELTRADER_CPP_EXT_USE_DLL)
	#ifdef BABELTRADER_CPP_EXT_EXPORTS
		#define BABELTRADER_CPP_EXT_EXPORT __declspec(dllexport)
	#else
		#define BABELTRADER_CPP_EXT_EXPORT __declspec(dllimport)
	#endif
#else
	#define BABELTRADER_CPP_EXT_EXPORT
#endif

#endif // !BABELTRADER_CPP_EXT_MACRO_H_
