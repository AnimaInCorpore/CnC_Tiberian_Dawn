#include "drive.h"

/*
** Smooth turn track tables (ported from legacy `DRIVE.CPP`).
** These tables are referenced by movement code but are safe to port early as
** pure data.
*/
DriveClass::TrackType const DriveClass::Track1[24] = {
    {0x00F50000L, (DirType)0},
    {0x00EA0000L, (DirType)0},
    {0x00DF0000L, (DirType)0},
    {0x00D40000L, (DirType)0},
    {0x00C90000L, (DirType)0},
    {0x00BE0000L, (DirType)0},
    {0x00B30000L, (DirType)0},
    {0x00A80000L, (DirType)0},
    {0x009D0000L, (DirType)0},
    {0x00920000L, (DirType)0},
    {0x00870000L, (DirType)0},
    {0x007C0000L, (DirType)0},
    {0x00710000L, (DirType)0},
    {0x00660000L, (DirType)0},
    {0x005B0000L, (DirType)0},
    {0x00500000L, (DirType)0},
    {0x00450000L, (DirType)0},
    {0x003A0000L, (DirType)0},
    {0x002F0000L, (DirType)0},
    {0x00240000L, (DirType)0},
    {0x00190000L, (DirType)0},
    {0x000E0000L, (DirType)0},
    {0x00030000L, (DirType)0},
    {0x00000000L, (DirType)0}};

DriveClass::TrackType const DriveClass::Track2[] = {
    {0x00F8FF08L, (DirType)32}, {0x00F0FF10L, (DirType)32}, {0x00E8FF18L, (DirType)32}, {0x00E0FF20L, (DirType)32},
    {0x00D8FF28L, (DirType)32}, {0x00D0FF30L, (DirType)32}, {0x00C8FF38L, (DirType)32}, {0x00C0FF40L, (DirType)32},
    {0x00B8FF48L, (DirType)32}, {0x00B0FF50L, (DirType)32}, {0x00A8FF58L, (DirType)32}, {0x00A0FF60L, (DirType)32},
    {0x0098FF68L, (DirType)32}, {0x0090FF70L, (DirType)32}, {0x0088FF78L, (DirType)32}, {0x0080FF80L, (DirType)32},
    {0x0078FF88L, (DirType)32}, {0x0070FF90L, (DirType)32}, {0x0068FF98L, (DirType)32}, {0x0060FFA0L, (DirType)32},
    {0x0058FFA8L, (DirType)32}, {0x0050FFB0L, (DirType)32}, {0x0048FFB8L, (DirType)32}, {0x0040FFC0L, (DirType)32},
    {0x0038FFC8L, (DirType)32}, {0x0030FFD0L, (DirType)32}, {0x0028FFD8L, (DirType)32}, {0x0020FFE0L, (DirType)32},
    {0x0018FFE8L, (DirType)32}, {0x0010FFF0L, (DirType)32}, {0x0008FFF8L, (DirType)32}, {0x00000000L, (DirType)32}};

DriveClass::TrackType const DriveClass::Track3[] = {
    {0x01F5FF00L, (DirType)0},  {0x01EAFF00L, (DirType)0},  {0x01DFFF00L, (DirType)0},  {0x01D4FF00L, (DirType)0},
    {0x01C9FF00L, (DirType)0},  {0x01BEFF00L, (DirType)0},  {0x01B3FF00L, (DirType)0},  {0x01A8FF00L, (DirType)0},
    {0x019DFF00L, (DirType)0},  {0x0192FF00L, (DirType)0},  {0x0187FF00L, (DirType)0},  {0x0180FF00L, (DirType)0},
    {0x0175FF00L, (DirType)0},  {0x016BFF00L, (DirType)0},  {0x0160FF02L, (DirType)1},  {0x0155FF04L, (DirType)3},
    {0x014CFF06L, (DirType)4},  {0x0141FF08L, (DirType)5},  {0x0137FF0BL, (DirType)7},  {0x012EFF0FL, (DirType)8},
    {0x0124FF13L, (DirType)9},  {0x011AFF17L, (DirType)11}, {0x0110FF1BL, (DirType)12}, {0x0107FF1FL, (DirType)13},
    {0x00FCFF24L, (DirType)15}, {0x00F3FF28L, (DirType)16}, {0x00ECFF2CL, (DirType)17}, {0x00E0FF32L, (DirType)19},
    {0x00D7FF36L, (DirType)20}, {0x00CFFF3DL, (DirType)21}, {0x00C6FF42L, (DirType)23}, {0x00BAFF49L, (DirType)24},
    {0x00B0FF4DL, (DirType)25}, {0x00A8FF58L, (DirType)27}, {0x00A0FF60L, (DirType)28}, {0x0098FF68L, (DirType)29},
    {0x0090FF70L, (DirType)31}, {0x0088FF78L, (DirType)32}, {0x0080FF80L, (DirType)32}, {0x0078FF88L, (DirType)32},
    {0x0070FF90L, (DirType)32}, {0x0068FF98L, (DirType)32}, {0x0060FFA0L, (DirType)32}, {0x0058FFA8L, (DirType)32},
    {0x0050FFB0L, (DirType)32}, {0x0048FFB8L, (DirType)32}, {0x0040FFC0L, (DirType)32}, {0x0038FFC8L, (DirType)32},
    {0x0030FFD0L, (DirType)32}, {0x0028FFD8L, (DirType)32}, {0x0020FFE0L, (DirType)32}, {0x0018FFE8L, (DirType)32},
    {0x0010FFF0L, (DirType)32}, {0x0008FFF8L, (DirType)32}, {0x00000000L, (DirType)32}};

