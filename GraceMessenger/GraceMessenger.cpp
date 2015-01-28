#include <tchar.h>
#include <conio.h>

#include "messenger.h"
#include "logger/log.h"
#include "config_loader.h"


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

		user dht_user = user(L"DHT");
		GraceDHT::dht dht("localhost", 6000, dht_user.id);
		dht.start();

		callbacks callbacks;
		callbacks.dht_bootstrapped = dht_bootstrapped;
		callbacks.message_sent = message_sent;
		callbacks.message_received = message_received;
		callbacks.message_send_error = message_send_error;

		config config1;
		config1.dht_port = 5555;
		config1.ip_adress = "127.0.0.1";
		config1.user = user(L"Semyon");
		messenger messenger1(config1, callbacks);
		messenger1.start_dht();
		messenger1.dht_bootstrap(dht.get_node_id(), "127.0.0.1", 6000);
		
		config config2;
		config2.dht_port = 5566;
		config2.ip_adress = "localhost";
		config2.user = user(L"Boris");
		messenger messenger2(config2, callbacks);
		messenger2.start_dht();
		messenger2.dht_bootstrap(dht.get_node_id(), "localhost", 6000);

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		
		message m(config2.user.id, L"Hello from Semyon");
		messenger1.send_message(m);
		message m2(config1.user.id, L"Hello from Boris");
		messenger2.send_message(m2);

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

