/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   Heap.h
 * Author: LTSACH
 *
 * Created on 22 August 2020, 18:18
 */

#ifndef HEAP_H
#define HEAP_H
#include <memory.h>
#include "heap/IHeap.h"
#include <iostream>
#include <sstream>
/*
 * function pointer: int (*comparator)(T& lhs, T& rhs)
 *      compares objects of type T given in lhs and rhs.
 *      return: sign of (lhs - rhs)
 *              -1: lhs < rhs
 *              0 : lhs == rhs
 *              +1: ls > rhs
 *
 * function pointer: void (*deleteUserData)(Heap<T>* pHeap)
 *      remove user's data in case that T is a pointer type
 *      Users should pass &Heap<T>::free for "deleteUserData"
 *
 */
template<class T>
class Heap: public IHeap<T>{
public:
    class Iterator; //forward declaration
    
protected:
    T *elements;    //a dynamic array to contain user's data
    int capacity;   //size of the dynamic array
    int count;      //current count of elements stored in this heap
    int (*comparator)(T& lhs, T& rhs);      //see above
    void (*deleteUserData)(Heap<T>* pHeap); //see above
    
public:
    Heap(   int (*comparator)(T& , T&)=0,
            void (*deleteUserData)(Heap<T>*)=0 );
    
    Heap(const Heap<T>& heap); //copy constructor
    Heap<T>& operator=(const Heap<T>& heap); //assignment operator
    
    ~Heap();
    
    //Inherit from IHeap: BEGIN
    void push(T item);
    T pop();
    const T peek();
    void remove(T item, void (*removeItemData)(T)=0);
    bool contains(T item);
    int size();
    void heapify(T array[], int size);
    void clear();
    bool empty();
    string toString(string (*item2str)(T&)=0 );
    //Inherit from IHeap: END
    
    void println(string (*item2str)(T&)=0 ){
        cout << toString(item2str) << endl;
    }
    
    Iterator begin(){
        return Iterator(this, true);
    }
    Iterator end(){
        return Iterator(this, false);
    }
    
public:
    /* if T is pointer type:
     *     pass the address of method "free" to Heap<T>'s constructor:
     *     to:  remove the user's data (if needed)
     * Example:
     *  Heap<Point*> heap(&Heap<Point*>::free);
     *  => Destructor will call free via function pointer "deleteUserData"
     */
    static void free(Heap<T> *pHeap){
        for(int idx=0; idx < pHeap->count; idx++) delete pHeap->elements[idx];
    }
    
    
private:
    bool aLTb(T& a, T& b){
        return compare(a, b) < 0;
    }
    int compare(T& a, T& b){
        if(comparator != 0) return comparator(a, b);
        else{
            if (a < b) return -1;
            else if(a > b) return 1;
            else return 0;
        }
    }
    
    void ensureCapacity(int minCapacity);
    void swap(int a, int b);
    void reheapUp(int position);
    void reheapDown(int position);
    int getItem(T item);
    
    void removeInternalData();
    void copyFrom(const Heap<T>& heap);
    
    
//////////////////////////////////////////////////////////////////////
////////////////////////  INNER CLASSES DEFNITION ////////////////////
//////////////////////////////////////////////////////////////////////
    
public:
    
    //Iterator: BEGIN
    class Iterator{
    private:
        Heap<T>* heap;
        int cursor;
    public:
        Iterator(Heap<T>* heap=0, bool begin=0){
            this->heap = heap;
            if(begin && (heap !=0)) cursor = 0;
            if(!begin && (heap !=0)) cursor = heap->size();
        }
        Iterator& operator=(const Iterator& iterator){
            this->heap = iterator.heap;
            this->cursor = iterator.cursor;
            return *this;
        }
        
        T& operator*(){
            return this->heap->elements[cursor];
        }
        bool operator!=(const Iterator& iterator){
            return this->cursor != iterator.cursor;
        }
        // Prefix ++ overload
        Iterator& operator++(){
            cursor++;
            return *this;
        }
        // Postfix ++ overload
        Iterator operator++(int){
            Iterator iterator = *this;
            ++*this;
            return iterator;
        }
        void remove(void (*removeItemData)(T)=0){
            this->heap->remove(this->heap->elements[cursor], removeItemData);
        }
    };
    //Iterator: END
};


//////////////////////////////////////////////////////////////////////
////////////////////////     METHOD DEFNITION      ///////////////////
//////////////////////////////////////////////////////////////////////


template<class T>
Heap<T>::Heap(
        int (*comparator)(T&, T&),
        void (*deleteUserData)(Heap<T>* ) )
    {

    this->capacity=10;
    this->count=0;
    this->comparator=comparator;
    this->deleteUserData=deleteUserData;
    this->elements = new T[capacity];
}

template<class T>
Heap<T>::Heap(const Heap<T>& heap) {
    copyFrom(heap); // Use the copyFrom function to handle deep copy
    deleteUserData = nullptr; // Reset deleteUserData to avoid double deletion
}

template<class T>
Heap<T>& Heap<T>::operator=(const Heap<T>& heap) {
    if (this != &heap) {
        removeInternalData(); // Clean up existing data
        copyFrom(heap); // Perform deep copy
        deleteUserData = nullptr; // Reset deleteUserData to avoid double deletion
    }
    return *this;
}

