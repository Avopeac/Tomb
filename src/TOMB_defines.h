#pragma once

#define TOMB_Main main

#ifndef TOMB_SUCCESS
#define TOMB_SUCCESS 0
#endif

#ifndef TOMB_FAILURE
#define TOMB_FAILURE 1
#endif

#ifndef TOMB_CHECK_RESULT
#define TOMB_CHECK_RESULT(x) ((x == TOMB_SUCCESS) ? 1 : 0)
#endif