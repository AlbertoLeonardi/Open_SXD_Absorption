// Environment Set-Up
//
// Preprocessor Definitions Update

#ifndef _INCLUDE_APP_DEPENDENCIES
#define _INCLUDE_APP_DEPENDENCIES 1

#include <App_Environment.h>

// *** *** *** *** *** *** *** *** *** *** *** *** *** _BUILD_GPU & _NVIDIA_CUDA_SUPPORT

#if     _BUILD_GPU == 'Y'
	#define _NVIDIA_CUDA_SUPPORT
#endif

// *** *** *** *** *** *** *** *** *** *** *** *** *** _APP_OUTPUT_LOG_EXCLUDE
#ifndef _APP_OUTPUT_LOG_EXCLUDE
	#define _APP_OUTPUT_LOG_EXCLUDE				'Y'
#endif
#if     _APP_OUTPUT_LOG_EXCLUDE == 'Y'
	#ifdef _APP_OUTPUT_LOG_STDOU_EXCLUDE
		#undef _APP_OUTPUT_LOG_STDOU_EXCLUDE
	#endif
	#ifdef _APP_OUTPUT_LOG_FILE_EXCLUDE
		#undef _APP_OUTPUT_LOG_FILE_EXCLUDE
	#endif
	#define _APP_OUTPUT_LOG_STDOU_EXCLUDE		'Y'
	#define _APP_OUTPUT_LOG_FILE_EXCLUDE		'Y'
#elif   _APP_OUTPUT_LOG_EXCLUDE != 'N'
	#undef _APP_OUTPUT_LOG_EXCLUDE;
	#define _APP_OUTPUT_LOG_EXCLUDE				'N'
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _APP_OUTPUT_LOG_STDOU_EXCLUDE
#ifndef _APP_OUTPUT_LOG_STDOU_EXCLUDE
	#define _APP_OUTPUT_LOG_STDOU_EXCLUDE		'Y'
#endif
#if     _APP_OUTPUT_LOG_STDOU_EXCLUDE == 'Y'
	#ifdef _SIGNAL_MESSAGE_STDOU
		#undef _SIGNAL_MESSAGE_STDOU
	#endif
	#ifdef _SIGNAL_WARNING_STDOU
		#undef _SIGNAL_WARNING_STDOU
	#endif
	#ifdef _SIGNAL_STREAM_STDOU
		#undef _SIGNAL_STREAM_STDOU
	#endif
	#define _SIGNAL_MESSAGE_STDOU				'N'
	#define _SIGNAL_WARNING_STDOU				'N'
	#define _SIGNAL_STREAM_STDOU				'N'
#elif   _APP_OUTPUT_LOG_STDOU_EXCLUDE != 'N'
	#undef _APP_OUTPUT_LOG_STDOU_EXCLUDE;
	#define _APP_OUTPUT_LOG_STDOU_EXCLUDE		'N'
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _APP_OUTPUT_LOG_FILE_EXCLUDE
#ifndef _APP_OUTPUT_LOG_FILE_EXCLUDE
	#define _APP_OUTPUT_LOG_FILE_EXCLUDE		'Y'
#endif
#if     _APP_OUTPUT_LOG_FILE_EXCLUDE == 'Y'
	#ifdef _SIGNAL_MESSAGE_FILE
		#undef _SIGNAL_MESSAGE_FILE
	#endif
	#ifdef _SIGNAL_WARNING_FILE
		#undef _SIGNAL_WARNING_FILE
	#endif
	#ifdef _SIGNAL_STREAM_FILE
		#undef _SIGNAL_STREAM_FILE
	#endif
	#define _SIGNAL_MESSAGE_FILE				'N'
	#define _SIGNAL_WARNING_FILE				'N'
	#define _SIGNAL_STREAM_FILE					'N'
#elif   _APP_OUTPUT_LOG_FILE_EXCLUDE != 'N'
	#undef _APP_OUTPUT_LOG_FILE_EXCLUDE;
	#define _APP_OUTPUT_LOG_FILE_EXCLUDE		'N'
#endif


// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_MESSAGE_EXCLUDE
#ifndef _SIGNAL_MESSAGE_EXCLUDE
	#define _SIGNAL_MESSAGE_EXCLUDE			'Y'
#endif
#if     _SIGNAL_MESSAGE_EXCLUDE == 'Y'
	#ifdef _SIGNAL_MESSAGE_STDOU
		#undef _SIGNAL_MESSAGE_STDOU
	#endif
	#ifdef _SIGNAL_MESSAGE_FILE
		#undef _SIGNAL_MESSAGE_FILE
	#endif
	#define _SIGNAL_MESSAGE_STDOU				'N'
	#define _SIGNAL_MESSAGE_FILE				'N'
