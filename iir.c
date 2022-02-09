#include "iir.h"

double mult_biquad(mult_biquad_state *state, double data)
{
	double result;
	result = state->gain * (state->b1 * data + state->b2 * state->x1 + state->b3 * state->x2) - state->a2 * state->y1 - state->a3 * state->y2;
	result = result / state->a1;
	state->x2 = state->x1;
	state->x1 = data;
	state->y2 = state->y1;
	state->y1 = result;
	return result;
}
