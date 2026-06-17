#pragma once

#define BIT(x) (1 << x)

#define PT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)