DriveClass::TrackType const DriveClass::Track4[] = {
    {0x00F5FF00L, (DirType)0},  {0x00EBFF00L, (DirType)0},  {0x00E0FF00L, (DirType)0},  {0x00D5FF00L, (DirType)0},
    {0x00CBFF01L, (DirType)0},  {0x00C0FF03L, (DirType)0},  {0x00B5FF05L, (DirType)1},  {0x00ABFF07L, (DirType)1},
    {0x00A0FF0AL, (DirType)2},  {0x0095FF0DL, (DirType)3},  {0x008BFF10L, (DirType)4},  {0x0080FF14L, (DirType)5},
    {0x0075FF18L, (DirType)8},  {0x006DFF1CL, (DirType)12}, {0x0063FF22L, (DirType)16}, {0x005AFF25L, (DirType)20},
    {0x0052FF2BL, (DirType)23}, {0x0048FF32L, (DirType)27}, {0x0040FF37L, (DirType)32}, {0x0038FF3DL, (DirType)36},
    {0x0030FF46L, (DirType)39}, {0x002BFF4FL, (DirType)43}, {0x0024FF58L, (DirType)47}, {0x0020FF60L, (DirType)51},
    {0x001BFF6DL, (DirType)54}, {0x0017FF79L, (DirType)57}, {0x0014FF82L, (DirType)60}, {0x0011FF8FL, (DirType)62},
    {0x000DFF98L, (DirType)63}, {0x0009FFA2L, (DirType)64}, {0x0006FFACL, (DirType)64}, {0x0004FFB5L, (DirType)66},
    {0x0003FFC0L, (DirType)64}, {0x0002FFCBL, (DirType)64}, {0x0001FFD5L, (DirType)64}, {0x0000FFE0L, (DirType)64},
    {0x0000FFEBL, (DirType)64}, {0x0000FFF5L, (DirType)64}, {0x00000000L, (DirType)64}};

