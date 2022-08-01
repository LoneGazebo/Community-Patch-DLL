// This file includes functions needed to build with clang.
extern "C" {
	long _ftol3(float f) { return static_cast<long>(f); }
	double _ltod3(long l) { return static_cast<double>(l); }
}
