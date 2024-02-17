#include "Animator.h"


/*
* Constructor of Animator class. Starts an animator for a given value.
*/
Animator::Animator() {
	time = TimeGlobals::Get();
	startValue = 0;
	endValue = 0;
	startTime = 0;
	endTime = 0;
	running = false;
	switched = false;
};


Animator::~Animator() {
};


void Animator::Initialize(float value) {
	startValue = value;
	endValue = value;
}

/*
* Starts the animator by setting a target value and a duration to reach it.
*/
void Animator::Start(float duration, float finalValue) {
	float currenttime = time->GameDaysPassed->data * 24;

	startTime = currenttime;
	endTime = currenttime + duration;
	running = true;

	startValue = GetValue();
	endValue = finalValue;
}


/*
* Gets the value for the animated value at the current time.
*/
float Animator::GetValue() {
	float currenttime = time->GameDaysPassed->data * 24;
	if (!running) return startValue;

	if (currenttime > endTime) {
		running = false;
		startValue = endValue;
		return endValue;
	}

	running = true;
	currenttime = invLerp(startTime, endTime, currenttime);
	return std::lerp(startValue, endValue, currenttime);
};
