/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* $Header:   F:\projects\c&c\vcs\code\dpmi.h_v   2.17   16 Oct 1995 16:44:52   JOE_BOSTIC  $ */
/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               *** 
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer                                            *
 *                                                                                             *
 *                    File Name : DPMI.H                                                       *
 *                                                                                             *
 *                   Programmer : Joe L. Bostic                                                *
 *                                                                                             *
 *                   Start Date : July 2, 1994                                                 *
 *                                                                                             *
 *                  Last Update : July 2, 1994   [JLB]                                         *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 #ifndef DPMI_H
 #define DPMI_H

 #include <dos.h>
 #include <cstdlib>
 #include <cstdio>
 #include <cstring>
 #include <cstdint>


 extern void output(short port, short data);

 class DOSSegmentClass {
	 /*
	 ** This is the selector/segment value. In real mode it is the segment,
	 ** in protected mode it is the selector (also 16 bits). This value is
	 ** moved into DS or ES when accessing memory. Note: in Watcom flat
	 ** addressing, Selector == Segment<<4 (ex: 0x0A0000)
	 */
	 uintptr_t Selector;

	 /*
	 ** These are C equivalents for pushing and popping the DS segment register.
	 */
	 void Push_DS(void) {/*__emit__(0x1E);*/};
	 void Pop_DS(void) {/*__emit__(0x1F);*/};

 public:
	 DOSSegmentClass(void);
	 ~DOSSegmentClass(void);
	 DOSSegmentClass(unsigned short segment, long size=(1024L*64L));

	 uintptr_t Get_Selector(void);

	 void Assign(unsigned short segment, long size=(1024L*64L));

	 void Copy_To(void *source, int dest, int size);
	 void Copy_From(void *dest, int source, int size);
	 void Copy_Word_To(short data, int dest);
	 void Copy_Byte_To(char data, int dest);
	 void Copy_DWord_To(long data, int dest);
	 short Copy_Word_From(int source);
	 char Copy_Byte_From(int source);
	 long Copy_DWord_From(int source);

	 static void Copy(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size);
	 static void Swap(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size);
 };


 inline DOSSegmentClass::DOSSegmentClass(void)
 {
	 Selector = 0xB0000;
 }

 inline DOSSegmentClass::~DOSSegmentClass(void)
 {
 }

 inline void DOSSegmentClass::Copy_Word_To(short data, int dest)
 {
	 *(short *)(reinterpret_cast<void*>(Selector + dest)) = data;
 }

 inline void DOSSegmentClass::Copy_Byte_To(char data, int dest)
 {
	 *(char *)(reinterpret_cast<void*>(Selector + dest)) = data;
 }

 inline void DOSSegmentClass::Copy_DWord_To(long data, int dest)
 {
	 *(long *)(reinterpret_cast<void*>(Selector + dest)) = data;
 }

 inline DOSSegmentClass::DOSSegmentClass(unsigned short segment, long)
 {
	 Assign(segment);
 }

 inline void DOSSegmentClass::Assign(unsigned short segment, long)
 {
	 Selector = static_cast<uintptr_t>(segment) << 4;
 }

 inline void DOSSegmentClass::Copy_To(void *source, int dest, int size)
 {
	 memmove(reinterpret_cast<void*>(Selector + dest), source, static_cast<std::size_t>(size));
 }

 inline void DOSSegmentClass::Copy_From(void *dest, int source, int size)
 {
	 memmove(dest, reinterpret_cast<void*>(Selector + source), static_cast<std::size_t>(size));
 }

 inline void DOSSegmentClass::Copy(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size) {
	 memmove(reinterpret_cast<void*>(dest.Selector + doffset), reinterpret_cast<void*>(src.Selector + soffset), static_cast<std::size_t>(size));
 }

 inline short DOSSegmentClass::Copy_Word_From(int source)
 {
	 return *(short*)(reinterpret_cast<void*>(Selector + source));
 }

 inline char DOSSegmentClass::Copy_Byte_From(int source)
 {
	 return *(char*)(reinterpret_cast<void*>(Selector + source));
 }

 inline long DOSSegmentClass::Copy_DWord_From(int source)
 {
	 return *(long*)(reinterpret_cast<void*>(Selector + source));
 }

 inline uintptr_t DOSSegmentClass::Get_Selector(void)
 {
	 return Selector;
 }

 #endif


