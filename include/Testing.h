#pragma once

#include "Macros.h"

//Function pointer with no arguments returning nothing
typedef void(*fptr)();

//This function checks if `v` is 0 and if so, outputs the provided
//message in printf style (for C compatibility) to an
//implementation-defined output (as not every application will support
//output to stderr).
void __checkTestEq__(int v, const char isFatal, const char* failFmt, ...);

/**
 * @brief Registers a function to call at runtime
 * that contains test(s).
 * 
 * @param f 
 */
void __registerTestFunction__(fptr f);

#ifdef DO_TEST
/**
 * @brief Generate a function that runs tests.
 * You should only write it at the end of a source file
 * (and definitely NOT in headers) because if a build is not
 * compiled for testing it'll disable "-Wunused-variable"
 * and "-Wunused-but-set-variable" warnings
 * right before every instance of this macro to not create
 * noise in the terminal.
 */
#define TEST(name) \
void __test__##name(); \
BeforeMain(__register__##name) { __registerTestFunction__(__test__##name); } \
void __test__##name()

#define EXPECT_EQ(x, y) do { \
    int v = (x) == (y); \
    __checkTestEq__(v, 0, v ? "" : "Test for equality failed in " __FILE__ "/%d", __LINE__); \
} while(0)

#define EXPECT_NEQ(x, y) do { \
    int v = (x) != (y); \
    __checkTestEq__(v, 0, v ? "" : "Test for inequality failed in " __FILE__ "/%d", __LINE__); \
} while(0)

#define EXPECT_EQ_MSG(x, y, failMsg, ...) do { \
    int v = (x) == (y); \
    __checkTestEq__(v, 0, v ? "" : "Test for equality failed in " __FILE__ "/%d: " failMsg, __LINE__, ##__VA_ARGS__); \
} while(0)

#define EXPECT_NEQ_MSG(x, y, failMsg, ...) do { \
    int v = (x) != (y); \
    __checkTestEq__(v, 0, v ? "" : "Test for equality failed in " __FILE__ "/%d: " failMsg, __LINE__, ##__VA_ARGS__); \
} while(0)



#define ASSERT_EQ(x, y) do { \
    int v = (x) == (y); \
    __checkTestEq__(v, 1, v ? "" : "Test for equality failed in " __FILE__ "/%d", __LINE__); \
} while(0)

#define ASSERT_NEQ(x, y) do { \
    int v = (x) != (y); \
    __checkTestEq__(v, 1, v ? "" : "Test for inequality failed in " __FILE__ "/%d", __LINE__); \
} while(0)

#define ASSERT_EQ_MSG(x, y, failMsg, ...) do { \
    int v = (x) == (y); \
    __checkTestEq__(v, 1, v ? "" : "Test for equality failed in " __FILE__ "/%d: " failMsg, __LINE__, ##__VA_ARGS__); \
} while(0)

#define ASSERT_NEQ_MSG(x, y, failMsg, ...) do { \
    int v = (x) != (y); \
    __checkTestEq__(v, 1, v ? "" : "Test for equality failed in " __FILE__ "/%d: " failMsg, __LINE__, ##__VA_ARGS__); \
} while(0)
#else
/**
 * @brief Generate a function that runs tests.
 * You should only write it at the end of a source file
 * (and definitely NOT in headers) because if a build is not
 * compiled for testing it'll disable "-Wunused-variable"
 * and "-Wunused-but-set-variable" warnings
 * right before every instance of this macro to not create
 * noise in the terminal.
 */
#define TEST(name) \
_Pragma("GCC diagnostic push") \
_Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
_Pragma("GCC diagnostic ignored \"-Wunused-but-set-variable\"") \
void __test__##name()
#define EXPECT_EQ(x, y)
#define EXPECT_NEQ(x, y)
#define EXPECT_EQ_MSG(x, y, failMsg, ...)
#define EXPECT_NEQ_MSG(x, y, failMsg, ...)

#define ASSERT_EQ(x, y)
#define ASSERT_NEQ(x, y)
#define ASSERT_EQ_MSG(x, y, failMsg, ...)
#define ASSERT_NEQ_MSG(x, y, failMsg, ...)
#endif