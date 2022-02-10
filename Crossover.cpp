#include "stdafx.h"
#include "Crossover.h"


// y = (exp(x*a)-1)/(exp(th * a) - 1)
// y' = a * (exp(x*a)) / (exp(th * a) - 1)


Crossover::Crossover(double _a, double _th)
{
	a = abs(_a);
	th = abs(_th);

	// y' = 1
	// a * (exp(x*a)) / (exp(th * a) - 1) = 1
	// a * (exp(x*a)) = (exp(th * a) - 1)
	// exp(x*a) = (exp(th * a) - 1) / a
	// x * a = ln ((exp(th * a) - 1) / a)
	// x = ln ((exp(th * a) - 1) / a) / a
	double temp = log((exp(th * a) - 1) / a) / a;	// logº¯Êýµ×Îªe
	if (temp < 0.0) {
		theta_b = theta = 0.0;
	}
	else {
		theta = temp;
		theta_b = (exp(temp * a) - 1) / (exp(th * a) - 1) - temp;
	}
}


Crossover::~Crossover()
{
}

double Crossover::GetSample(double input)
{
	double ret;
	uint64 sign = *((uint64*)(&input)) & 0x8000000000000000ULL;
	double absinput = abs(input);
	if (absinput < theta) {
		ret = (exp(absinput * a) - 1) / (exp(th * a) - 1);
		*((uint64*)(&ret)) |= sign;
		return ret;
	}
	else {
		ret = absinput + theta_b;
		*((uint64*)(&ret)) |= sign;
		return ret;
	}
}