#elif   _SIGNAL_MESSAGE_EXCLUDE != 'N'
	#undef _SIGNAL_MESSAGE_EXCLUDE;
	#define _SIGNAL_MESSAGE_EXCLUDE				'N'
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_MESSAGE_STDOU
#ifndef _SIGNAL_MESSAGE_STDOU
	#define _SIGNAL_MESSAGE_STDOU				'N'
#elif   _SIGNAL_MESSAGE_STDOU != 'Y'
	#if _SIGNAL_MESSAGE_STDOU != 'N'
		#undef _SIGNAL_MESSAGE_STDOU
		#define _SIGNAL_MESSAGE_STDOU			'N'
	#endif
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_MESSAGE_FILE
#ifndef _SIGNAL_MESSAGE_FILE
	#define _SIGNAL_MESSAGE_FILE				'N'
#elif   _SIGNAL_MESSAGE_FILE != 'Y'
	#if _SIGNAL_MESSAGE_FILE != 'N'
		#undef _SIGNAL_MESSAGE_FILE
		#define _SIGNAL_MESSAGE_FILE			'N'
	#endif
#endif


// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_WARNING_EXCLUDE
#ifndef _SIGNAL_WARNING_EXCLUDE
	#define _SIGNAL_WARNING_EXCLUDE			'Y'
#endif
#if     _SIGNAL_WARNING_EXCLUDE == 'Y'
	#ifdef _SIGNAL_WARNING_STDOU
		#undef _SIGNAL_WARNING_STDOU
	#endif
	#ifdef _SIGNAL_WARNING_FILE
		#undef _SIGNAL_WARNING_FILE
	#endif
	#define _SIGNAL_WARNING_STDOU				'N'
	#define _SIGNAL_WARNING_FILE				'N'
#elif   _SIGNAL_WARNING_EXCLUDE != 'N'
	#undef _SIGNAL_WARNING_EXCLUDE;
	#define _SIGNAL_WARNING_EXCLUDE				'N'
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_WARNING_STDOU
#ifndef _SIGNAL_WARNING_STDOU
	#define _SIGNAL_WARNING_STDOU				'N'
#elif   _SIGNAL_WARNING_STDOU != 'Y'
	#if _SIGNAL_WARNING_STDOU != 'N'
		#undef _SIGNAL_WARNING_STDOU
		#define _SIGNAL_WARNING_STDOU			'N'
	#endif
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_WARNING_FILE
#ifndef _SIGNAL_WARNING_FILE
	#define _SIGNAL_WARNING_FILE				'N'
#elif   _SIGNAL_WARNING_FILE != 'Y'
	#if _SIGNAL_WARNING_FILE != 'N'
		#undef _SIGNAL_WARNING_FILE
		#define _SIGNAL_WARNING_FILE			'N'
	#endif
#endif


// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_STREAM_EXCLUDE
#ifndef _SIGNAL_STREAM_EXCLUDE
	#define _SIGNAL_STREAM_EXCLUDE			'Y'
#endif
#if     _SIGNAL_STREAM_EXCLUDE == 'Y'
	#ifdef _SIGNAL_STREAM_STDOU
		#undef _SIGNAL_STREAM_STDOU
	#endif
	#ifdef _SIGNAL_STREAM_FILE
		#undef _SIGNAL_STREAM_FILE
	#endif
	#define _SIGNAL_STREAM_STDOU				'N'
	#define _SIGNAL_STREAM_FILE				'N'
#elif   _SIGNAL_STREAM_EXCLUDE != 'N'
	#undef _SIGNAL_STREAM_EXCLUDE;
	#define _SIGNAL_STREAM_EXCLUDE				'N'
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_STREAM_STDOU
#ifndef _SIGNAL_STREAM_STDOU
	#define _SIGNAL_STREAM_STDOU				'N'
#elif   _SIGNAL_STREAM_STDOU != 'Y'
	#if _SIGNAL_STREAM_STDOU != 'N'
		#undef _SIGNAL_STREAM_STDOU
		#define _SIGNAL_STREAM_STDOU			'N'
	#endif
#endif
// *** *** *** *** *** *** *** *** *** *** *** *** *** _SIGNAL_STREAM_FILE
#ifndef _SIGNAL_STREAM_FILE
	#define _SIGNAL_STREAM_FILE				'N'
#elif   _SIGNAL_STREAM_FILE != 'Y'
	#if _SIGNAL_STREAM_FILE != 'N'
		#undef _SIGNAL_STREAM_FILE
		#define _SIGNAL_STREAM_FILE			'N'
	#endif
#endif

#endif