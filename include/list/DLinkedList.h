/*
 * File:   DLinkedList.h
 */

#ifndef DLINKEDLIST_H
#define DLINKEDLIST_H

#include "list/IList.h"

#include <sstream>
#include <iostream>
#include <type_traits>
using namespace std;

template <class T>
class DLinkedList : public IList<T>
{
public:
    class Node;        // Forward declaration
    class Iterator;    // Forward declaration
    class BWDIterator; // Forward declaration

protected:
    Node *head; // this node does not contain user's data
    Node *tail; // this node does not contain user's data
    int count;
    bool (*itemEqual)(T &lhs, T &rhs);        // function pointer: test if two items (type: T&) are equal or not
    void (*deleteUserData)(DLinkedList<T> *); // function pointer: be called to remove items (if they are pointer type)

public:
    DLinkedList(
        void (*deleteUserData)(DLinkedList<T> *) = 0,
        bool (*itemEqual)(T &, T &) = 0);
    DLinkedList(const DLinkedList<T> &list);
    DLinkedList<T> &operator=(const DLinkedList<T> &list);
    ~DLinkedList();

    // Inherit from IList: BEGIN
    void add(T e);
    void add(int index, T e);
    T removeAt(int index);
    bool removeItem(T item, void (*removeItemData)(T) = 0);
    bool empty();
    int size();
    void clear();
    T &get(int index);
    int indexOf(T item);
    bool contains(T item);
    string toString(string (*item2str)(T &) = 0);
    // Inherit from IList: END

    void println(string (*item2str)(T &) = 0)
    {
        cout << toString(item2str) << endl;
    }
    void setDeleteUserDataPtr(void (*deleteUserData)(DLinkedList<T> *) = 0)
    {
        this->deleteUserData = deleteUserData;
    }

    bool contains(T array[], int size)
    {
        int index = 0;
        for (DLinkedList<T>::Iterator it = begin(); it != end(); it++)
        {
            if (!equals(*it, array[index++], this->itemEqual))
                return false;
        }
        return true;
    }

    /*
     * free(DLinkedList<T> *list):
     *  + to remove user's data (type T, must be a pointer type, e.g.: int*, Point*)
     *  + if users want a DLinkedList removing their data,
     *      he/she must pass "free" to constructor of DLinkedList
     *      Example:
     *      DLinkedList<T> list(&DLinkedList<T>::free);
     */
    static void free(DLinkedList<T> *list)
    {
        typename DLinkedList<T>::Iterator it = list->begin();
        while (it != list->end())
        {
            delete *it;
            it++;
        }
    }

    /* begin, end and Iterator helps user to traverse a list forwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::Iterator it;
     for(it = list.begin(); it != list.end(); it++){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    Iterator begin()
    {
        return Iterator(this, true);
    }
    Iterator end()
    {
        return Iterator(this, false);
    }

    /* last, beforeFirst and BWDIterator helps user to traverse a list backwardly
     * Example: assume "list" is object of DLinkedList

     DLinkedList<char>::BWDIterator it;
     for(it = list.last(); it != list.beforeFirst(); it--){
            char item = *it;
            std::cout << item; //print the item
     }
     */
    BWDIterator bbegin()
    {
        return BWDIterator(this, true);
    }
    BWDIterator bend()
    {
        return BWDIterator(this, false);
    }

protected:
    static bool equals(T &lhs, T &rhs, bool (*itemEqual)(T &, T &))
    {
        if (itemEqual != 0)
            return itemEqual(lhs, rhs);
        if constexpr (DLinkedList::checkEqual::value)
            return lhs == rhs;
        return false;
    }
    void copyFrom(const DLinkedList<T> &list);
    void removeInternalData();
    Node *getPreviousNodeOf(int index);

    //////////////////////////////////////////////////////////////////////
    ////////////////////////  INNER CLASSES DEFNITION ////////////////////
    //////////////////////////////////////////////////////////////////////
public:
    class Node
    {
    public:
        T data;
        Node *next;
        Node *prev;
        friend class DLinkedList<T>;

    public:
        Node(Node *next = 0, Node *prev = 0)
        {
            this->next = next;
            this->prev = prev;
        }
        Node(T data, Node *next = 0, Node *prev = 0)
        {
            this->data = data;
            this->next = next;
            this->prev = prev;
        }
    };

    //////////////////////////////////////////////////////////////////////
    class Iterator
    {
    private:
        DLinkedList<T> *pList;
        Node *pNode;

    public:
        Iterator(DLinkedList<T> *pList = 0, bool begin = true)
        {
            if (begin)
            {
                if (pList != 0)
                    this->pNode = pList->head->next;
                else
                    pNode = 0;
            }
            else
            {
                if (pList != 0)
                    this->pNode = pList->tail;
                else
                    pNode = 0;
            }
            this->pList = pList;
        }

