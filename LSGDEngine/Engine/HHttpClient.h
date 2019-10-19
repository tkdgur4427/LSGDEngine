#pragma once

// forward declaration
namespace httplib
{
	class Client;
}

namespace lsgd { namespace networking {

	class HHttpClient
	{
	public:
		HHttpClient(const HString& InAddress, uint32 InPortNumber);

		void Post(const HString& InContent);

		static void Test();

		shared_ptr<httplib::Client> HttpClientObject;
	};

} }
