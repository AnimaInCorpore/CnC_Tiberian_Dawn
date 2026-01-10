/*
**  Command & Conquer(tm)
**  Copyright 2025 Electronic Arts Inc.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* $Header:   F:\\projects\\c&c\\vcs\\code\\factory.cpv   2.18   16 Oct 1995 16:51:26   JOE_BOSTIC  $ */

#include "function.h"

#include "factory.h"

namespace {
template <typename T, int MaxCount>
class FixedPoolClass {
public:
    FixedPoolClass() : ActiveCount(0) {
        for (int i = 0; i < MaxCount; ++i) Used[i] = false;
    }

    void* Allocate() {
        for (int i = 0; i < MaxCount; ++i) {
            if (!Used[i]) {
                Used[i] = true;
                ++ActiveCount;
                return Slots[i].Data;
            }
        }
        return NULL;
    }

    void Free(T* ptr) {
        if (!ptr) return;
        int id = ID(ptr);
        if (id < 0) return;
        if (Used[id]) {
            Used[id] = false;
            if (ActiveCount > 0) --ActiveCount;
        }
    }

    void Free_All() {
        for (int i = 0; i < MaxCount; ++i) Used[i] = false;
        ActiveCount = 0;
    }

    int ID(void const* ptr) const {
        for (int i = 0; i < MaxCount; ++i) {
            if (ptr == Slots[i].Data) return i;
        }
        return -1;
    }

    int ActiveCount;

private:
    union Slot {
        void* AlignPtr;
        double AlignDouble;
        long AlignLong;
        unsigned char Data[sizeof(T)];
    };

    Slot Slots[MaxCount];
    bool Used[MaxCount];
};

static FixedPoolClass<FactoryClass, FACTORY_MAX> Factories;
}  // namespace

/***********************************************************************************************
 * FactoryClass::Validate -- validates factory pointer                                         *
 *=============================================================================================*/
#ifdef CHEAT_KEYS
int FactoryClass::Validate(void) const {
    int num;

    num = Factories.ID(this);
    if (num < 0 || num >= FACTORY_MAX) {
        Validate_Error("FACTORY");
        return (0);
    } else
        return (1);
}
#else
#define Validate()
#endif

/***********************************************************************************************
 * FactoryClass::FactoryClass -- Default constructor for factory objects.                      *
 *=============================================================================================*/
FactoryClass::FactoryClass(void) {
    IsSuspended = false;
    IsDifferent = false;
    Balance = 0;
    SpecialItem = SPC_NONE;
    Object = NULL;
    House = NULL;
    Set_Rate(0);
    Set_Stage(0);
}

/***********************************************************************************************
 * FactoryClass::~FactoryClass -- Default destructor for factory objects.                      *
 *=============================================================================================*/
FactoryClass::~FactoryClass(void) {
    if (GameActive) {
        Abandon();
    }
}

/***********************************************************************************************
 * FactoryClass::Init -- Clears all units for scenario preparation.                            *
 *=============================================================================================*/
void FactoryClass::Init(void) { Factories.Free_All(); }

/***********************************************************************************************
 * FactoryClass::operator new -- Allocates a factory object from the free factory pool.        *
 *=============================================================================================*/
void* FactoryClass::operator new(size_t) {
    void* ptr = Factories.Allocate();
    if (ptr) {
        ((FactoryClass*)ptr)->IsActive = true;
    }
    return (ptr);
}

/***********************************************************************************************
 * FactoryClass::operator delete -- Returns a factory to the free factory pool.                *
 *=============================================================================================*/
void FactoryClass::operator delete(void* ptr) {
    if (ptr) {
        ((FactoryClass*)ptr)->IsActive = false;
    }
    Factories.Free((FactoryClass*)ptr);
}

/***********************************************************************************************
 * FactoryClass::AI -- Process factory production logic.                                       *
 *=============================================================================================*/
void FactoryClass::AI(void) {
    Validate();
    if (!IsSuspended && (Object != NULL || SpecialItem)) {
        int stages = 1;

        /*
        **  Determine the acceleration factor for factory production.
        **  This applies only to human players. The computer builds
        **  units on a building by building basis -- quantity of building
        **  factory types doesn't affect individual factories.
        */
        if (Object && House->IsHuman) {
            switch (Object->What_Am_I()) {
                case RTTI_AIRCRAFT:
                    stages = House->AircraftFactories;
                    break;

                case RTTI_INFANTRY:
                    stages = House->InfantryFactories;
                    break;

                case RTTI_UNIT:
                    stages = House->UnitFactories;
                    break;

                case RTTI_BUILDING:
                    stages = House->BuildingFactories;
                    break;
            }
            stages = MAX(stages, 1);
        }

        for (int index = 0; index < stages; index++) {
            if (!Has_Completed() && Graphic_Logic()) {
                IsDifferent = true;

                int cost = Cost_Per_Tick();

                cost = MIN(cost, Balance);

                /*
                **  Enough time has expired so that another production step can occur.
                **  If there is insufficient funds, then go back one production step and
                **  continue the countdown. The idea being that by the time the next
                **  production step occurs, there may be sufficient funds available.
                */
                if (cost > House->Available_Money()) {
                    Set_Stage(Fetch_Stage() - 1);
                } else {
                    House->Spend_Money(cost);
                    Balance -= cost;
                }
                if (Debug_Instant_Build) {
                    Set_Stage(STEP_COUNT);
                }
                /*
                **  If the production has completed, then suspend further production.
                */
                if (Fetch_Stage() == STEP_COUNT) {
                    IsSuspended = true;
                    Set_Rate(0);
                    House->Spend_Money(Balance);
                    Balance = 0;
                }
            }
        }
    }
}