DriveClass::TrackType const DriveClass::Track5[] = {
    {0xFFF8FE08L, (DirType)32}, {0xFFF0FE10L, (DirType)32}, {0xFFE8FE18L, (DirType)32}, {0xFFE0FE20L, (DirType)32},
    {0xFFD8FE28L, (DirType)32}, {0xFFD0FE30L, (DirType)32}, {0xFFC8FE38L, (DirType)32}, {0xFFC0FE40L, (DirType)32},
    {0xFFB8FE48L, (DirType)32}, {0xFFB0FE50L, (DirType)32}, {0xFFA8FE58L, (DirType)32}, {0xFFA0FE60L, (DirType)32},
    {0xFF98FE68L, (DirType)32}, {0xFF90FE70L, (DirType)32}, {0xFF88FE78L, (DirType)32}, {0xFF80FE80L, (DirType)32},
    {0xFF78FE88L, (DirType)32}, {0xFF71FE90L, (DirType)32}, {0xFF6AFE97L, (DirType)32}, {0xFF62FE9FL, (DirType)32},
    {0xFF5AFEA8L, (DirType)32}, {0xFF53FEB0L, (DirType)35}, {0xFF4BFEB7L, (DirType)38}, {0xFF44FEBEL, (DirType)41},
    {0xFF3EFEC4L, (DirType)44}, {0xFF39FECEL, (DirType)47}, {0xFF34FED8L, (DirType)50}, {0xFF30FEE0L, (DirType)53},
    {0xFF2DFEEBL, (DirType)56}, {0xFF2CFEF5L, (DirType)59}, {0xFF2BFF00L, (DirType)62}, {0xFF2CFF0BL, (DirType)66},
    {0xFF2DFF15L, (DirType)69}, {0xFF30FF1FL, (DirType)72}, {0xFF34FF28L, (DirType)75}, {0xFF39FF30L, (DirType)78},
    {0xFF3EFF3AL, (DirType)81}, {0xFF44FF44L, (DirType)84}, {0xFF4BFF4BL, (DirType)87}, {0xFF53FF50L, (DirType)90},
    {0xFF5AFF58L, (DirType)93}, {0xFF62FF60L, (DirType)96}, {0xFF6AFF68L, (DirType)96}, {0xFF71FF70L, (DirType)96},
    {0xFF78FF78L, (DirType)96}, {0xFF80FF80L, (DirType)96}, {0xFF88FF88L, (DirType)96}, {0xFF90FF90L, (DirType)96},
    {0xFF98FF98L, (DirType)96}, {0xFFA0FFA0L, (DirType)96}, {0xFFA8FFA8L, (DirType)96}, {0xFFB0FFB0L, (DirType)96},
    {0xFFB8FFB8L, (DirType)96}, {0xFFC0FFC0L, (DirType)96}, {0xFFC8FFC8L, (DirType)96}, {0xFFD0FFD0L, (DirType)96},
    {0xFFD8FFD8L, (DirType)96}, {0xFFE0FFE0L, (DirType)96}, {0xFFE8FFE8L, (DirType)96}, {0xFFF0FFF0L, (DirType)96},
    {0xFFF8FFF8L, (DirType)96}, {0x00000000L, (DirType)96}};

DriveClass::TrackType const DriveClass::Track6[] = {
    {0x0100FE00L, (DirType)32}, {0x00F8FE08L, (DirType)32}, {0x00F0FE10L, (DirType)32}, {0x00E8FE18L, (DirType)32},
    {0x00E0FE20L, (DirType)32}, {0x00D8FE28L, (DirType)32}, {0x00D0FE30L, (DirType)32}, {0x00C8FE38L, (DirType)32},
    {0x00C0FE40L, (DirType)32}, {0x00B8FE48L, (DirType)32}, {0x00B0FE50L, (DirType)32}, {0x00A8FE58L, (DirType)32},
    {0x00A0FE60L, (DirType)32}, {0x0098FE68L, (DirType)32}, {0x0090FE70L, (DirType)32}, {0x0088FE78L, (DirType)32},
    {0x0080FE80L, (DirType)32}, {0x0078FE88L, (DirType)32}, {0x0070FE90L, (DirType)32}, {0x0068FE98L, (DirType)32},
    {0x0060FEA0L, (DirType)32}, {0x0058FEA8L, (DirType)32}, {0x0055FEAEL, (DirType)32}, {0x004EFEB8L, (DirType)35},
    {0x0048FEC0L, (DirType)37}, {0x0042FEC9L, (DirType)40}, {0x003BFED2L, (DirType)43}, {0x0037FEDAL, (DirType)45},
    {0x0032FEE3L, (DirType)48}, {0x002BFEEBL, (DirType)51}, {0x0026FEF5L, (DirType)53}, {0x0022FEFEL, (DirType)56},
    {0x001CFF08L, (DirType)59}, {0x0019FF12L, (DirType)61}, {0x0015FF1BL, (DirType)64}, {0x0011FF26L, (DirType)64},
    {0x000EFF30L, (DirType)64}, {0x000BFF39L, (DirType)64}, {0x0009FF43L, (DirType)64}, {0x0007FF4EL, (DirType)64},
    {0x0005FF57L, (DirType)64}, {0x0003FF62L, (DirType)64}, {0x0001FF6DL, (DirType)64}, {0x0000FF77L, (DirType)64},
    {0x0000FF80L, (DirType)64}, {0x0000FF8BL, (DirType)64}, {0x0000FF95L, (DirType)64}, {0x0000FFA0L, (DirType)64},
    {0x0000FFABL, (DirType)64}, {0x0000FFB5L, (DirType)64}, {0x0000FFC0L, (DirType)64}, {0x0000FFCBL, (DirType)64},
    {0x0000FFD5L, (DirType)64}, {0x0000FFE0L, (DirType)64}, {0x0000FFEBL, (DirType)64}, {0x0000FFF5L, (DirType)64},
    {0x00000000L, (DirType)64}};

