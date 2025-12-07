#pragma once
#include "../base.hpp"
struct Oper
{
	static const uint paramCnt = 2;
	static const uint ansCnt = 1;
	virtual void exec( bool ) = 0;
	virtual void calGrad() = 0;
	virtual ~Oper() {}
};

