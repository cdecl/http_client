# http_client
C++ Http Client Library (header only)

> C++11, boost::asio, openssl



##example##
```c++

#include <iostream>
#include <fstream>
using namespace std;

#include "include/http_client.h"


void sync_test(GLASS::http_service &service)
{
	using namespace GLASS;

	http_client c(service);
	
	if (c.open("http://httpbin.org/")) {
		c.add_path("post?k=1111&v=44444");
		c.add_header("User-Agent", "cdecl/webclient");

		// wait & get 
		// c++ promise, future

		//c.get();
		//c.post();
		//c.head();
		//c.send("DELETE");
		//c.send("PUT");
		int status = c.post("body data").get();

		if (status > HTTP_TIMEOUT) {
			cout << "status : " << status << endl;

			if (status == HTTP_200) {
				http_parser ps = http_parser::parse(c.response());
				cout << ps.body().str() << endl;
			}

		}
	}
}


void async_test(GLASS::http_service &service)
{
	using namespace GLASS;

	http_client c1(service);
	http_client c2(service);
	
	// 3 second delay
	if (c1.open("http://httpbin.org/delay/3") && c2.open("http://httpbin.org/delay/3")) {
		c1.add_header("User-Agent", "cdecl/webclient 1");
		auto ret1 = c1.get();	// async get 

		c2.add_header("User-Agent", "cdecl/webclient 2");
		auto ret2 = c2.get();	// async get 

		// ...

		auto st1 = ret1.get();	// wait
		auto st2 = ret2.get();	// wait 

		if (st1 > HTTP_TIMEOUT || st2 > HTTP_TIMEOUT) {
			cout << "status : " << st1 << ", " << st2 << endl;

			{
				http_parser p = http_parser::parse(c1.response());
				cout << p.body().str() << endl;
			}

			{
				http_parser p = http_parser::parse(c2.response());
				cout << p.body().str() << endl;
			}
		}
			
	}
	
}

void proxy_test(GLASS::http_service &service)
{
	using namespace GLASS;

	http_client c(service);
	
	// proxy test (fiddler)
	if (c.open("http://httpbin.org/xml", "http://localhost:8888")) {
		int status = c.get().get();

		if (status > HTTP_TIMEOUT) {
			cout << "status : " << status << endl;

			if (status == HTTP_200) {
				http_parser ps = http_parser::parse(c.response());
				cout << ps.body().str() << endl;
			}
		}
	}
	
}


void timeout_test(GLASS::http_service &service)
{
	using namespace GLASS;

	http_client c(service);

	if (c.open("http://httpbin.org/delay/5", "", 1000)) {
		int status = c.get().get();

		if (status == HTTP_TIMEOUT) {
			cout << "TIMEOUT " << endl;
		}
	}

}


int main()
{
	try {
		GLASS::http_service service;

		cout << ">> sync_test" << endl;
 		sync_test(service);

		cout << ">> async_test" << endl;
		async_test(service);

		cout << ">> proxy_test" << endl;
		proxy_test(service);

		cout << ">> timeout_test" << endl;
		timeout_test(service);

	}
	catch (std::exception &e) {
		cout << e.what() << endl;
	}

	return 0;
}



```
