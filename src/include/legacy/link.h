#pragma once

// Simple doubly linked list node that handles insertion and removal from chains.
class LinkClass {
 public:
  LinkClass();
  LinkClass(LinkClass& link);
  virtual ~LinkClass();

  LinkClass& operator=(LinkClass& link);

  [[nodiscard]] virtual LinkClass* Get_Next() const;
  [[nodiscard]] virtual LinkClass* Get_Prev() const;
  virtual LinkClass& Add(LinkClass& object);
  virtual LinkClass& Add_Tail(LinkClass& object);
  virtual LinkClass& Add_Head(LinkClass& object);

  [[nodiscard]] virtual LinkClass const& Head_Of_List() const;
  virtual LinkClass& Head_Of_List() {
    return const_cast<LinkClass&>(
        static_cast<LinkClass const&>(*this).Head_Of_List());
  }

  [[nodiscard]] virtual LinkClass const& Tail_Of_List() const;
  virtual LinkClass& Tail_Of_List() {
    return const_cast<LinkClass&>(
        static_cast<LinkClass const&>(*this).Tail_Of_List());
  }

  virtual void Zap();
  virtual LinkClass* Remove();

 private:
  LinkClass* Next = nullptr;
  LinkClass* Prev = nullptr;
};
