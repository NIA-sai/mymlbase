#pragma once
// ai done
// havn't learned yet
#include <windows.h>
namespace timer
{

	double get_cpu_frequency_ghz()
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency( &freq );
		return freq.QuadPart / 1e9;
	}
	static inline unsigned long long rdtsc()
	{
		unsigned int lo, hi;
		__asm__ __volatile__(
		    "cpuid\n\t"
		    "rdtsc\n\t"
		    : "=a"( lo ), "=d"( hi )
		    :
		    : "%rbx", "%rcx", "memory" );
		return ( (unsigned long long)hi << 32 ) | lo;
	}
	static inline double ms()
	{
		static double msf = get_cpu_frequency_ghz() * 1e6;
		return rdtsc() / msf;
	}
	double now_ms()
	{
		static LARGE_INTEGER freq = []
		{
			LARGE_INTEGER f;
			QueryPerformanceFrequency( &f );
			return f;
		}();

		LARGE_INTEGER t;
		QueryPerformanceCounter( &t );
		return t.QuadPart * 1000.0 / freq.QuadPart;
	}
}