#ifndef ASSERT_H
#define ASSERT_H

extern void __abort(const char * function, const char * filename, const char * line);

#if defined(NDEBUG)
#define assert(expression) (0)
#else
#define assert(expression) ((expression) ? 0: __abort(__func__, __FILE__, __LINE__))
#endif

#endif
