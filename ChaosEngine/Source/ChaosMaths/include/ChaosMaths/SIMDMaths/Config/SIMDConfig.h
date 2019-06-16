#pragma once
#if (defined(_M_AMD64) || defined(_M_X64) || defined(__amd64)) && ! defined(__x86_64__)
	#define __x86_64__ 1
#endif

#ifdef _AVX2_SETTING_
    #if _AVX2_SETTING_ == 1
        #ifdef __GNUC__
	        #include <x86intrin.h>         // x86intrin.h includes header files for whatever instruction						               // sets are specified on the compiler command line, such as					              // xopintrin.h, fma4intrin.h
        #else
	        #include <immintrin.h>       // MS version of immintrin.h covers AVX, AVX2 and FMA3
        #endif

    #elif  defined(_AVX_SETTING_)
		#if _AVX_SETTING_ == 1	
		    #include <immintrin.h> 
		#endif
    #elif  defined(_SSE_SETTING_)
		#if _SSE_SETTING_ == 1	
		    #include <nmmintrin.h>             // SSE4.2
		#endif
    #endif
#endif