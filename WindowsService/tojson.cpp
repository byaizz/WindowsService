#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include <iostream>
#include <Windows.h>

using namespace rapidjson;
using namespace std;

const char *str = "{\"name\" : \"tom\", \"key1\" : \"10.5\", \"key2\" : 20.66}";
bool tocpp(const char *json)
{
	Document d;
	d.Parse(json);
	if (d.HasMember("name") && d["name"].IsString())
		cout << d["name"].GetString() << endl;
	if (d.HasMember("key1") && d["key1"].IsString())
		cout << d["key1"].GetString() << endl;
	if (d.HasMember("key2") && d["key2"].IsDouble())
		cout << d["key2"].GetDouble() << endl;
	return true;
}
struct test {
	int num1;
	int num2;
};
bool tojson()
{
	Document d;
	d.SetObject();
	Document::AllocatorType &allocator = d.GetAllocator();
	Value name("tom");
	Value key1("10.5");
	Value key2(10.5f);
	d.AddMember("name", name, allocator);
	d.AddMember("key1", key1, allocator);
	d.AddMember("key2", key2, allocator);
	test t = { 10,20 };
	Document dd;
	dd.SetObject();
	dd.AddMember("name", name, dd.GetAllocator());
	d.AddMember("dd", dd, allocator);

	StringBuffer sb;
	Writer<StringBuffer> write(sb);
	d.Accept(write);
	

	cout << sb.GetString() << endl;

	return true;
}

int __main()
{
	tojson();
	getchar();
	return 0;
}