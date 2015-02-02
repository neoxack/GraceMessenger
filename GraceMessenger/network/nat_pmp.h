#pragma once



namespace GraceMessenger
{
	namespace Network
	{
		class nat_pmp
		{
		public:
			void forward_port(unsigned short localPort, unsigned short publicPort);  
		};

	}
}
