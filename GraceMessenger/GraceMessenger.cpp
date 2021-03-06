#include <tchar.h>
#include <conio.h>
#include <iostream>

#include "messenger.h"
#include "config/config.h"
#include "config/config_loader.h"

//#include "logger/log.h"



void dht_bootstrapped(bool success, int error)
{
	success ? std::cout << "---dht bootstrap success---" << std::endl : 
			  std::cout << "---dht bootstrap failed---" << std::endl;
}

void message_sent(const GraceMessenger::message *mes)
{
	std::cout << "sent message: ";
	std::wcout << mes->content << std::endl;
}

void message_received(const GraceMessenger::message *mes)
{
	std::cout << "recv message: ";
	std::wcout << mes->content << std::endl;
}

void message_delivered(const GraceMessenger::message *mes)
{
	std::cout << "message delivered: ";
	std::wcout << mes->content << std::endl;
}


void message_send_error(const GraceMessenger::message *mes, const GraceMessenger::status* status)
{
	std::cout << "message send error: ";
	std::cout << status->message << std::endl;
}


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		using namespace GraceMessenger;
			
		callbacks callbacks;
		callbacks.dht_bootstrapped = dht_bootstrapped;
		callbacks.message_sent = message_sent;
		callbacks.message_received = message_received;
		callbacks.message_send_error = message_send_error;
		callbacks.message_delivered = message_delivered;

		config dht_config = config_loader::load_config(L"dht_conf.json");
		messenger dht(dht_config, callbacks);
		std::cout << "DHT started. port: " << dht.dht_port() << std::endl;
		std::cout << "ID: " << dht.id() << std::endl;
	
		/*auto id = "0F3003DECBFBAB835ECBCC7D9175795ED3E2A09A07E0A56B2E446F7D1D465643";
		unsigned short port = 6000;
		auto ip = "54.191.219.95";*/

		config config1 = config_loader::load_config(L"config1.json");
		messenger messenger1(config1, callbacks);
		messenger1.dht_bootstrap();
		messenger1.online();
		
		config config2 = config_loader::load_config(L"config2.json");
		messenger messenger2(config2, callbacks);
		messenger2.dht_bootstrap();
		messenger2.online();

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		
		message m(config2.user.id, L"Hello from Semyon");
		messenger1.send_message(m);
		message m2(config1.user.id, L"Hello from Boris");
		messenger2.send_message(m2);

		std::cout << "DHT nodes count: " << messenger2.dht_nodes_count() << std::endl;

		//config_loader::save_config(config2, L"config2.json");

		//config from_file = config_loader::load_config(L"config2.json");


		_getch();
		
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
		LOG(Error, e.what());
	}
	return 0;
}

