#pragma once

#if defined(_WIN32)
	#if defined(OMXDB_DLL_EXPORTS)
		#define OMXDB_EXPORT __declspec(dllexport)
	#else
		#define OMXDB_EXPORT __declspec(dllimport)
	#endif  // defined(LEVELDB_COMPILE_LIBRARY)
#else  // defined(_WIN32)
	#if defined(OMXDB_DLL_EXPORTS)
		#define OMXDB_EXPORT __attribute__((visibility("default")))
	#else
		#define OMXDB_EXPORT
	#endif
#endif  // defined(_WIN32)
