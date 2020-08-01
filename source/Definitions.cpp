#include "Definitions.h"

std::condition_variable statical::sharedCondition;
std::mutex statical::mSharedCondition;

std::condition_variable statical::newFirebaseNotification;
std::mutex statical::mNewFirebaseNotification;