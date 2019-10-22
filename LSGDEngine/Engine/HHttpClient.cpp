#include "HEnginePCH.h"
#include "HHttpClient.h"

// httplib 3rd library
#include "..\..\LSGDEngine\ThirdParty\httplib\httplib.h"
// thread for ::Test
#include <thread>

using namespace lsgd::networking;
using namespace lsgd;

HHttpClient::HHttpClient(const HString& InAddress, uint32 InPortNumber)
{
	HttpClientObject = HMakeShared<httplib::Client>(InAddress.c_str(), InPortNumber);
}

void HHttpClient::Post(const HString& InContent)
{
	//HttpClientObject->Post("/hi", InContent, "text/plain");
}

void HHttpClient::Test()
{
	httplib::Server server;
	server.Post("/ConnectToMatchmakingServer", [](const httplib::Request& req, httplib::Response& res)
		{
			const HString Content(
				"{\"result\":1, \"serverno\":2, \"ip\":\"10.99.1.21\", \"port\":6000}"
			);
			res.set_content(Content, "application/json");
		});
	server.listen("localhost", 5999);
}
