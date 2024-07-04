#ifndef DISTRIBUTEDLOCK_COMMON_H
#define DISTRIBUTEDLOCK_COMMON_H
#include <chrono>
using day = std::chrono::duration<size_t, std::ratio<24 * 60 * 60>>;
using hour = std::chrono::duration<size_t, std::ratio<60 * 60>>;
using minute = std::chrono::duration<size_t, std::ratio<60>>;
using second = std::chrono::duration<size_t>;
using milli_second = std::chrono::duration<size_t, std::milli>;
using micro_second = std::chrono::duration<size_t, std::micro>;
using nano_second = std::chrono::duration<size_t, std::nano>;


#endif //DISTRIBUTEDLOCK_COMMON_H
