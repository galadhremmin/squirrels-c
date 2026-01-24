#pragma once

#define AUTO_CLEANUP_FUNC(x) __attribute__((cleanup(x)))