DriveClass::TrackType const DriveClass::Track7[] = {
    {0x0006FFFFL, (DirType)0},  {0x000CFFFEL, (DirType)4},  {0x0011FFFCL, (DirType)8},  {0x0018FFFAL, (DirType)12},
    {0x001FFFF6L, (DirType)16}, {0x0024FFF3L, (DirType)19}, {0x002BFFF0L, (DirType)22}, {0x0030FFFDL, (DirType)23},
    {0x0035FFEBL, (DirType)24}, {0x0038FFE8L, (DirType)25}, {0x003CFFE6L, (DirType)26}, {0x0040FFE3L, (DirType)27},
    {0x0043FFE0L, (DirType)28}, {0x0046FFDDL, (DirType)29}, {0x0043FFDFL, (DirType)30}, {0x0040FFE1L, (DirType)30},
    {0x003CFFE3L, (DirType)30}, {0x0038FFE5L, (DirType)30}, {0x0035FFE7L, (DirType)31}, {0x0030FFE9L, (DirType)31},
    {0x002BFFEBL, (DirType)31}, {0x0024FFEDL, (DirType)31}, {0x001FFFF1L, (DirType)31}, {0x0018FFF4L, (DirType)32},
    {0x0011FFF7L, (DirType)32}, {0x000CFFFAL, (DirType)32}, {0x0006FFFDL, (DirType)32}, {0x00000000L, (DirType)32}};

DriveClass::TrackType const DriveClass::Track8[] = {
    {0x0003FFFCL, (DirType)32}, {0x0006FFF7L, (DirType)36}, {0x000AFFF1L, (DirType)40}, {0x000CFFEBL, (DirType)44},
    {0x000DFFE4L, (DirType)46}, {0x000EFFDCL, (DirType)48}, {0x000FFFD5L, (DirType)50}, {0x0010FFD0L, (DirType)52},
    {0x0011FFC9L, (DirType)54}, {0x0012FFC2L, (DirType)56}, {0x0011FFC0L, (DirType)58}, {0x0010FFC2L, (DirType)60},
    {0x000EFFC9L, (DirType)62}, {0x000CFFCFL, (DirType)64}, {0x000AFFD5L, (DirType)64}, {0x0008FFDAL, (DirType)64},
    {0x0006FFE2L, (DirType)64}, {0x0004FFE9L, (DirType)64}, {0x0002FFEFL, (DirType)64}, {0x0001FFF5L, (DirType)64},
    {0x0000FFF9L, (DirType)64}, {0x00000000L, (DirType)64}};

DriveClass::TrackType const DriveClass::Track9[] = {
    {0xFFF50002L, (DirType)0},  {0xFFEB0004L, (DirType)2},  {0xFFE00006L, (DirType)4},  {0xFFD50009L, (DirType)6},
    {0xFFCE000CL, (DirType)9},  {0xFFC8000FL, (DirType)11}, {0xFFC00012L, (DirType)13}, {0xFFB80015L, (DirType)16},
    {0xFFC00012L, (DirType)18}, {0xFFC8000EL, (DirType)20}, {0xFFCE000AL, (DirType)22}, {0xFFD50004L, (DirType)24},
    {0xFFDE0000L, (DirType)26}, {0xFFE9FFF8L, (DirType)28}, {0xFFEEFFF2L, (DirType)30}, {0xFFF5FFEBL, (DirType)32},
    {0xFFFDFFE1L, (DirType)34}, {0x0002FFD8L, (DirType)36}, {0x0007FFD2L, (DirType)39}, {0x000BFFCBL, (DirType)41},
    {0x0010FFC5L, (DirType)43}, {0x0013FFBEL, (DirType)45}, {0x0015FFB7L, (DirType)48}, {0x0013FFBEL, (DirType)50},
    {0x0011FFC5L, (DirType)52}, {0x000BFFCCL, (DirType)54}, {0x0008FFD4L, (DirType)56}, {0x0005FFDFL, (DirType)58},
    {0x0003FFEBL, (DirType)62}, {0x0001FFF5L, (DirType)64}, {0x00000000L, (DirType)64}};

