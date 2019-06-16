cmake_minimum_required(VERSION 3.12.2)

set(INCLUDEDIR "include/ChaosMaths")
set(SOURCEDIR "src")

set(HEADER_FILES
		include/ChaosMaths.h 
		${INCLUDEDIR}/SIMDMaths/Config/SIMDConfig.h
		${INCLUDEDIR}/SIMDMaths/Vector/Vector2.h
		${INCLUDEDIR}/SIMDMaths/Vector/Vector2.inl
		${INCLUDEDIR}/SIMDMaths/Vector/Vector3.h
		${INCLUDEDIR}/SIMDMaths/Vector/Vector4.h
		${INCLUDEDIR}/SIMDMaths/Vector/Vector4.inl
		${INCLUDEDIR}/SIMDMaths/Memory/SIMDAlignedMemory.h
		${INCLUDEDIR}/SIMDMaths/Matrix/Matrix3x3.h
		${INCLUDEDIR}/SIMDMaths/Matrix/Matrix3x3.inl
		${INCLUDEDIR}/SIMDMaths/Matrix/Matrix4x4.h
		${INCLUDEDIR}/SIMDMaths/Matrix/Matrix4x4.inl
		${INCLUDEDIR}/SIMDMaths/Utilities.h
		${INCLUDEDIR}/SIMDMaths/Matrix/MatrixUtilities.h
		${INCLUDEDIR}/SIMDMaths/Quaternion/Quaternion.h
		${INCLUDEDIR}/SIMDMaths/DualQuaternion/DualQuaternion.h)

set(SOURCE_FILES 
	${INCLUDEDIR}/SIMDMaths/Vector/Vector3.cpp
	${SOURCEDIR}/Quaternion/Quaternion.cpp
	${SOURCEDIR}/DualQuaternion/DualQuaternion.cpp)

GroupSources("${SOURCE_FILES}") 
GroupSources("${HEADER_FILES}") 
add_library(ChaosMaths STATIC ${SOURCE_FILES}  ${HEADER_FILES})
target_include_directories(ChaosMaths PUBLIC "include")

macro(CHECK_FOR_AVX)
    set(AVX_FLAGS)

    include(CheckCXXSourceRuns)
    set(CMAKE_REQUIRED_FLAGS)
    

    if(MSVC AND NOT MSVC_VERSION LESS 1600)
        set(CMAKE_REQUIRED_FLAGS "/arch:AVX")
    endif()

	check_cxx_source_runs("
       #include <nmmintrin.h>
		int main()
	   {
			union
			{
				__m128i v;
				int a[4];
			} vector4;

		__m128i test = { 0, 0, 0, 0};
		__m128i test2 = {0, 0, 0, 0};
		vector4.v = _mm_cmpgt_epi64(test2, test);
		if(vector4.a[0] != 0)
		return -1;
			
		return 0;
		}"
        HAVE_SSE4_2_EXTENSIONS)

    check_cxx_source_runs("
        #include <immintrin.h>
        int main()
        {
          __m256 a, b, c;
          const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
          float dst[8];
          a = _mm256_loadu_ps( src );
          b = _mm256_loadu_ps( src );
          c = _mm256_add_ps( a, b );
          _mm256_storeu_ps( dst, c );
          for( int i = 0; i < 8; i++ ){
            if( ( src[i] + src[i] ) != dst[i] ){
              return -1;
            }
          }
          return 0;
        }"
        HAVE_AVX_EXTENSIONS)

    # Check AVX2
    if(MSVC AND NOT MSVC_VERSION LESS 1800)
        set(CMAKE_REQUIRED_FLAGS "/arch:AVX2")
    endif()

    check_cxx_source_runs("
        #include <immintrin.h>
        int main()
        {
          __m256i a, b, c;
          const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
          int dst[8];
          a =  _mm256_loadu_si256( (__m256i*)src );
          b =  _mm256_loadu_si256( (__m256i*)src );
          c = _mm256_add_epi32( a, b );
          _mm256_storeu_si256( (__m256i*)dst, c );
          for( int i = 0; i < 8; i++ ){
            if( ( src[i] + src[i] ) != dst[i] ){
              return -1;
            }
          }
          return 0;
        }"
        HAVE_AVX2_EXTENSIONS)

    # Set Flags
    if(MSVC)
        if(HAVE_AVX2_EXTENSIONS AND NOT MSVC_VERSION LESS 1800)
			message("-- AVX2 is available")
            target_compile_options(ChaosMaths PUBLIC $<$<BOOL:${MSVC}>:/arch:AVX2>)
			target_compile_definitions(ChaosMaths PUBLIC _AVX2_SETTING_)
        elseif(HAVE_AVX_EXTENSIONS  AND NOT MSVC_VERSION LESS 1600)
			message("-- AVX is available")
            target_compile_options(ChaosMaths PUBLIC $<$<BOOL:${MSVC}>:/arch:AVX>)
			target_compile_definitions(ChaosMaths PUBLIC _AVX_SETTING_)
		 elseif(HAVE_SSE4_2_EXTENSIONS  AND NOT MSVC_VERSION LESS 1600)
			message("-- SSE4.2 is available")
            target_compile_options(ChaosMaths PUBLIC $<$<BOOL:${MSVC}>:/arch:AVX>)
			target_compile_definitions(ChaosMaths PUBLIC _SSE_SETTING_)
		 elseif(NOT HAVE_SSE4_2_EXTENSIONS)
			message("SIMD NOT SUPPORTED")
			target_compile_definitions(ChaosMaths PUBLIC _SIMD_SETTING_)
        endif()
    endif()
endmacro(CHECK_FOR_AVX)

CHECK_FOR_AVX()