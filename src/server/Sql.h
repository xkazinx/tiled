#pragma once

namespace Server
{
struct Sql;
using SelectValue = szt;

struct SelectItem
{
	SelectItem(const MYSQL_ROW & row, const vector<unsigned long> & length, const std::map<string, SelectValue> & fields)
		: _row(row), _length(length), _fields(fields)
	{

	}

	template <typename T>
	T Get(const string & field)
	{
		if (_fields.contains(field))
		{
			const auto idx = _fields.at(field);
			string data = _row[idx] ? string(_row[idx], _length[idx]) : string();

			if constexpr (is_same_v<bool, T>)
			{
				return std::stoi(data) > 0;
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				if constexpr (std::is_floating_point_v<T>)
				{
					return (T)std::stod(data);
				}
				else if constexpr (std::is_signed_v<T>)
				{
					return (T)std::stoll(data);
				}
				else
				{
					return (T)std::stoull(data);
				}
			}
			else return data;
		}

		Err("Field not found for SelectItem::Pop: {}", field);
		return T{};
	}

	const MYSQL_ROW & _row;
	const vector<unsigned long> & _length;
	const std::map<string, SelectValue> & _fields;
};

struct SelectResult final
{
	SelectResult(Sql * sql);

	~SelectResult();

	MYSQL_RES * _ptr = nullptr;
	vector<MYSQL_ROW> _rows;
	vector<MYSQL_FIELD *> _fields;
	vector<vector<unsigned long>> _lengths;
	std::map<string, SelectValue> _fields_indexes;
	SelectValue _current_row_idx = 0;

	unique_ptr<SelectItem> Pop()
	{
		const auto idx = _current_row_idx++;
		if (idx >= _rows.size())
			return nullptr;

		return make_unique<SelectItem>(_rows[idx], _lengths[idx], _fields_indexes);
	}

	auto GetCount() const { return _rows.size(); }
	auto IsEmpty() const { return _rows.empty(); }
};

// One connection to the game's database. Where it is: init.json beside the exe
// (never committed: init.json.example) or launch flags, db_host (localhost),
// db_port (3306), db_user (tiled), db_pass, db_name (tiled).
struct Sql final
{
	Sql();
	~Sql();

	static unique_ptr<Sql> Create();
	MYSQL _ptr;

	MYSQL * Get() { return &_ptr; }

	unsigned FetchError();

	// A value for a query: escaped and quoted ('...')
	string Quote(const string & value);

	// #todo make class query
	bool Query(const string & query);
	bool Insert(const string & table, const vector<string> & columns, const vector<vector<string>> & rows);
	unique_ptr<SelectResult> Select(const string & table, const vector<string> & columns, const string & extra = "");

	// The accounts (table `players`, server/sql/schema.sql): the password kept as
	// SHA-256 of a random salt and the password, never as it was typed
	bool CheckLogin(const string & user, const string & pass);
	bool CreateAccount(const string & user, const string & pass);

	static string Scape(const string & str, const string & scape, const string & add = "")
	{
		return scape + str + scape + add;
	}
};
}
