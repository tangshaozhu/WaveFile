#include "stdafx.h"
#include "Crossover.h"


// y = (exp(x*a)-1)/(exp(th * a) - 1)
// y' = a * (exp(x*a)) / (exp(th * a) - 1)


Crossover::Crossover(float _a, float _th)
{
	a = fabs(_a);
	th = fabs(_th);

	// y' = 1
	// a * (exp(x*a)) / (exp(th * a) - 1) = 1
	// a * (exp(x*a)) = (exp(th * a) - 1)
	// exp(x*a) = (exp(th * a) - 1) / a
	// x * a = ln ((exp(th * a) - 1) / a)
	// x = ln ((exp(th * a) - 1) / a) / a
	double temp = log((exp(th * a) - 1) / a) / a;	// logº¯Êýµ×Îªe
	if (temp < 0.f) {
		theta_b = theta = 0.0f;
	}
	else {
		theta = temp;
		theta_b = (exp((double)temp * a) - 1) / (exp((double)th * a) - 1) - (double)temp;
	}
}


Crossover::~Crossover()
{
}

float Crossover::GetSample(float input)
{
	float ret;
	uint32 sign = *((uint32*)(&input)) & 0x80000000;
	float absinput = abs(input);
	if (absinput < theta) {
		ret = (exp(absinput * a) - 1) / (exp(th * a) - 1);
		*((uint32*)(&ret)) |= sign;
		return ret;
	}
	else {
		ret = absinput + theta_b;
		*((uint32*)(&ret)) |= sign;
		return ret;
	}
}