/***********************************************************************************************
 * FactoryClass::Has_Changed -- Checks to see if a production step has occurred?               *
 *=============================================================================================*/
bool FactoryClass::Has_Changed(void) {
    Validate();
    bool changed = IsDifferent;
    IsDifferent = false;
    return (changed);
}

/***********************************************************************************************
 * FactoryClass::Set -- Assigns a factory to produce an object.                                *
 *=============================================================================================*/
bool FactoryClass::Set(TechnoTypeClass const& object, HouseClass& house) {
    Validate();
    Abandon();

    IsDifferent = true;
    IsSuspended = true;
    Set_Rate(0);
    Set_Stage(0);

    Object = (TechnoClass*)object.Create_One_Of(&house);

    if (Object) {
        House = Object->House;
        Balance = object.Cost_Of();
        Object->PurchasePrice = Balance;
    }

    return (Object != NULL);
}

/***********************************************************************************************
 * FactoryClass::Set -- Force factory to "produce" special object.                             *
 *=============================================================================================*/
bool FactoryClass::Set(int const& type, HouseClass& house) {
    Validate();
    Abandon();

    IsDifferent = true;
    IsSuspended = true;
    Set_Rate(0);
    Set_Stage(0);

    SpecialItem = type;
    House = &house;
    Balance = 0;

    return (SpecialItem != SPC_NONE);
}

/***********************************************************************************************
 * FactoryClass::Set -- Fills a factory with an already completed object.                      *
 *=============================================================================================*/
void FactoryClass::Set(TechnoClass& object) {
    Validate();
    Abandon();
    Object = &object;
    House = Object->House;
    Balance = 0;
    Set_Rate(0);
    Set_Stage(STEP_COUNT);
    IsDifferent = true;
    IsSuspended = true;
}

/***********************************************************************************************
 * FactoryClass::Suspend -- Temporarily stop production.                                       *
 *=============================================================================================*/
bool FactoryClass::Suspend(void) {
    Validate();
    if (!IsSuspended) {
        IsSuspended = true;
        Set_Rate(0);
        return (true);
    }
    return (false);
}

/***********************************************************************************************
 * FactoryClass::Start -- Resumes production after suspension or creation.                     *
 *=============================================================================================*/
bool FactoryClass::Start(void) {
    Validate();
    if ((Object || SpecialItem) && IsSuspended && !Has_Completed()) {
        if (House->Available_Money() >= Cost_Per_Tick()) {
            int time;

            if (Object) {
                time = Object->Class_Of().Time_To_Build(House->Class->House);
            } else {
                time = TICKS_PER_MINUTE * 5;
            }

            int frac = House->Power_Fraction();
            frac = Bound(frac, 0x0010, 0x0100);
            int rate = (time * 256) / frac;

            rate /= STEP_COUNT;
            rate = Bound(rate, 1, 255);

            Set_Rate(rate);
            IsSuspended = false;
            return (true);
        }
    }
    return (false);
}

/***********************************************************************************************
 * FactoryClass::Abandon -- Abandons current construction with money refunded.                 *
 *=============================================================================================*/
bool FactoryClass::Abandon(void) {
    Validate();
    if (Object) {
        if (Object) {
            House->Refund_Money(Object->Class_Of().Cost_Of() - Balance);
            Balance = 0;

            ScenarioInit++;
            delete Object;
            Object = NULL;
            ScenarioInit--;
        }
        if (SpecialItem) {
            SpecialItem = SPC_NONE;
        }

        Set_Rate(0);
        Set_Stage(0);
        IsSuspended = true;
        IsDifferent = true;

        return (true);
    }
    return (false);
}

/***********************************************************************************************
 * FactoryClass::Completion -- Fetchs the completion step for this factory.                    *
 *=============================================================================================*/
int FactoryClass::Completion(void) {
    Validate();
    return (Fetch_Stage());
}

/***********************************************************************************************
 * FactoryClass::Has_Completed -- Checks to see if object has completed production.            *
 *=============================================================================================*/
bool FactoryClass::Has_Completed(void) {
    Validate();
    if (Object && Fetch_Stage() == STEP_COUNT) {
        return (true);
    }
    if (SpecialItem && Fetch_Stage() == STEP_COUNT) {
        return (true);
    }
    return (false);
}

/***********************************************************************************************
 * FactoryClass::Get_Object -- Fetches pointer to object being constructed.                    *
 *=============================================================================================*/
TechnoClass* FactoryClass::Get_Object(void) const {
    Validate();
    return (Object);
}

/***************************************************************************
 * FactoryClass::Get_Special_Item -- gets factorys spc prod item           *
 *=========================================================================*/
int FactoryClass::Get_Special_Item(void) const {
    Validate();
    return (SpecialItem);
}

/***********************************************************************************************
 * FactoryClass::Cost_Per_Tick -- Breaks entire production cost into managable chunks.         *
 *=============================================================================================*/
int FactoryClass::Cost_Per_Tick(void) {
    Validate();
    if (Object) {
        int steps = STEP_COUNT - Fetch_Stage();
        if (steps) {
            return (Balance / steps);
        }
        return (Balance);
    }
    return (0);
}

/***********************************************************************************************
 * FactoryClass::Completed -- Clears factory object after a completed production process.      *
 *=============================================================================================*/
bool FactoryClass::Completed(void) {
    Validate();
    if (Object && Fetch_Stage() == STEP_COUNT) {
        Object = NULL;
        IsSuspended = true;
        IsDifferent = true;
        Set_Stage(0);
        Set_Rate(0);
        return (true);
    }

    if (SpecialItem && Fetch_Stage() == STEP_COUNT) {
        SpecialItem = SPC_NONE;
        IsSuspended = true;
        IsDifferent = true;
        Set_Stage(0);
        Set_Rate(0);
        return (true);
    }
    return (false);
}

