#pragma once

class FacingClass
{
	public:
		FacingClass(void);
		FacingClass(DirType dir) {CurrentFacing = DesiredFacing = dir;};
		operator DirType(void) const {return CurrentFacing;};

		DirType Current(void) const {return CurrentFacing;};
		DirType Desired(void) const {return DesiredFacing;};

		int Set_Desired(DirType facing);
		int Set_Current(DirType facing);

		void Set(DirType facing) {
			Set_Current(facing);
			Set_Desired(facing);
		};

		DirType Get(void) const { return CurrentFacing; }

		int Is_Rotating(void) const {return (DesiredFacing != CurrentFacing);};

		int Difference(void) const {return (signed char)(*((unsigned char*)&DesiredFacing) - *((unsigned char*)&CurrentFacing));};
		int Difference(DirType facing) const {return (signed char)(*((unsigned char*)&facing) - *((unsigned char*)&CurrentFacing));};
		int Rotation_Adjust(int rate);

	private:
		DirType CurrentFacing;
		DirType DesiredFacing;
};
