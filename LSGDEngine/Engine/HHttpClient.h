#pragma once

// httplib 3rd library
#include "..\..\LSGDEngine\ThirdParty\httplib\httplib.h"
// thread for ::Test
#include <thread>

namespace lsgd { namespace networking {

	class HHttpClient
	{
	public:
		HHttpClient(const HString& InAddress, uint32 InPortNumber)
			: HttpClientObject(InAddress.c_str(), InPortNumber)
		{}

		void Post(const HString& InContent)
		{			
			HttpClientObject.Post("/hi", InContent, "text/plain");
		}

		static void Test()
		{
			// temporary create the thread for http server
			std::thread HttpServer([] {
					httplib::Server Server;
					Server.Post("/hi", [](const httplib::Request& req, httplib::Response& res) {
						res.set_content(req.body, "text/plain");
						});
					Server.listen("localhost", 1234);
				});			

			HHttpClient Client("localhost", 1234);
			Client.Post("testtest");

			HttpServer.join();
		}

		httplib::Client HttpClientObject;
	};

} }
