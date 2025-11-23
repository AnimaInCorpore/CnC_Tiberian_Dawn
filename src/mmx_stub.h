#ifndef MMX_STUB_H
#define MMX_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

extern bool MMXAvailable;
bool Detect_MMX_Availability();
void Init_MMX();

#ifdef __cplusplus
}
#endif

#endif // MMX_STUB_H
