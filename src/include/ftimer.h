#pragma once

extern long Frame;

class TCountDownTimerClass {
public:
	TCountDownTimerClass(long set=0) {
		Set(set);
	};

	~TCountDownTimerClass(void) {}

	operator long(void) const {return Time();};

	void Set(long set) {
		Started = Frame;
		DelayTime = set;
	};

	void Clear(void) {
		Started   = -1;
		DelayTime = 0;
	};
	long Get_Start(void) const {
		return(Started);
	};
	long Get_Delay(void) const {
		return(DelayTime);
	};
	bool Active(void) const {
		return(Started != -1);
	};
	int Expired(void) const {return (Time() == 0);};
	long Time(void) const {
		long remain = DelayTime - (Frame-Started);
		if (remain < 0) remain = 0;
		return(remain);
	};

protected:
	long Started;
	long DelayTime;
};