DriveClass::TrackType const DriveClass::Track10[] = {
    {0xFFF6000BL, (DirType)32}, {0xFFF00015L, (DirType)37}, {0xFFEB0020L, (DirType)42}, {0xFFE9002BL, (DirType)47},
    {0xFFE50032L, (DirType)52}, {0xFFE30038L, (DirType)57}, {0xFFE00040L, (DirType)60}, {0xFFE20038L, (DirType)62},
    {0xFFE40032L, (DirType)64}, {0xFFE5002AL, (DirType)68}, {0xFFE6001EL, (DirType)70}, {0xFFE70015L, (DirType)72},
    {0xFFE8000BL, (DirType)74}, {0xFFE90000L, (DirType)76}, {0xFFE8FFF5L, (DirType)78}, {0xFFE7FFEBL, (DirType)80},
    {0xFFE6FFE0L, (DirType)82}, {0xFFE5FFD5L, (DirType)84}, {0xFFE4FFCEL, (DirType)86}, {0xFFE2FFC5L, (DirType)88},
    {0xFFE0FFC0L, (DirType)90}, {0xFFE3FFC5L, (DirType)92}, {0xFFE5FFCEL, (DirType)94}, {0xFFE9FFD5L, (DirType)95},
    {0xFFEBFFE0L, (DirType)96}, {0xFFF0FFEBL, (DirType)96}, {0xFFF6FFF5L, (DirType)96}, {0x00000000L, (DirType)96}};

DriveClass::TrackType const DriveClass::Track11[] = {
    {0x01000000L, DIR_SW}, {0x00F30008L, DIR_SW}, {0x00E50010L, DIR_SW_X1}, {0x00D60018L, DIR_SW_X1},
    {0x00C80020L, DIR_SW_X1}, {0x00B90028L, DIR_SW_X1}, {0x00AB0030L, DIR_SW_X2}, {0x009C0038L, DIR_SW_X2},
    {0x008D0040L, DIR_SW_X2}, {0x007F0048L, DIR_SW_X2}, {0x00710050L, DIR_SW_X2}, {0x00640058L, DIR_SW_X2},
    {0x00550060L, DIR_SW_X2},
    {0x00000000L, DIR_SW_X2}};

DriveClass::TrackType const DriveClass::Track12[] = {
    {0xFF550060L, DIR_SW_X2}, {0xFF640058L, DIR_SW_X2}, {0xFF710050L, DIR_SW_X2}, {0xFF7F0048L, DIR_SW_X2},
    {0xFF8D0040L, DIR_SW_X2}, {0xFF9C0038L, DIR_SW_X2}, {0xFFAB0030L, DIR_SW_X2}, {0xFFB90028L, DIR_SW_X1},
    {0xFFC80020L, DIR_SW_X1}, {0xFFD60018L, DIR_SW_X1}, {0xFFE50010L, DIR_SW_X1}, {0xFFF30008L, DIR_SW},
    {0x00000000L, DIR_SW}};

DriveClass::TrackType const DriveClass::Track13[] = {
    {XYP_COORD(10, -21), (DirType)(DIR_SW - 10)}, {XYP_COORD(10, -21), (DirType)(DIR_SW - 10)},
    {XYP_COORD(10, -20), (DirType)(DIR_SW - 10)}, {XYP_COORD(10, -20), (DirType)(DIR_SW - 10)},
    {XYP_COORD(9, -18), (DirType)(DIR_SW - 10)},  {XYP_COORD(9, -18), (DirType)(DIR_SW - 10)},
    {XYP_COORD(9, -17), (DirType)(DIR_SW - 10)},  {XYP_COORD(8, -16), (DirType)(DIR_SW - 10)},
    {XYP_COORD(8, -15), (DirType)(DIR_SW - 10)},  {XYP_COORD(7, -14), (DirType)(DIR_SW - 10)},
    {XYP_COORD(7, -13), (DirType)(DIR_SW - 10)},  {XYP_COORD(6, -12), (DirType)(DIR_SW - 10)},
    {XYP_COORD(6, -11), (DirType)(DIR_SW - 10)},  {XYP_COORD(5, -10), (DirType)(DIR_SW - 10)},
    {XYP_COORD(5, -9), (DirType)(DIR_SW - 10)},   {XYP_COORD(4, -8), (DirType)(DIR_SW - 10)},
    {XYP_COORD(4, -7), (DirType)(DIR_SW - 10)},   {XYP_COORD(3, -6), (DirType)(DIR_SW - 10)},
    {XYP_COORD(3, -5), (DirType)(DIR_SW - 9)},    {XYP_COORD(2, -4), (DirType)(DIR_SW - 7)},
    {XYP_COORD(2, -3), (DirType)(DIR_SW - 5)},    {XYP_COORD(1, -2), (DirType)(DIR_SW - 3)},
    {XYP_COORD(1, -1), (DirType)(DIR_SW - 1)},
    {0x00000000L, DIR_SW}};

