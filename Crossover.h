#pragma once
#include "Disorter.h"

// ��Խʧ�棬�̳�ʧ����
class Crossover :
	public Disorter
{
public:
	Crossover(double _a, double _th);
	~Crossover();

private:
	virtual double GetSample(double input);	// Ӧ�ü���virtualʹ��������
	double a;
	double th;
	double theta;
	double theta_b;
};

