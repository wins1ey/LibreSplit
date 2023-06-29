#ifndef AUTOSPLITTER_HPP
#define AUTOSPLITTER_HPP

#include <stdatomic.h>

extern atomic_bool usingAutoSplitter;
extern atomic_bool callStart;
extern atomic_bool callSplit;
extern atomic_bool callIsLoading;
extern atomic_bool callReset;

#endif