        Iterator &operator=(const Iterator &iterator)
        {
            this->pNode = iterator.pNode;
            this->pList = iterator.pList;
            return *this;
        }
        void remove(void (*removeItemData)(T) = 0)
        {
            pNode->prev->next = pNode->next;
            pNode->next->prev = pNode->prev;
            Node *pNext = pNode->prev; // MUST prev, so iterator++ will go to end
            if (removeItemData != 0)
                removeItemData(pNode->data);
            delete pNode;
            pNode = pNext;
            pList->count -= 1;
        }

        T &operator*()
        {
            return pNode->data;
        }
        bool operator!=(const Iterator &iterator)
        {
            return pNode != iterator.pNode;
        }
        // Prefix ++ overload
        Iterator &operator++()
        {
            pNode = pNode->next;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int)
        {
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
    };
class BWDIterator
{
private:
    DLinkedList<T> *pList;
    Node *pNode;
    public:
    BWDIterator(DLinkedList<T> *pList = 0, bool end = true)
      {
        if (end)
        {
          if (pList != 0)
            this->pNode = pList->tail->prev;
          else
            pNode = 0;
        }
        else
        {
          if (pList != 0)
            this->pNode = pList->head;
          else
            pNode = 0;
        }
        this->pList = pList;
      }
      
      BWDIterator &operator=(const BWDIterator &iterator)
      {
        this->pNode = iterator.pNode;
        this->pList = iterator.pList;
        return *this;
      }

      void remove(void (*removeItemData)(T) = 0)
      {
        pNode->prev->next = pNode->next;
        pNode->next->prev = pNode->prev;
        Node *pPrev = pNode->next; // MUST next, so iterator-- will go to begin
        if (removeItemData != 0)
          removeItemData(pNode->data);
        delete pNode;
        pNode = pPrev;
        pList->count -= 1;
      }

      T &operator*()
      {
        return pNode->data;
      }

