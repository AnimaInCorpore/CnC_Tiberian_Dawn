#pragma once

#include "list.h"

class CheckListClass : public ListClass {
public:
    CheckListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down);
    ~CheckListClass() {}

    void Check_Item(int index, int checked);
    int Is_Checked(int index) const;

    enum CheckListClassEnum {
        CHECK_CHAR = '\3',
        UNCHECK_CHAR = ' ',
    };

    void Set_Read_Only(int read_only) { IsReadOnly = (read_only != 0); }

protected:
    virtual int Action(unsigned flags, KeyNumType& key);

private:
    bool IsReadOnly;
};

