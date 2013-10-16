#include <iostream>

using std::cout;
using std::endl;


class Args {
	private:
		bool setTolerance;
		double tolerance;

		bool cOption;
		char mode;
		char *inputSize;

	public:
		Args();
		~Args();

		void parseArgs();
		const bool isToleranceSet();
		const double :
};
