#include "CriticalSection.h"

Locking<FirebaseTokensHandler> critical_section::firebaseTokensHandler = Locking<FirebaseTokensHandler>(FirebaseTokensHandler {});
Locking<SensorsHandler> critical_section::sensorsHandler = Locking<SensorsHandler>(SensorsHandler {});
Locking<ClientHandler> critical_section::clientUpdater = Locking<ClientHandler>(ClientHandler {});
atomic<bool> critical_section::alarmActivated = false;
atomic<bool> critical_section::defensesActivated = false;
