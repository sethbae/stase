#ifndef STASE_PTR_VEC_H
#define STASE_PTR_VEC_H

#include "utils.h"

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
            exit_with_failure();
        }
#endif
        ptr[i++] = elem;
    }

    inline T pop() {
#ifdef SAFE_STACK_VEC
        if (i <= 0) {
            exit_with_failure();
        }
#endif
        return ptr[--i];
    }

    inline short unsigned size() const {
        return i;
    }

    inline T operator[] (short unsigned j) const {
#ifdef SAFE_STACK_VEC
        if (j < 0 || j >= n) {
            exit_with_failure();
        }
#endif
        return ptr[j];
    }

    inline T & operator[] (short unsigned j) {
#ifdef SAFE_STACK_VEC
        if (j < 0 || j >= n) {
            exit_with_failure();
        }
#endif
        return ptr[j];
    }

private:
    inline void exit_with_failure() const {
        std::cout << "*****Illegal ptr_vec usage: aborting\n";
        print_stack_trace_and_abort(0);
    }
};

#endif //STASE_PTR_VEC_H
