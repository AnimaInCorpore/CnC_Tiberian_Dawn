#pragma once

#include "legacy_compat.h"
#include "file.h"

class LayerClass : public DynamicVectorClass<ObjectClass *>
{
	public:
		void Sort(void);
		bool Submit(ObjectClass const * object, bool sort=false);
		int Sorted_Add(ObjectClass const * const object);

		virtual void Init(void) {Clear();};
		virtual void One_Time(void) {};

		/*
		**	File I/O.
		*/
		bool Load(FileClass & file);
		bool Save(FileClass & file);
		virtual void Code_Pointers(void);
		virtual void Decode_Pointers(void);
};