DriveClass::RawTrackType const DriveClass::RawTracks[13] = {
    {Track1, -1, 0, -1}, {Track2, -1, 0, -1}, {Track3, 37, 12, 22}, {Track4, 26, 11, 19}, {Track5, 45, 15, 31},
    {Track6, 44, 16, 27}, {Track7, -1, 0, -1}, {Track8, -1, 0, -1}, {Track9, -1, 0, -1}, {Track10, -1, 0, -1},
    {Track11, -1, 0, -1}, {Track12, -1, 0, -1}, {Track13, -1, 0, -1}};

DriveClass::TurnTrackType const DriveClass::TrackControl[67] = {
    {1, 0, DIR_N, F_},
    {3, 7, DIR_NE, F_D},
    {4, 9, DIR_E, F_D},
    {0, 0, DIR_SE, F_},
    {0, 0, DIR_S, F_},
    {0, 0, DIR_SW, F_},
    {4, 9, DIR_W, (DriveClass::TrackControlType)(F_X | F_D)},
    {3, 7, DIR_NW, (DriveClass::TrackControlType)(F_X | F_D)},
    {6, 8, DIR_N, (DriveClass::TrackControlType)(F_T | F_X | F_Y | F_D)},
    {2, 0, DIR_NE, F_},
    {6, 8, DIR_E, F_D},
    {5, 10, DIR_SE, F_D},
    {0, 0, DIR_S, F_},
    {0, 0, DIR_SW, F_},
    {0, 0, DIR_W, F_},
    {5, 10, DIR_NW, (DriveClass::TrackControlType)(F_T | F_X | F_Y | F_D)},
    {4, 9, DIR_N, (DriveClass::TrackControlType)(F_T | F_X | F_Y | F_D)},
    {3, 7, DIR_NE, (DriveClass::TrackControlType)(F_T | F_X | F_Y | F_D)},
    {1, 0, DIR_E, (DriveClass::TrackControlType)(F_T | F_X)},
    {3, 7, DIR_SE, (DriveClass::TrackControlType)(F_T | F_X | F_D)},
    {4, 9, DIR_S, (DriveClass::TrackControlType)(F_T | F_X | F_D)},
    {0, 0, DIR_SW, F_},
    {0, 0, DIR_W, F_},
    {0, 0, DIR_NW, F_},
    {0, 0, DIR_N, F_},
    {5, 10, DIR_NE, (DriveClass::TrackControlType)(F_Y | F_D)},
    {6, 8, DIR_E, (DriveClass::TrackControlType)(F_Y | F_D)},
    {2, 0, DIR_SE, F_Y},
    {6, 8, DIR_S, (DriveClass::TrackControlType)(F_T | F_X | F_D)},
    {5, 10, DIR_SW, (DriveClass::TrackControlType)(F_T | F_X | F_D)},
    {0, 0, DIR_W, F_},
    {0, 0, DIR_NW, F_},
    {0, 0, DIR_N, F_},
    {0, 0, DIR_NE, F_},
    {4, 9, DIR_E, (DriveClass::TrackControlType)(F_Y | F_D)},
    {3, 7, DIR_SE, (DriveClass::TrackControlType)(F_Y | F_D)},
    {1, 0, DIR_S, F_Y},
    {3, 7, DIR_SW, (DriveClass::TrackControlType)(F_X | F_Y | F_D)},
    {4, 9, DIR_W, (DriveClass::TrackControlType)(F_X | F_Y | F_D)},
    {0, 0, DIR_NW, F_},
    {0, 0, DIR_N, F_},
    {0, 0, DIR_NE, F_},
    {0, 0, DIR_E, F_},
    {5, 10, DIR_SE, (DriveClass::TrackControlType)(F_T | F_D)},
    {6, 8, DIR_S, (DriveClass::TrackControlType)(F_T | F_D)},
    {2, 0, DIR_SW, F_T},
    {6, 8, DIR_W, (DriveClass::TrackControlType)(F_X | F_Y | F_D)},
    {5, 10, DIR_NW, (DriveClass::TrackControlType)(F_X | F_Y | F_D)},
    {4, 9, DIR_N, (DriveClass::TrackControlType)(F_T | F_Y | F_D)},
    {0, 0, DIR_NE, F_},
    {0, 0, DIR_E, F_},
    {0, 0, DIR_SE, F_},
    {4, 9, DIR_S, (DriveClass::TrackControlType)(F_T | F_D)},
    {3, 7, DIR_SW, (DriveClass::TrackControlType)(F_T | F_D)},
    {1, 0, DIR_W, F_T},
    {3, 7, DIR_NW, (DriveClass::TrackControlType)(F_T | F_Y | F_D)},
    {6, 8, DIR_N, (DriveClass::TrackControlType)(F_T | F_Y | F_D)},
    {5, 10, DIR_NE, (DriveClass::TrackControlType)(F_T | F_Y | F_D)},
    {0, 0, DIR_E, F_},
    {0, 0, DIR_SE, F_},
    {0, 0, DIR_S, F_},
    {5, 10, DIR_SW, (DriveClass::TrackControlType)(F_X | F_D)},
    {6, 8, DIR_W, (DriveClass::TrackControlType)(F_X | F_D)},
    {2, 0, DIR_NW, F_X},
    {11, 11, DIR_SW, F_},
    {12, 12, DIR_SW_X2, F_},
    {13, 13, DIR_SW, F_}};

