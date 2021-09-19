#pragma once

class Sql
{
public:
	Sql();
	~Sql();

	static unique_ptr<Sql> Create();
	MYSQL _mysql;

	auto Get() { return &_mysql; }

	unsigned FetchError();
};

