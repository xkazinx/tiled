#include "stdafx.h"
#include "Sql.h"

#include <ranges>

Sql::Sql()
{
	mysql_init(Get()); // #Check if needs to be called for every connection
}

Sql::~Sql()
{
}

unique_ptr<Sql> Sql::Create()
{
	auto sql = make_unique<Sql>();

	if (!mysql_real_connect(sql->Get(), "localhost", "root", "root", "tileship", 3306, nullptr, 0) ||
		sql->FetchError())
	{
		cout << "Error in mysql_real_connect" << endl;
		sql.reset();
	}
	
	return std::move(sql);
}

unsigned Sql::FetchError()
{
	const auto err = mysql_errno(Get());
	if (err != 0)
		cout << "Error in connecting error: " << err << endl;

	return err;

}

void Sql::Query(const string& query)
{
	mysql_real_query(Get(), query.data(), query.size());
}

void Sql::Insert(const string& table, const vector<string>& columns, const vector<vector<string>>& rows)
{
	// #todo Make this function shared
	auto scape = [&](auto& text, auto& scape)
	{
		return scape + text + scape;
	};

	string values = "";

	for(int i = 0; i < columns.size() - 1; i++)
	{
		values += scape(columns[i], "`") + ",";
	}

	values += scape(columns[columns.size() - 1], "`");

	string data;

	for (int i = 0; i <= rows.size() - 1; i++)
	{
		const auto & row = rows[i];

		data += "(";

		for (int j = 0; j < row.size() - 1; j++)
		{
			data += scape(row[j], "\"") + ",";
		}

		data += scape(row[row.size() - 1], "\"") + ")";

		data += i == rows.size() - 1 ? ";" : ",";
	}

	Query(std::format("INSERT INTO `{}` ({}) VALUES {}", table, values, data));
}

unique_ptr<SelectResult> Sql::Select(const string& table, const vector<string>& columns)
{
	// #todo Make this function shared
	auto scape = [&](auto& text, auto& scape)
	{
		return scape + text + scape;
	};

	string select = "";

	for (int i = 0; i < columns.size() - 1; i++)
	{
		select += scape(columns[i], "`") + ",";
	}

	select += scape(columns[columns.size() - 1], "`");

	Query(std::format("SELECT {} FROM `{}`", select, table));

	return make_unique<SelectResult>(this);
}

SelectResult::SelectResult(Sql * sql)
{
	_ptr = mysql_store_result(sql->Get());

	{
		auto total = mysql_num_rows(_ptr);
		_rows.reserve(total);
		_lengths.resize(total);
		while (total-- > 0)
		{
			_rows.emplace_back(mysql_fetch_row(_ptr));

			auto lengths = mysql_fetch_lengths(_ptr);
			for(szt i = 0; i < _ptr->field_count; ++i)
			{
				// #todo storing length pointers directly
				_lengths[_rows.size() - 1].emplace_back(lengths[i]);
			}
		}
	}

	{
		mysql_field_seek(_ptr, 0);
		auto total = _ptr->field_count;
		_fields.reserve(total);
		while (total-- > 0)
		{
			_fields.emplace_back(mysql_fetch_field(_ptr));
			_fields_indexes[_fields.back()->name] = _ptr->field_count - (total + 1);
		}
	}
}

SelectResult::~SelectResult()
{
	mysql_free_result(_ptr);
}