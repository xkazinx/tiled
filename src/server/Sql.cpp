#include "stdafx.h"
#include "Sql.h"

Sql::Sql()
{
	mysql_init(Get()); // #Check if needs to be called for every connection
}

Sql::~Sql() = default;

unique_ptr<Sql> Sql::Create()
{
	auto sql = make_unique<Sql>();

	if (!mysql_real_connect(sql->Get(), "localhost", "root", "root", 
		"tileship", 3306, nullptr, 0) ||
		sql->FetchError())
	{
		Log::Error("Error in mysql_real_connect");
		return nullptr;
	}
	
	return move(sql);
}

unsigned Sql::FetchError()
{
	const auto err = mysql_errno(Get());
	if (err != 0)
		Log::Error("Error in MySQL: {}", err);
	// #todo get error string
	return err;

}

void Sql::Query(const string& query)
{
	if (mysql_real_query(Get(), query.data(), query.size()) != 0)
	{
		FetchError();
	}
}

void Sql::Insert(const string& table, const vector<string>& columns, const vector<vector<string>>& rows)
{
	// #debug #ifdef DEBUG_SQL
	assert(columns.size() > 0);

	if (rows.empty())
	{
		cout << "Warning: Tried to run Sql::Insert without rows to insert" << endl;
		return;
	}

	Print("Sql::Insert called for {} table", table);
	
	auto values = "("s;
	{
		for (szt i = 0; i < columns.size() - 1; i++)
		{
			values += Scape(columns[i], "`", ",");
		}

		values += Scape(columns[columns.size() - 1], "`)");
	}

	string data;
	{
		for (szt i = 0; i <= rows.size() - 1; i++)
		{
			const auto& row = rows[i];

			data += "(";

			// #todo Scape differently if not string
			for (szt j = 0; j < row.size() - 1; j++)
			{
				data += Scape(row[j], "\"", ",");
			}

			data += Scape(row[row.size() - 1], "\"", ")");

			data += i == rows.size() - 1 ? ";" : ",";
		}
	}

	Query(format("INSERT INTO `{}` ({}) VALUES {}", table, values, data));
}

unique_ptr<SelectResult> Sql::Select(const string& table, const vector<string>& columns)
{
	string select;

	for (szt i = 0; i < columns.size() - 1; i++)
	{
		select += Scape(columns[i], "`", ",");
	}

	select += Scape(columns[columns.size() - 1], "`");

	Query(format("SELECT {} FROM `{}`", select, table));

	return make_unique<SelectResult>(this);
}

SelectResult::SelectResult(Sql * sql)
{
	_ptr = mysql_store_result(sql->Get());

	{
		auto total = (SelectValue)mysql_num_rows(_ptr);
		_rows.reserve(total);
		_lengths.resize(total);
		while (total-- > 0)
		{
			_rows.emplace_back(mysql_fetch_row(_ptr));

			auto lengths = mysql_fetch_lengths(_ptr);
			for(szt i = 0; i < _ptr->field_count; ++i)
			{
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