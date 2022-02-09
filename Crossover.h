#pragma once
#include "Disorter.h"

// 交越失真，继承失真器
class Crossover :
	public Disorter
{
public:
	Crossover(float _a, float _th);
	~Crossover();

private:
	virtual float GetSample(float input);	// 应该加上virtual使程序清晰
	float a;
	float th;
	float theta;
	float theta_b;
};

