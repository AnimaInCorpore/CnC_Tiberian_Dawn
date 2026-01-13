#pragma once

#include "legacy_compat.h"

class FixedHeapClass {
public:
    FixedHeapClass(int size);
    virtual ~FixedHeapClass(void);

    int ID(void const* pointer);
    int Count(void) { return ActiveCount; }
    int Length(void) { return TotalCount; }
    int Avail(void) { return TotalCount - ActiveCount; }

    virtual int Set_Heap(int count, void* buffer = 0);
    virtual void* Allocate(void);
    virtual void Clear(void);
    virtual int Free(void* pointer);
    virtual int Free_All(void);

protected:
    void* operator[](int index) { return ((char*)Buffer) + (index * Size); }

    unsigned IsAllocated : 1;
    int Size;
    int TotalCount;
    int ActiveCount;
    void* Buffer;
    BooleanVectorClass FreeFlag;

private:
    FixedHeapClass& operator=(FixedHeapClass const&);
    FixedHeapClass(FixedHeapClass const&);
};

template <class T>
class TFixedHeapClass : public FixedHeapClass {
public:
    TFixedHeapClass(void) : FixedHeapClass(sizeof(T)) {}
    virtual ~TFixedHeapClass(void) {}

    int ID(T const* pointer) { return FixedHeapClass::ID(pointer); }

    virtual T* Alloc(void) { return (T*)FixedHeapClass::Allocate(); }
    virtual int Free(T* pointer) { return FixedHeapClass::Free(pointer); }

protected:
    T& operator[](int index) { return *(((T*)Buffer) + index); }
};

class FixedIHeapClass : public FixedHeapClass {
public:
    FixedIHeapClass(int size) : FixedHeapClass(size) {}
    virtual ~FixedIHeapClass(void) {}

    virtual int Set_Heap(int count, void* buffer = 0);
    virtual void* Allocate(void);
    virtual void Clear(void);
    virtual int Free(void* pointer);
    virtual int Free_All(void);

    virtual void* Active_Ptr(int index) { return ActivePointers[index]; }

    DynamicVectorClass<void*> ActivePointers;
};

template <class T>
class TFixedIHeapClass : public FixedIHeapClass {
public:
    TFixedIHeapClass(void) : FixedIHeapClass(sizeof(T)) {}
    virtual ~TFixedIHeapClass(void) {}

    int ID(T const* pointer) { return FixedIHeapClass::ID(pointer); }
    virtual T* Alloc(void) { return (T*)FixedIHeapClass::Allocate(); }
    virtual int Free(T* pointer) { return FixedHeapClass::Free(pointer); }
    virtual int Free(void* pointer) { return FixedHeapClass::Free(pointer); }

    virtual int Save(FileClass&);
    virtual int Load(FileClass&);
    virtual void Code_Pointers(void);
    virtual void Decode_Pointers(void);

    virtual T* Ptr(int index) { return (T*)FixedIHeapClass::ActivePointers[index]; }
    virtual T* Raw_Ptr(int index) { return (T*)((*this)[index]); }
};

template <class T>
int TFixedIHeapClass<T>::Save(FileClass& file)
{
    int idx;

    if (file.Write(&ActiveCount, sizeof(ActiveCount)) != sizeof(ActiveCount)) {
        return false;
    }

    for (int i = 0; i < ActiveCount; i++) {
        idx = ID(Ptr(i));
        if (file.Write(&idx, sizeof(idx)) != sizeof(idx)) {
            return false;
        }

        if (!Ptr(i)->Save(file)) {
            return false;
        }
    }

    return true;
}

template <class T>
int TFixedIHeapClass<T>::Load(FileClass& file)
{
    int a_count;

    if (file.Read(&a_count, sizeof(a_count)) != sizeof(a_count)) {
        return false;
    }

    if (a_count > TotalCount) {
        return false;
    }

    for (int i = 0; i < a_count; i++) {
        int idx;

        if (file.Read(&idx, sizeof(idx)) != sizeof(idx)) {
            return false;
        }

        T* ptr = (T*)(*this)[idx];
        FreeFlag[idx] = true;
        ActiveCount++;
        ActivePointers.Add(ptr);

        if (!ptr->Load(file)) {
            return false;
        }
    }

    return true;
}

template <class T>
void TFixedIHeapClass<T>::Code_Pointers(void)
{
    for (int i = 0; i < ActiveCount; i++) {
        Ptr(i)->Code_Pointers();
    }
}

template <class T>
void TFixedIHeapClass<T>::Decode_Pointers(void)
{
    for (int i = 0; i < ActiveCount; i++) {
        Ptr(i)->Decode_Pointers();
    }
}

