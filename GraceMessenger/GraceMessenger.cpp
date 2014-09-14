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
	GraceMessenger::messenger messenger;

	uint8_t asecret[32];
	uint8_t apublic[32];
	GraceMessenger::Crypto::generate_keypair(asecret, apublic);

	uint8_t bsecret[32];
	uint8_t bpublic[32];
	GraceMessenger::Crypto::generate_keypair(bsecret, bpublic);

	uint8_t ashared[32];
	uint8_t bshared[32];

	GraceMessenger::Crypto::generate_sharedkey(ashared, asecret, bpublic);
	GraceMessenger::Crypto::generate_sharedkey(bshared, bsecret, apublic);
	LOG(Info, "123454");
	LOG(Warning, "123414");

	aes256_context ctx;
	uint8_t key[32];
	uint8_t buf[16], i;

	/* put a test vector */
	for (i = 0; i < sizeof(buf); i++) buf[i] = i * 16 + i;
	for (i = 0; i < sizeof(key); i++) key[i] = i;

	DUMP("txt: ", i, buf, sizeof(buf));
	DUMP("key: ", i, key, sizeof(key));
	printf("---\n");

	aes256_init(&ctx, key);
	aes256_encrypt_ecb(&ctx, buf);
	
	DUMP("enc: ", i, buf, sizeof(buf));
	printf("tst: 8e a2 b7 ca 51 67 45 bf ea fc 49 90 4b 49 60 89\n");

	aes256_init(&ctx, key);
	aes256_decrypt_ecb(&ctx, buf);
	DUMP("dec: ", i, buf, sizeof(buf));

	aes256_done(&ctx);

	return 0;
}

