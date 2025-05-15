#pragma once

#include <cassert>
#include <iostream>

#ifndef __JW_TYPE_DEFINITION_H__
#define __JW_TYPE_DEFINITION_H__

#define	WIDEN2(x)		L##x
#define	WIDEN(x)		WIDEN2(x)
#define	__WFILE__		WIDEN(__FILE__)

//#define STRLEN(x)       strlen(x)
#define STRLEN(x)       wcslen(x)


//#define STRNCPY(DEST, NUM, SOURCE, COUNT)       strncpy_s(DEST, NUM, SOURCE, COUNT)
#define STRNCPY(DEST, NUM, SOURCE, COUNT)       wcsncpy_s(DEST, NUM, SOURCE, COUNT)

#define	countof(A)		(sizeof(A)/sizeof((A)[0]))

#define ASSERT_WITH_MSG(CONDITION, MSG) \
    do { \
        if (!(CONDITION)) { \
            std::wcerr << L"Assertion failed: " << (MSG) << std::endl; \
            assert(CONDITION); \
        } \
    } while (false)

#define WRITE_LOCK(mtx) std::unique_lock<std::shared_mutex> lk(mtx)
#define READ_LOCK(mtx) std::shared_lock<std::shared_mutex> lk(mtx)

#define MAKE_CRASH { void * p = nullptr; *((int*)p) = 10; }

#define JW_MIN(r, l) r < l ? r : l;
#define JW_MAX(r, l) r > l ? r : l;

static constexpr int64_t INVALID_VALUE = 0;

#endif