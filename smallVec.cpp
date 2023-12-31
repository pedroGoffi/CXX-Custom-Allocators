#ifndef SMALLVEC_CPP
#define SMALLVEC_CPP
#include "../include/smallVec.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
using namespace Pietra;

#define __SVEC_BLOCK_SIZE(_type) sizeof(_type) * 1024

template<typename T>
Core::SVecHdr* Core::SVec<T>::get_head(){    
    return ((Core::SVecHdr*)((char*)(this->data) - offsetof(Core::SVecHdr, buff)));
}

template<typename T>
int Core::SVec<T>::len(){    
    return this->data? this->get_head()->len: 0;
}

template<typename T>
int Core::SVec<T>::cap(){    
    return this->data? this->get_head()->cap: 0;
}

template<typename T>
T* Core::SVec<T>::end(){    
    return (T*)(this->data + this->len());
}

template<typename T>
void Core::SVec<T>::make_fit(int size){    
    if(!this->check_if_fits(size)){
        this->__grow_buffer__(size);
    }
}

template<typename T>
bool Core::SVec<T>::check_if_fits(int size){    
    return size <= this->cap();
}
template<typename T>
void Core::SVec<T>::free(){
    if(this->data){
        std::free(this->get_head());
    }

    this->get_head()->len = 0;
    this->get_head()->cap = 0;    
}
template<typename T>
void Core::SVec<T>::reset(){
    this->get_head()->len = 0;
}
template<typename T>
void Core::SVec<T>::__grow_buffer__(int new_len) {
    assert(this->cap() <= (SIZE_MAX - 1)/2);
    
    size_t new_cap = CLAMP_MIN(2*this->cap(), MAX(new_len, 16));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(Core::SVecHdr, buff))/sizeof(T));
    size_t new_size = MAX(offsetof(Core::SVecHdr, buff) + new_cap*sizeof(T), 1);
    SVecHdr *new_hdr;
    if (this->data) {
        new_hdr = (SVecHdr *)std::realloc((void*)this->get_head(), new_size);
        assert(new_hdr);
    } else {
        new_hdr = (SVecHdr *)std::malloc(new_size);
        assert(new_hdr);
        new_hdr->len = 0;
    }
    
    new_hdr->cap = new_cap;
    *(char**)&this->data = new_hdr->buff;    
}
template<typename T>
T* Core::SVec<T>::begin(){
    return this->data;
}

template<typename T>
void Core::SVec<T>::push(T item){
    this->make_fit(__SVEC_BLOCK_SIZE(T));
    this->data[this->len()] = item;
    this->get_head()->len++;
}
template<typename T>
T Core::SVec<T>::pop(){
    assert(this->len() > 0);            
    
    T item = this->data[this->len() - 1];
    *(std::nullptr_t*)this->data[this->len() - 1] = nullptr;
    this->get_head()->len -= 1;
    
    printf("poping: %i: %i\n", 
        this->get_head()->len,
        this->len());
    return item;
}
template<typename T>
T& Core::SVec<T>::find(T item){
    for(T it: *this){
        if(it == item) return it;        
    }

    return nullptr;
}
template<typename T>
T& Core::SVec<T>::at(int index){
    if(index > this->len()){
        printf("[ERR]: SVec.at | index(%i) > lenght(%i)\n", index, this->len());
        exit(1);
    }
    return this->data[index];
}
template<typename T>
T& Core::SVec<T>::back(){
    if(this->len() == 0){
        printf("[ERR]: SVec.back | size == 0\n");
        exit(1);
    }
    return this->at(this->len() - 1);
  
}
template<typename T>
T& Core::SVec<T>::next(){
    return this->at(this->id++);
}


#endif /*SMALLVEC_CPP*/