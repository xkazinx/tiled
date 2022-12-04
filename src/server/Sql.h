#pragma once

class Sql;
using SelectValue = szt;

class SelectItem
{
public:
	SelectItem(const MYSQL_ROW & row, const vector<unsigned long> & length, const map<string, SelectValue>& fields)
		: _row(row), _length(length), _fields(fields)
	{
		
	}

	template<typename T>
	T Get(string field)
	{
		if (_fields.contains(field))
		{
			const auto idx = _fields.at(field);
			string data(_row[idx], _length[idx]);

			if constexpr (is_same_v<bool, T>)
			{
				return std::stoi(data) > 0;
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				if constexpr (std::is_floating_point_v<T>)
				{
					return (T)std::strtof(data);
				}
				else
				{
					if constexpr (std::is_signed_v<T>)
					{
						return (T)std::stoll(data);
					}
					else if constexpr (std::is_unsigned_v<T>)
					{
						return (T)std::stoull(data);
					}
				}
			}
			else return data;
		}

		PrintError("Field not found for SelectItem::Pop: {}", field);
		return string("");
	}

	const MYSQL_ROW & _row;
	const vector<unsigned long>& _length;
	const map<string, SelectValue> & _fields;
};

class SelectResult final
{
public:
	SelectResult(Sql* sql);

	~SelectResult();

	MYSQL_RES* _ptr;
	vector<MYSQL_ROW> _rows;
	vector<MYSQL_FIELD*> _fields;
	vector<vector<unsigned long>> _lengths;
	map<string, SelectValue> _fields_indexes;
	SelectValue _current_row_idx = 0;

	unique_ptr<SelectItem> Pop()
	{
		const auto idx = _current_row_idx++;
		if (idx >= _rows.size())
			return nullptr;

		return make_unique<SelectItem>(_rows[(szt)idx], _lengths[(szt)idx],_fields_indexes);
	}

	auto GetCount() const { return _rows.size(); }
	auto IsEmpty() const { return _rows.empty(); }
};

class Sql final
{
public:
	Sql();
	~Sql();

	static unique_ptr<Sql> Create();
	MYSQL _ptr;

	auto Get() { return &_ptr; }

	unsigned FetchError();

	// #todo make class query
	void Query(const string & query);
	void Insert(const string & table, const vector<string> & columns, const vector<vector<string>> & rows);
	unique_ptr<SelectResult> Select(const string& table, const vector<string>& columns, const string& extra = "");

	static string Scape(const string& str, const string& scape, const string& add = "")
	{
		return scape + str + scape + add;
	}
};
