#include "link.h"

LinkClass::LinkClass() { Zap(); }

LinkClass::LinkClass(LinkClass& link) {
  Zap();
  Add(link);
}

LinkClass::~LinkClass() { Remove(); }

void LinkClass::Zap() {
  Next = nullptr;
  Prev = nullptr;
}

LinkClass& LinkClass::operator=(LinkClass& link) {
  Remove();
  Add(link);
  return link;
}

LinkClass* LinkClass::Get_Next() const { return Next; }

LinkClass* LinkClass::Get_Prev() const { return Prev; }

LinkClass const& LinkClass::Head_Of_List() const {
  LinkClass const* link = this;
  while (link->Prev) {
    link = link->Prev;
    if (link == this) {
      break;
    }
  }
  return *link;
}

LinkClass const& LinkClass::Tail_Of_List() const {
  LinkClass const* link = this;
  while (link->Next) {
    link = link->Next;
    if (link == this) {
      break;
    }
  }
  return *link;
}

LinkClass& LinkClass::Add(LinkClass& list) {
  LinkClass* ptr = list.Next;
  list.Next = this;
  Prev = &list;
  Next = ptr;
  if (ptr) {
    ptr->Prev = this;
  }
  return Head_Of_List();
}

LinkClass& LinkClass::Add_Head(LinkClass& list) {
  LinkClass* ptr = &list.Head_Of_List();
  ptr->Prev = this;
  Next = ptr;
  Prev = nullptr;
  return *this;
}

LinkClass& LinkClass::Add_Tail(LinkClass& list) {
  LinkClass* ptr = &list.Tail_Of_List();
  ptr->Next = this;
  Prev = ptr;
  Next = nullptr;
  return Head_Of_List();
}

LinkClass* LinkClass::Remove() {
  LinkClass* head = &Head_Of_List();
  LinkClass* tail = &Tail_Of_List();

  if (Prev) {
    Prev->Next = Next;
  }
  if (Next) {
    Next->Prev = Prev;
  }
  Prev = nullptr;
  Next = nullptr;

  if (head == this) {
    if (tail == this) {
      return nullptr;
    }
    return &tail->Head_Of_List();
  }
  return head;
}
