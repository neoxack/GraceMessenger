#include <tchar.h>
#include "messenger.h"
#include "crypto/curve25519.h"
#include "crypto/crypto.h"


int _tmain(int argc, _TCHAR* argv[])
{
	GraceMessenger::messenger messenger;

	uint8_t asecret[32];
	uint8_t apublic[32];
	GraceMessenger::Crypto::generage_keypair(asecret, apublic);

	uint8_t bsecret[32];
	uint8_t bpublic[32];
	GraceMessenger::Crypto::generage_keypair(bsecret, bpublic);

	uint8_t ashared[32];
	uint8_t bshared[32];

	GraceMessenger::Crypto::generage_sharedkey(ashared, asecret, bpublic);
	GraceMessenger::Crypto::generage_sharedkey(bshared, bsecret, apublic);


	return 0;
}

