#pragma once

// Deletes the copy constructor, the copy assignment operator, the move constructor, and the move assignment operator.
#define DELETE_COPY_AND_MOVE(T)       \
    T(const T &) = delete;            \
    T &operator=(const T &) = delete; \
    T(T &&) = delete;                 \
    T &operator=(T &&) = delete;
