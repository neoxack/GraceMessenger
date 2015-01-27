#pragma once

#include <string>

namespace GraceMessenger
{

	class status
	{
	public:
		bool is_ok()
		{
			return code == ok;
		}

		enum status_code
		{
			ok,
			error
		};

		int code;
		std::string message;
	};

}