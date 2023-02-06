#ifndef STASE_PTR_VEC_H
#define STASE_PTR_VEC_H

#include <iostream>

template <typename T>
class ptr_vec {

private:
    const short unsigned n;
    short unsigned i;
    T * ptr;

public:
    ptr_vec(T * ptr, const short unsigned n) : n(n), i(0), ptr(ptr) {}

    inline void push(T elem) {
#ifdef SAFE_STACK_VEC
        if (i >= n) {
            exit_with_failure("pushing to full stack");
        }
#endif
        ptr[i++] = elem;
    }

    inline T pop() {
#ifdef SAFE_STACK_VEC
        if (i == 0) {
            exit_with_failure("popping from empty stack");
        }
#endif
        return ptr[--i];
    }

    inline short unsigned size() const {
        return i;
    }

    inline T operator[] (short unsigned j) const {
#ifdef SAFE_STACK_VEC
        if (j >= n) {
            exit_with_failure("accessing " + std::to_string(j) + " at " + std::to_string(i) + "/" + std::to_string(n));
        }
#endif
        return ptr[j];
    }

    inline T & operator[] (short unsigned j) {
#ifdef SAFE_STACK_VEC
        if (j >= n) {
            exit_with_failure("assigning " + std::to_string(j) + " at " + std::to_string(i) + "/" + std::to_string(n));
        }
#endif
        return ptr[j];
    }

private:
    inline void exit_with_failure(const std::string message) const {
        std::cout << "*****Illegal ptr_vec usage: " + message + "*****\n";
        abort();
    }
};

#endif //STASE_PTR_VEC_H
