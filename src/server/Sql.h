#pragma once

class Sql final
{
public:
	Sql();
	~Sql();

	static unique_ptr<Sql> Create();
	MYSQL _mysql;

	auto Get() { return &_mysql; }

	unsigned FetchError();

	// #todo make class query
	void Query(const string & query);

	void Insert(const string & table, const vector<string> & columns, const vector<vector<string>> & rows);

	template <typename T>
	vector<T> && Select(const string& table, const vector<string>& columns, const string);
};