DriveClass::DriveClass(void)
    : Class(&UnitTypeClass::As_Reference(UNIT_HARVESTER)),
      Tiberium(0),
      IsHarvesting(0),
      IsReturning(0),
      IsTurretLockedDown(0),
      IsOnShortTrack(0),
      SpeedAccum(0),
      TrackNumber(0),
      TrackIndex(0)
{
}

DriveClass::DriveClass(UnitType classid, HousesType house)
    : Class(&UnitTypeClass::As_Reference(classid)),
      Tiberium(0),
      IsHarvesting(0),
      IsReturning(0),
      IsTurretLockedDown(0),
      IsOnShortTrack(0),
      SpeedAccum(0),
      TrackNumber(0),
      TrackIndex(0)
{
    (void)house;
}

int DriveClass::Offload_Tiberium_Bail(void) { return 0; }

void DriveClass::Do_Turn(DirType dir) { (void)dir; }

void DriveClass::Approach_Target(void) {}

ObjectTypeClass const& DriveClass::Class_Of(void) const
{
    static ObjectTypeClass dummy(false, false, false, false, false, false, false, false, 0, "Drive", ARMOR_NONE, 0);
    return dummy;
}

void DriveClass::Overrun_Square(CELL cell, bool threaten)
{
    (void)cell;
    (void)threaten;
}

void DriveClass::Assign_Destination(TARGET target) { (void)target; }

void DriveClass::Per_Cell_Process(bool center) { (void)center; }

bool DriveClass::Ok_To_Move(DirType) const { return true; }

void DriveClass::AI(void) {}

#ifdef CHEAT_KEYS
void DriveClass::Debug_Dump(MonoClass* mono) const { (void)mono; }
#endif

void DriveClass::Force_Track(int track, COORDINATE coord)
{
    (void)track;
    (void)coord;
}

int DriveClass::Tiberium_Load(void) const
{
    if (*this == UNIT_HARVESTER) {
        return Cardinal_To_Fixed(UnitTypeClass::STEP_COUNT, Tiberium);
    }
    return 0x0000;
}

void DriveClass::Exit_Map(void) {}

void DriveClass::Mark_Track(COORDINATE headto, MarkType type)
{
    (void)headto;
    (void)type;
}

void DriveClass::Code_Pointers(void) {}

void DriveClass::Decode_Pointers(void) {}

void DriveClass::Fixup_Path(PathType* path) { (void)path; }

bool DriveClass::While_Moving(void) { return false; }

bool DriveClass::Start_Of_Move(void) { return false; }

void DriveClass::Lay_Track(void) {}

COORDINATE DriveClass::Smooth_Turn(COORDINATE adj, DirType* dir)
{
    (void)dir;
    return adj;
}
