#ifndef _TIME_C_
#define _TIME_C_

#include <sys/time.h>

class time_counting {
private:
	timeval _record;

	double _get_float_sec(const timeval &t) {
		double rst = t.tv_sec;
		rst += t.tv_usec / 1000000.0;
		return rst;
	}
public:
	time_counting() {
		record();
	}

	void record() {
		gettimeofday(&_record, NULL);
	}

	double get_time_sec() {
		timeval now;
		gettimeofday(&now, NULL);

		return _get_float_sec(now) - _get_float_sec(_record);
	}
};

#endif //_TIME_C_