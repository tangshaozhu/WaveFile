#pragma once
#include "Disorter.h"

// 交越失真，继承失真器
class Crossover :
	public Disorter
{
public:
	Crossover(double _a, double _th);
	~Crossover();

private:
	virtual double GetSample(double input);	// 应该加上virtual使程序清晰
	double a;
	double th;
	double theta;
	double theta_b;
};

