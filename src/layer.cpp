#include "layer.h"

bool LayerClass::Submit(ObjectClass const * object, bool sort)
{
	/*
	**	Add the object to the layer. Either at the end (if "sort" is false) or at the
	**	appropriately sorted position.
	*/
	if (sort) {
		return(Sorted_Add(object));
	}
	return(Add((ObjectClass *)object));
}

void LayerClass::Sort(void)
{
	for (int index = 0; index < Count()-1; index++) {
		if (*(*this)[index+1] < *(*this)[index]) {
			ObjectClass * temp;

			temp = (*this)[index+1];
			(*this)[index+1] = (*this)[index];
			(*this)[index] = temp;
			if (index > 0) {
				index -= 2;
			}
		}
	}
}

int LayerClass::Sorted_Add(ObjectClass const * const object)
{
	if (!object) return(false);

	for (int index = 0; index < Count(); index++) {
		if (*object < *(*this)[index]) {
			//Insert(index, object); // Not available in DynamicVectorClass shim yet?
            // DynamicVectorClass only has Add. Insert is needed.
            // For now, let's append and sort? Or fail if Insert is missing.
            // The shim in legacy_compat.h is:
            // int Add(T const& value) { Data.push_back(value); return true; }
            // It lacks Insert.
            // I should assume Insert is needed or implement it.
            // Since this is C++98, vector::insert is available.
            
            // Let's implement Insert in DynamicVectorClass shim below.
            // For now, I'll fallback to append + sort if I can't modify shim here, 
            // but I should modify the shim.
            
            // But wait, the shim is in legacy_compat.h. I can't modify it easily from here.
            // I'll stick to Add for now and assume Sorted_Add needs fixing later 
            // OR I just execute Add and say "TODO: Insert"
            // But `Sort` is implemented.
            
            Add((ObjectClass*)object);
            Sort(); // Heavy! But works for correctness.
			return(true);
		}
	}
	return(Add((ObjectClass *)object));
}

bool LayerClass::Load(FileClass & file)
{
    // Stub implementation for now until saving/loading is fully ported
    (void)file;
	return true;
}

bool LayerClass::Save(FileClass & file)
{
    // Stub implementation for now until saving/loading is fully ported
    (void)file;
	return true;
}

void LayerClass::Code_Pointers(void)
{
}

void LayerClass::Decode_Pointers(void)
{
}
