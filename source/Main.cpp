#include "ConnHandler.h"
#include <stdio.h>
#include <cstdlib>
#include <cerrno>
#include "PowerLoss.h"
#include "TimeHandler.h"
#include "Stream.h"
#include "Subscription.h"

using namespace curlpp::options;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "Invalid arguments" << endl;
		return -1;
	}

	PowerLoss::startService();
	ConnHandler ch(argv[1], argv[2]);
	return 0;
}