      bool operator!=(const BWDIterator &iterator)
      {
        return pNode != iterator.pNode;
      }
      BWDIterator &operator--()
      {
          if (pNode != nullptr) {
              pNode = pNode->prev;
          }
        return *this;
      }
      BWDIterator operator--(int)
      {
        BWDIterator iterator = *this;
        --*this;
        return iterator;
      }
      BWDIterator &operator++()
      {
          if (pNode != nullptr) {
              pNode = pNode->prev;
          }
        return *this;
      }
      BWDIterator operator++(int)
      {
        BWDIterator iterator = *this;
        ++*this;
        return iterator;
      }
    };
  private:
      class checkEqual {
      private:
          template <typename U>
          static auto eq_op(int) -> decltype(declval<U>() == declval<U>(), true_type());
          template <typename>
          static false_type eq_op(...);
      public:
          static constexpr bool value = decltype(eq_op<T>(0))::value;
      };
      class checkToString {
      private:
          template <typename U>
          static auto os_op(int) -> decltype(declval<ostream&>() << declval<U>(), true_type());
          template <typename>
          static false_type os_op(...);
      public:
          static constexpr bool value = decltype(os_op<T>(0))::value;
      };

};
//////////////////////////////////////////////////////////////////////
// Define a shorter name for DLinkedList:

template <class T>
using List = DLinkedList<T>;

//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////

template <class T>
DLinkedList<T>::DLinkedList(
    void (*deleteUserData)(DLinkedList<T> *),
    bool (*itemEqual)(T &, T &))
{
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    this->count = 0;
    this->itemEqual = itemEqual;
    this->deleteUserData = deleteUserData;
    // TODO
}

template <class T>
DLinkedList<T>::DLinkedList(const DLinkedList<T> &list)
{
    this->head = new Node();
    this->tail = new Node();
    this->head->next = this->tail;
    this->tail->prev = this->head;
    copyFrom(list);
    // TODO
}

template <class T>
DLinkedList<T> &DLinkedList<T>::operator=(const DLinkedList<T> &list)
{
    if (this != &list)
    {
        removeInternalData();
        copyFrom(list);
    }
    return *this; 
    // TODO
}

template <class T>
DLinkedList<T>::~DLinkedList()
{
    removeInternalData();
    delete head;
    delete tail;
    // TODO
}

template <class T>
void DLinkedList<T>::add(T e)
{
    Node *newNode = new Node(e);
    tail->prev->next = newNode;
    newNode->prev = tail->prev;
    newNode->next = tail;
    tail->prev = newNode;
    count++;
    // TODO
}
template <class T>
void DLinkedList<T>::add(int index, T e)
{
    if (index < 0 || index > count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    Node *newNode = new Node(e);
    Node *prevNode = getPreviousNodeOf(index);
    
    newNode->next = prevNode->next;
    newNode->prev = prevNode;
    prevNode->next->prev = newNode;
    prevNode->next = newNode;
    count++;
    // TODO
}

template <class T>
typename DLinkedList<T>::Node *DLinkedList<T>::getPreviousNodeOf(int index)
{
    
    /**
     * Returns the node preceding the specified index in the doubly linked list.
     * If the index is in the first half of the list, it traverses from the head; otherwise, it traverses from the tail.
     * Efficiently navigates to the node by choosing the shorter path based on the index's position.
     */
    if (index <= count / 2)
    {
        Node *curr = head;
        for (int i = 0; i < index; i++)
        {
            curr = curr->next;
        }
        return curr;
    }
    else
    {
        Node *curr = tail->prev;
        for (int i = count; i > index; i--)
        {
            curr = curr->prev;
        }
        return curr;
    }
    // TODO
}

template <class T>
T DLinkedList<T>::removeAt(int index)
{
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    Node *prevNode = getPreviousNodeOf(index);
    Node *delNode = prevNode->next;
    T delData = delNode->data;
    prevNode->next = delNode->next;
    delNode->next->prev = prevNode;
    delete delNode;
    count--;
    return delData;
    // TODO
}

template <class T>
bool DLinkedList<T>::empty()
{
    return count == 0;
    // TODO
}

template <class T>
int DLinkedList<T>::size()
{
    return count;
    // TODO
}

template <class T>
void DLinkedList<T>::clear()
{
    removeInternalData();
    // TODO
}

template <class T>
T &DLinkedList<T>::get(int index)
{
    if (index < 0 || index >= count)
    {
        throw std::out_of_range("Index is out of range!");
    }

    Node *curr = getPreviousNodeOf(index);
    return curr->next->data;
    // TODO
}

template <class T>
int DLinkedList<T>::indexOf(T item)
{
    Node *curr = head->next;
    for (int i = 0; i < count; i++)
    {
        if (equals(curr->data, item, itemEqual))
        {
            return i;
        }
        curr = curr->next;
    }
    return -1;
    // TODO
}

template <class T>
bool DLinkedList<T>::removeItem(T item, void (*removeItemData)(T))
{
    int index = indexOf(item);
    if (index == -1)
    {
        return false;
    }

    T data = removeAt(index);
    if (removeItemData != 0) {
        removeItemData(data);
    }
    return true;
    // TODO
}

template <class T>
bool DLinkedList<T>::contains(T item)
{
    return indexOf(item) != -1;
    // TODO
}

template <class T>
string DLinkedList<T>::toString(string (*item2str)(T &))
{
    /**
     * Converts the list into a string representation, where each element is formatted using a user-provided function.
     * If no custom function is provided, it directly uses the element's default string representation.
     * Example: If the list contains {1, 2, 3} and the provided function formats integers, calling toString would return "[1, 2, 3]".
     *
     * @param item2str A function that converts an item of type T to a string. If null, default to string conversion of T.
     * @return A string representation of the list with elements separated by commas and enclosed in square brackets.
     */
    stringstream ss;
    ss << "[";
    Node *curr = head->next;
    for (int i = 0; i < count; i++)
    {
        if (item2str != 0)
        {
            ss << item2str(curr->data);
        }
        else if constexpr (DLinkedList::checkToString::value)
        {
            ss << curr->data;
        }
        if (i < count - 1)
        {
            ss << ", ";
        }
        curr = curr->next;
    }
    ss << "]";
    return ss.str();
    // TODO
}

template <class T>
void DLinkedList<T>::copyFrom(const DLinkedList<T> &list)
{
    /**
     * Copies the contents of another doubly linked list into this list.
     * Initializes the current list to an empty state and then duplicates all data and pointers from the source list.
     * Iterates through the source list and adds each element, preserving the order of the nodes.
     */
    Node *curr = list.head->next;
    for (int i = 0; i < list.count; i++)
    {
        add(curr->data);
        curr = curr->next;
    }
    this->count = list.count;
    this->itemEqual = list.itemEqual;
    this->deleteUserData = list.deleteUserData;
    // TODO
}

template <class T>
void DLinkedList<T>::removeInternalData()
{
    /**
     * Clears the internal data of the list by deleting all nodes and user-defined data.
     * If a custom deletion function is provided, it is used to free the user's data stored in the nodes.
     * Traverses and deletes each node between the head and tail to release memory.
     */
    if (deleteUserData != 0) {
        deleteUserData(this);
    }
    
    Node *curr = head->next;
    Node *nextNode;
    for (int i = 0; i < count; i++)
    {
        nextNode = curr->next;
        delete curr;
        curr = nextNode;
    }
    head->next = tail;
    tail->prev = head;
    count = 0;
    // TODO
}

#endif /* DLINKEDLIST_H */