template<class T>
Heap<T>::~Heap() {
    removeInternalData(); // Ensure all allocated memory is cleaned up
}

template<class T>
void Heap<T>::push(T item) {
    ensureCapacity(count + 1); // Ensure capacity for new item
    elements[count] = item; // Insert item at the end
    reheapUp(count); // Reheap upwards to maintain heap property
    count++; // Increase element count
}

template<class T>
T Heap<T>::pop() {
    if (count==0) throw std::underflow_error("Calling to peek with the empty heap.");
    T root = elements[0]; // Store the root element to return
    elements[0] = elements[count - 1]; // Move last element to the root
    count--; // Decrease size
    reheapDown(0); // Reheap downwards to maintain heap property
    return root;
}

template<class T>
const T Heap<T>::peek() {
    if (count==0) throw std::underflow_error("Calling to peek with the empty heap.");
    return elements[0]; // Return the root element
}

template<class T>
void Heap<T>::remove(T item, void (*removeItemData)(T)) {
    int pos = getItem(item); // Get index of item
    if (pos == -1) return; // Item not found
if (removeItemData != nullptr) {
    removeItemData(elements[pos]);
}
    elements[pos] = elements[count - 1]; // Replace with last element
    count--; // Decrease size
    reheapDown(pos); // Restore heap property

}
template<class T>
bool Heap<T>::contains(T item) {
    return getItem(item) != -1; // Check if item exists in the heap
}

template<class T>
int Heap<T>::size() {
    return count; // Return the number of elements in the heap
}

template<class T>
void Heap<T>::heapify(T array[], int size) {
    clear(); // Reset heap
    ensureCapacity(size); // Ensure enough space for the array
    for (int i = 0; i < size; i++) {
        push(array[i]); // Insert each item and maintain heap property
    }
}

template<class T>
void Heap<T>::clear() {
    this->removeInternalData(); // Clean up existing data
    this->count=0;
    this->capacity=10;
    this->elements = new T[this->capacity];
}

template<class T>
bool Heap<T>::empty() {
    return count == 0; // Return true if no elements are present
}
template<class T>
string Heap<T>::toString(string (*item2str)(T&)){
    stringstream os;
    if(item2str != 0){
        os << "[";
        for(int idx=0; idx < count -1; idx++)
            os << item2str(elements[idx]) << ",";
        if(count > 0) os << item2str(elements[count - 1]);
        os << "]";
    }
    else{
        os << "[";
        for(int idx=0; idx < count -1; idx++)
            os << elements[idx] << ",";
        if(count > 0) os << elements[count - 1];
        os << "]";
    }
    return os.str();
}

template<class T>
void Heap<T>::ensureCapacity(int minCapacity){
    if(minCapacity >= capacity){
        //re-allocate
        int old_capacity = capacity;
        capacity = old_capacity + (old_capacity >> 2);
        try{
            T* new_data = new T[capacity];
            //OLD: memcpy(new_data, elements, capacity*sizeof(T));
            memcpy(new_data, elements, old_capacity*sizeof(T));
            delete []elements;
            elements = new_data;
        }
        catch(std::bad_alloc e){
            e.what();
        }
    }
}

template<class T>
void Heap<T>::swap(int a, int b){
    T temp = this->elements[a];
    this->elements[a] = this->elements[b];
    this->elements[b] = temp;
}

template<class T>
void Heap<T>::reheapUp(int position) {
    while (position > 0) {
        int parent = (position - 1) / 2; // Calculate parent index
        if (parent >= 0) {
            if (!aLTb(elements[parent], elements[position])) {
                swap(parent, position); // Swap if the child is larger than the parent
                position = parent; // Move up to the parent position
            } else {
                break; // Stop when heap property is satisfied
            }
        } else {
            break;
        }
    }
}


template<class T>
void Heap<T>::reheapDown(int position) {
    int leftChild, rightChild, largerChild;
    while ((leftChild = 2 * position + 1) < count) {
        rightChild = leftChild + 1;
        largerChild = (leftChild < count && aLTb(elements[leftChild], elements[rightChild])) ? leftChild : rightChild;
        if (!aLTb(elements[position], elements[largerChild])) {
            swap(position, largerChild); // Swap if parent is smaller than the larger child
            position = largerChild; // Move down to the child position
        } else {
            break; // Stop when heap property is satisfied
        }
    }
}
template<class T>
int Heap<T>::getItem(T item) {
    for (int i = 0; i < count; i++) {
        if (compare(elements[i], item) == 0) return i; // Return index if item is found
    }
    return -1; // Return -1 if item is not found
}

template<class T>
void Heap<T>::removeInternalData(){
    if(this->deleteUserData != 0) deleteUserData(this); //clear users's data if they want
    delete []elements;
}

template<class T>
void Heap<T>::copyFrom(const Heap<T>& heap){
    capacity = heap.capacity;
    count = heap.count;
    elements = new T[capacity];
    this->comparator = heap.comparator;
    this->deleteUserData = heap.deleteUserData;
    
    //Copy items from heap:
    for(int idx=0; idx < heap.count; idx++){
        this->elements[idx] = heap.elements[idx];
    }
}

#endif /* HEAP_H */

