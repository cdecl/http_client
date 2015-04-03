#ifdef _MSC_VER
 #include "stdafx.h"
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
using namespace std;

#include "lib/http_client.h"
using namespace json11;

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

//#define PROXY_TEST

TEST_CASE("Http Synchronize Call ", "Synchronize")
{
	using namespace GLASS;
	using namespace json11;

	http_service service;
	http_client c(service);

	SECTION("GET") {
		REQUIRE(c.open("http://httpbin.org/get"));

		int status = c.get().get();
		CAPTURE(status);
		REQUIRE(status == HTTP_200);

		http_parser ps = http_parser::parse(c.response());
		REQUIRE(ps.body().str().size() > 0);

		REQUIRE(ps.header()["Content-Type"] == "application/json");

		Json json = ps.json();
		REQUIRE(json.is_object());
		REQUIRE(json["headers"]["Host"] == "httpbin.org");
	}

	SECTION("POST") {
		const std::string data = "q=sample";

		REQUIRE(c.open("http://httpbin.org/post"));

		int status = c.post(data).get();
		CAPTURE(status);
		REQUIRE(status == HTTP_200);

		http_parser ps = http_parser::parse(c.response());
		REQUIRE(ps.body().str().size() > 0);

		REQUIRE(ps.header()["Content-Type"] == "application/json");

		Json json = ps.json();
		REQUIRE(json.is_object());
		REQUIRE(json["data"] == data);
	}

	SECTION("HEAD") {
		REQUIRE(c.open("http://httpbin.org/get"));

		auto r = c.head();
		REQUIRE(r.get() == HTTP_200);
		{
			http_parser ps = http_parser::parse(c.response());
			REQUIRE(ps.body().str().size() == 0);
		}
	}
}

TEST_CASE("Http Asynchronize Call", "Asynchronize")
{
	using namespace GLASS;
	using namespace json11;

	http_service service;

	http_client c1(service);
	http_client c2(service);

	SECTION("Asynchronize") {
		REQUIRE(c1.open("http://httpbin.org"));
		REQUIRE(c2.open("http://httpbin.org"));

		c1.add_path("/get");
		c2.add_path("/post");

		auto r1 = c1.get();
		auto r2 = c2.post();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		REQUIRE(r1.get() == HTTP_200);
		REQUIRE(r2.get() == HTTP_200);
		{
			http_parser ps = http_parser::parse(c1.response());
			REQUIRE(ps.body().str().size() > 0);
			REQUIRE(ps.body().str().size() == stoi(ps.header()["Content-Length"]));
		}
		{
			http_parser ps = http_parser::parse(c2.response());
			REQUIRE(ps.body().str().size() > 0);
			REQUIRE(ps.body().str().size() == stoi(ps.header()["Content-Length"]));
		}
	}
}

TEST_CASE("Http Timeout", "Timeout")
{
	using namespace GLASS;
	using namespace json11;

	http_service service;
	http_client c(service);

	SECTION("Timeout") {
		// delay 1 second, timeout : 500ms 
		REQUIRE(c.open("http://httpbin.org/delay/1", "", 500));
		
		int status = c.get().get();
		REQUIRE(status == HTTP_TIMEOUT);
	}
}


TEST_CASE("Http Other Test", "Chunked, Htts")
{
	using namespace GLASS;
	using namespace json11;

	http_service service;
	http_client c(service);

	SECTION("Chunked") {
		REQUIRE(c.open("http://httpbin.org"));

		const int LINES = 100;
		c.add_path(string("/stream/") + to_string(LINES));

		int status = c.get().get();
		CAPTURE(status);
		REQUIRE(status == HTTP_200);

		http_parser ps = http_parser::parse(c.response());
		REQUIRE(ps.body().str().size() > 0);

		REQUIRE(ps.header()["Transfer-Encoding"] == "chunked");

		auto &body = ps.body();
		std::string s;
		int n = 0;
		while (getline(body, s)) {
			if (s.length() > 0 && s[0] == '{') ++n;
		}
		REQUIRE(LINES == n);
	}

	SECTION("Https") {
		// https protocal
		REQUIRE(c.open("https://httpbin.org/get"));

		int status = c.get().get();
		CAPTURE(status);
		REQUIRE(status == HTTP_200);

		http_parser ps = http_parser::parse(c.response());
		REQUIRE(ps.body().str().size() > 0);

		REQUIRE(ps.header()["Content-Type"] == "application/json");

		Json json = ps.json();
		REQUIRE(json.is_object());
		REQUIRE(json["headers"]["Host"] == "httpbin.org");
	}

}


#ifdef PROXY_TEST
TEST_CASE("proxy call", "use fiddler")
{
	using namespace GLASS;
	http_service service;
	http_client c(service);

	SECTION("proxy(fiddler) http") {
		REQUIRE(c.open("http://httpbin.org/", "http://localhost:8888"));

		int status = c.get().get();
		CAPTURE(status);
		REQUIRE(status == HTTP_200);
		{
			http_parser ps = http_parser::parse(c.response());
			REQUIRE(ps.body().str().size() > 0);
		}
	}

	SECTION("proxy(fiddler) https") {
		REQUIRE(c.open("https://httpbin.org/", "http://localhost:8888"));

		int status = c.get().get();
		CAPTURE(status);
		REQUIRE(status == HTTP_200);
		{
			http_parser ps = http_parser::parse(c.response());
			REQUIRE(ps.body().str().size() > 0);
		}
	}

}
#endif 


