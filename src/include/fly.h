#pragma once

typedef enum ImpactType {
	IMPACT_NONE,
	IMPACT_NORMAL,
	IMPACT_EDGE
} ImpactType;

class FlyClass {
	public:
		FlyClass(void) {
			SpeedAdd = MPH_IMMOBILE;
			SpeedAccum = 0;
		};

		void Fly_Speed(int speed, MPHType maximum);
		ImpactType Physics(COORDINATE &coord, DirType facing);
		MPHType Get_Speed(void) const {return(SpeedAdd);};

		void Code_Pointers(void);
		void Decode_Pointers(void);

	private:
		unsigned SpeedAccum;
		MPHType SpeedAdd;
};
