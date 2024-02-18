#pragma once

class Animator {
public:
	Animator();
	virtual ~Animator();
	void				Initialize(float startValue);
	float				GetValue();
	void				Start(float duration, float finalValue);

	TimeGlobals* time;
	float				startValue;
	float				endValue;
	float				startTime;
	float				endTime;
	bool				running;
	bool				switched; // set to true/false to detect if animator has been run before
};