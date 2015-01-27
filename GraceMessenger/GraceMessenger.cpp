#include <tchar.h>
#include "messenger.h"
#include "logger/log.h"


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		
		GraceMessenger::callbacks callbacks;
		GraceMessenger::config config;
		config.dht_port = 5555;
		config.messenger_port = 5566;
		config.ip_adress = "127.0.0.1";
		config.user = GraceMessenger::user(L"Semyon");
		GraceMessenger::messenger messenger(config, callbacks);
		messenger.start_dht();


	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
		LOG(Error, e.what());
	}
	return 0;
}

