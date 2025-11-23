#pragma once

#ifndef READ
#define	READ	0
#endif
#ifndef WRITE
#define	WRITE	1
#endif

class FileClass
{
	public:
		virtual ~FileClass(void) {};
		virtual char const * File_Name(void) const = 0;
		virtual char const * Set_Name(char const *filename) = 0;
		virtual int Create(void) = 0;
		virtual int Delete(void) = 0;
		virtual int Is_Available(int forced=false) = 0;
		virtual int Is_Open(void) const = 0;
		virtual int Open(char const *filename, int rights=READ) = 0;
		virtual int Open(int rights=READ) = 0;
		virtual long Read(void *buffer, long size) = 0;
		virtual long Seek(long pos, int dir) = 0;
		virtual long Size(void) = 0;
		virtual long Write(void const *buffer, long size) = 0;
		virtual void Close(void) = 0;

		operator char const * () {return File_Name();};
};
