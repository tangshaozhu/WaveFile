#pragma once
#include "Disorter.h"

// ��Խʧ�棬�̳�ʧ����
class Crossover :
	public Disorter
{
public:
	Crossover(float _a, float _th);
	~Crossover();

private:
	virtual float GetSample(float input);	// Ӧ�ü���virtualʹ��������
	float a;
	float th;
	float theta;
	float theta_b;
};

