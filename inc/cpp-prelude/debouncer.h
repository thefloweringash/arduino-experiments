#ifndef CPP_PRELUDE_DEBOUNCER
#define CPP_PRELUDE_DEBOUNCER

#include <limits>

template <typename T,
          typename io_t>
class Debouncer {
	T mPinState;
	bool mHigh;

public:
	enum Edge {
		None = 0, Falling = 1, Rising = 2
	}  __attribute__((packed));

	Edge step() {
		constexpr T allOn = std::numeric_limits<T>::max();
		constexpr T allOff = std::numeric_limits<T>::min();

		mPinState = (mPinState << 1) | io_t::get();
		if (mHigh == false && mPinState == allOn) {
			mHigh = true;
			return Rising;
		}
		else if (mHigh == true && mPinState == allOff) {
			mHigh = false;
			return Falling;
		}
		else {
			return None;
		}
	}
};

#endif
