#include <tchar.h>
#include "messenger.h"
#include "crypto/curve25519.h"
#include "crypto/crypto.h"
#include "crypto/aes256.h"
#include "logger/log.h"

#define DUMP(s, i, buf, sz)  {printf(s);                   \
                              for (i = 0; i < (sz);i++)    \
                                  printf("%02x ", buf[i]); \
                              printf("\n");}

int _tmain(int argc, _TCHAR* argv[])
{

	//GraceMessenger::callbacks callbacks;
	//GraceMessenger::config config;
	//GraceMessenger::messenger messenger(config, callbacks);

	//uint8_t asecret[32];
	//uint8_t apublic[32];
	//GraceMessenger::Crypto::generate_keypair(asecret, apublic);

	//uint8_t bsecret[32];
	//uint8_t bpublic[32];
	//GraceMessenger::Crypto::generate_keypair(bsecret, bpublic);

	//uint8_t ashared[32];
	//uint8_t bshared[32];

	//GraceMessenger::Crypto::generate_sharedkey(ashared, asecret, bpublic);
	//GraceMessenger::Crypto::generate_sharedkey(bshared, bsecret, apublic);
	//LOG(Info, "123454");
	//LOG(Warning, "123414");

	//aes256_context ctx;
	//uint8_t key[32];
	//uint8_t buf[16], i;

	///* put a test vector */
	//for (i = 0; i < sizeof(buf); i++) buf[i] = i * 16 + i;
	//for (i = 0; i < sizeof(key); i++) key[i] = i;

	//DUMP("txt: ", i, buf, sizeof(buf));
	//DUMP("key: ", i, key, sizeof(key));
	//printf("---\n");

	//aes256_init(&ctx, key);
	//aes256_encrypt_ecb(&ctx, buf);
	//
	//DUMP("enc: ", i, buf, sizeof(buf));
	//printf("tst: 8e a2 b7 ca 51 67 45 bf ea fc 49 90 4b 49 60 89\n");

	//aes256_init(&ctx, key);
	//aes256_decrypt_ecb(&ctx, buf);
	//DUMP("dec: ", i, buf, sizeof(buf));

	//aes256_done(&ctx);

	try{
		GraceDHT::dht dht("127.0.0.1", 9000);
		std::cout << dht.get_node_id() << std::endl;
		dht.start();

		GraceDHT::dht dht1("127.0.0.1", 9001);
		dht1.start();
		dht1.bootstrap(dht.get_node_id(), "127.0.0.1", 9000, [&](bool success, int error){std::cout << "bootstrap" << std::endl; });
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		GraceDHT::dht dht2("127.0.0.1", 9002);
		dht2.start();
		dht2.bootstrap(dht.get_node_id(), "127.0.0.1", 9000, [&](bool success, int error){std::cout << "bootstrap" << std::endl; });
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		GraceDHT::dht dht3("127.0.0.1", 9003);
		dht3.start();
		dht3.bootstrap(dht.get_node_id(), "127.0.0.1", 9000, [&](bool success, int error){std::cout << "bootstrap" << std::endl; });
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		GraceDHT::dht dht4("127.0.0.1", 9004);
		dht4.start();
		dht4.bootstrap(dht.get_node_id(), "127.0.0.1", 9000, [&](bool success, int error){std::cout << "bootstrap" << std::endl; });
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		GraceDHT::dht dht5("127.0.0.1", 9005);
		dht5.start();
		dht5.bootstrap(dht.get_node_id(), "127.0.0.1", 9000, [&](bool success, int error){std::cout << "bootstrap" << std::endl; });
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		GraceDHT::dht dht6("127.0.0.1", 9006);
		dht6.start();
		dht6.bootstrap(dht.get_node_id(), "127.0.0.1", 9000, [&](bool success, int error){std::cout << "bootstrap" << std::endl; });
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		dht6.find_node(dht5.get_node()->id,
			[](const GraceDHT::node_id *id, bool success, const GraceDHT::node* result, int error)
		{
			std::cout << (success ? "success" : "fail") << std::endl;
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		dht1.find_node(dht3.get_node()->id,
			[](const GraceDHT::node_id *id, bool success, const GraceDHT::node* result, int error)
		{
			std::cout << (success ? "success" : "fail") << std::endl;
		});

		dht2.find_node(dht3.get_node()->id,
			[](const GraceDHT::node_id *id, bool success, const GraceDHT::node* result, int error)
		{
			std::cout << (success ? "success" : "fail") << std::endl;
		});


		int a;
		std::cin >> a;
	}
	catch (std::exception e)
	{
		std::cout << e.what() << std::endl;
		LOG(Error, e.what());
	}
	return 0;
}

