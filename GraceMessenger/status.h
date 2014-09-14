#pragma once

#include <string>

namespace GraceMessenger
{

	class status
	{
	public:
		bool is_ok()
		{
			return _code == ok;
		}

	private:
		enum status_code
		{
			ok,
			error
		};

		int _code;
		std::string _message;
	};

}