// Environment Set-Up
//
// Preprocessor Definitions
// Note:
// To change the option flag from compiling line command use "/D" option (e.g.: "... /D _OPTION_FLEG '?' ...")

#ifndef _INCLUDE_APP_DEFAULT_SETUP
#define _INCLUDE_APP_DEFAULT_SETUP 1
												// Description/Options

#define _BUILD_OS							'V'	// V. VISUAL_STUDIO, L.LINUX, D. DEV-C++
#define _BUILD_GPU							'N' // Enable GPU compilation

#define _APP_INPUT_USE_COMPILE_LOCATION		'Y'	// In case no line-command specifics, switches between compiling and App_System.h location for searching the input file


#define _APP_OUTPUT_LOG_EXCLUDE				'N'	// Chose wheather to NOT prompt any log							(if set to 'Y' this flag will overwrite the stdou & file equivalent flags)
#define _APP_OUTPUT_LOG_STDOU_EXCLUDE		'N'	// Chose wheather to prompt logs in the shell screen (stdou)	(if set to 'Y' this flag will overwrite the type-specific equivalent flags (e.g., _SIGNAL_LOG_FILE, _SIGNAL_WARNING_FILE, etc.)
#define _APP_OUTPUT_LOG_FILE_EXCLUDE		'N'	// Chose wheather to prompt logs in the log text file			(if set to 'Y' this flag will overwrite the type-specific equivalent flags (e.g., _SIGNAL_LOG_FILE, _SIGNAL_WARNING_FILE, etc.)

#define _SIGNAL_MESSAGE_EXCLUDE				'N'	// Chose wheather to NOT prompt any message signal				(if set to 'Y' this flag will overwrite the stdou & file equivalent flags)
#define _SIGNAL_MESSAGE_STDOU				'Y'	// Chose wheather to prompt message signals in the shell screen (stdou)
#define _SIGNAL_MESSAGE_FILE				'Y'	// Chose wheather to prompt message signals in the log text file

#define _SIGNAL_WARNING_EXCLUDE				'N'	// Chose wheather to NOT prompt any warning signal				(if set to 'Y' this flag will overwrite the stdou & file equivalent flags)
#define _SIGNAL_WARNING_STDOU				'Y'	// Chose wheather to prompt warning signals in the shell screen (stdou)
#define _SIGNAL_WARNING_FILE				'Y'	// Chose wheather to prompt warning signals in the log text file

#define _SIGNAL_STREAM_EXCLUDE				'N'	// Chose wheather to NOT stream any comment						(if set to 'Y' this flag will overwrite the stdou & file equivalent flags)
#define _SIGNAL_STREAM_STDOU				'Y'	// Chose wheather to stream comments in the shell screen (stdou)
#define _SIGNAL_STREAM_FILE					'Y'	// Chose wheather to stream comments in the log text file

#define _UTILITY_STATISTICS_USE_PNG_RNG		'Y' // Chose wheather to use the png library as random number generator instead of standrd C

#define _UTILITY_USER_IO_EXCLUDE			'N'	// Chose wheather to allow user IO interactions

#define _THREADING_EXCLUDE					'N'	// Force to Serial Mode (None thread is created/joined/...)

#endif
