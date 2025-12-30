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

/* $Header:   F:\projects\c&c\vcs\code\packet.cpv   1.0   24 Apr 1996 08:21:58   PWG  $ */
/***************************************************************************
 *                                                                         *
 *                 Project Name : Westwood Auto Registration App           *
 *                                                                         *
 *                    File Name : PACKET.CPP                               *
 *                                                                         *
 *                   Programmer : Philip W. Gorrow                         *
 *                                                                         *
 *-------------------------------------------------------------------------*
 * Member functions for PacketClass.                                       *
 *-------------------------------------------------------------------------*/

#include "legacy/packet.h"

#include <algorithm>
#include <cstring>

#if defined(_WIN32)
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

PacketClass::~PacketClass(void) {
	FieldClass* next = nullptr;
	for (FieldClass* current = Head; current; current = next) {
		next = current->Next;
		delete current;
	}
}

void PacketClass::Add_Field(FieldClass* field) {
	field->Next = Head;
	Head = field;
}

PacketClass::PacketClass(char* curbuf) {
	Size = *reinterpret_cast<unsigned short*>(curbuf);
	curbuf += sizeof(unsigned short);
	Size = ntohs(Size);

	ID = *reinterpret_cast<short*>(curbuf);
	curbuf += sizeof(short);
	ID = ntohs(ID);

	Head = nullptr;
	Current = nullptr;

	int remaining_size = Size - 4;
	while (remaining_size > 0) {
		FieldClass* field = new FieldClass;

		std::memcpy(field, curbuf, FIELD_HEADER_SIZE);
		curbuf += FIELD_HEADER_SIZE;
		remaining_size -= FIELD_HEADER_SIZE;

		const int size = ntohs(field->Size);
		field->Data = new char[size];
		std::memcpy(field->Data, curbuf, static_cast<std::size_t>(size));
		curbuf += size;
		remaining_size -= size;

		const int pad = (4 - (ntohs(field->Size) & 3)) & 3;
		curbuf += pad;
		remaining_size -= pad;

		field->Net_To_Host();
		Add_Field(field);
	}
}

char* PacketClass::Create_Comms_Packet(int& size) {
	FieldClass* current;

	size = 4;
	for (current = Head; current; current = current->Next) {
		size += static_cast<unsigned short>(FIELD_HEADER_SIZE);
		size += current->Size;
		size += (4 - (size & 3)) & 3;
	}

	char* retval = new char[size];
	char* curbuf = retval;

	*reinterpret_cast<unsigned short*>(curbuf) =
	    static_cast<unsigned short>(htons(static_cast<unsigned short>(size)));
	curbuf += sizeof(unsigned short);
	*reinterpret_cast<short*>(curbuf) = htons(ID);
	curbuf += sizeof(short);

	for (current = Head; current; current = current->Next) {
		current->Host_To_Net();

		std::memcpy(curbuf, current, FIELD_HEADER_SIZE);
		curbuf += FIELD_HEADER_SIZE;

		const int payload = ntohs(current->Size);
		std::memcpy(curbuf, current->Data, static_cast<std::size_t>(payload));
		curbuf += payload;

		const int pad = (4 - (ntohs(current->Size) & 3)) & 3;
		if (pad) {
			std::memset(curbuf, 0, static_cast<std::size_t>(pad));
			curbuf += pad;
		}

		current->Net_To_Host();
	}

	return retval;
}

FieldClass* PacketClass::Find_Field(char* id) {
	for (FieldClass* current = Head; current; current = current->Next) {
		if (std::strncmp(id, current->ID, 4) == 0) {
			return current;
		}
	}
	return nullptr;
}

bool PacketClass::Get_Field(char* id, char& data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		data = *static_cast<char*>(field->Data);
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, unsigned char& data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		data = *static_cast<unsigned char*>(field->Data);
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, short& data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		data = *static_cast<short*>(field->Data);
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, unsigned short& data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		data = *static_cast<unsigned short*>(field->Data);
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, long& data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		data = *static_cast<long*>(field->Data);
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, char* data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		std::strcpy(data, static_cast<char*>(field->Data));
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, unsigned long& data) {
	FieldClass* field = Find_Field(id);
	if (field) {
		data = *static_cast<unsigned long*>(field->Data);
	}
	return field != nullptr;
}

bool PacketClass::Get_Field(char* id, void* data, int& length) {
	FieldClass* field = Find_Field(id);
	if (field) {
		const int copy_len = std::min(static_cast<int>(field->Size), length);
		std::memcpy(data, field->Data, static_cast<std::size_t>(copy_len));
		length = static_cast<int>(field->Size);
	}
	return field != nullptr;
}

