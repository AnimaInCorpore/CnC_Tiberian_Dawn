#ifndef GETCD_H
#define GETCD_H

/*
**  GetCDClass (portable)
**
**  The Win95 build enumerates physical CD-ROM drives and uses volume labels
**  (GDI/NOD/COVERT) to decide which disc is present. The SDL port typically
**  runs from repo-local `CD/...` mirrors instead, so "drives" are modeled as
**  available disc-root directories.
**
**  API is kept identical to the original call sites.
*/
class GetCDClass {
public:
	GetCDClass();

	int Get_First_CD_Drive();
	int Get_Number_Of_Drives();
	int Get_Next_CD_Drive();

private:
	void Refresh();

	int DriveCount;
	int Cursor;
	int Drives[16];
};

#endif // GETCD_H
