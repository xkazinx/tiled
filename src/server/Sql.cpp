#include "stdafx.h"
#include "Sql.h"

namespace Server
{
Sql::Sql()
{
	mysql_init(Get()); // #Check if needs to be called for every connection
}

Sql::~Sql()
{
	mysql_close(Get());
}

unique_ptr<Sql> Sql::Create()
{
	auto sql = make_unique<Sql>();

	const string host = lib::init::Value("db_host", "localhost");
	const string user = lib::init::Value("db_user", "tiled");
	const string pass = lib::init::Value("db_pass", "");
	const string name = lib::init::Value("db_name", "tiled");
	const auto port = (unsigned)lib::init::Number("db_port", 3306);
	// A socket file (Linux: the box's /run/mysqld/mysqld.sock), else TCP
	const string socket = lib::init::Value("db_socket", "");

	// TLS to a database on another machine (db_ssl); none on this one (an old
	// Connector/C 6.1 fails MySQL 8's handshake)
	if (!lib::init::Flag("db_ssl", host != "localhost" && host != "127.0.0.1"))
	{
		unsigned mode = SSL_MODE_DISABLED;
		mysql_options(sql->Get(), MYSQL_OPT_SSL_MODE, &mode);
	}

	if (!mysql_real_connect(sql->Get(), host.c_str(), user.c_str(), pass.c_str(),
		name.c_str(), port, socket.empty() ? nullptr : socket.c_str(), 0) ||
		sql->FetchError())
	{
		Err("mysql_real_connect {}@{}:{}/{}: {}", user, host, port, name, mysql_error(sql->Get()));
		return nullptr;
	}

	mysql_set_character_set(sql->Get(), "utf8mb4");
	return sql;
}

unsigned Sql::FetchError()
{
	const auto err = mysql_errno(Get());
	if (err != 0)
		Err("MySQL {}: {}", err, mysql_error(Get()));
	return err;
}

string Sql::Quote(const string & value)
{
	string out(value.size() * 2 + 1, '\0');
	out.resize(mysql_real_escape_string(Get(), out.data(), value.data(), (unsigned long)value.size()));
	return "'" + out + "'";
}

bool Sql::Query(const string & query)
{
	DebugLog("Query: {}", query);
	if (mysql_real_query(Get(), query.data(), (unsigned long)query.size()) != 0)
	{
		FetchError();
		return false;
	}
	return true;
}

bool Sql::Insert(const string & table, const vector<string> & columns, const vector<vector<string>> & rows)
{
	// #debug #ifdef DEBUG_SQL
	assert(columns.size() > 0);

	if (rows.empty())
	{
		Err("Tried to run Sql::Insert without rows to insert");
		return false;
	}

	DebugLog("Sql::Insert called for {} table", table);

	auto values = string("(");
	{
		for (szt i = 0; i < columns.size() - 1; i++)
		{
			values += Scape(columns[i], "`", ",");
		}

		values += Scape(columns[columns.size() - 1], "`", ")");
	}

	string data;
	{
		for (szt i = 0; i <= rows.size() - 1; i++)
		{
			const auto & row = rows[i];

			data += "(";

			for (szt j = 0; j < row.size(); j++)
			{
				data += Quote(row[j]);
				data += j == row.size() - 1 ? ")" : ",";
			}

			data += i == rows.size() - 1 ? ";" : ",";
		}
	}

	return Query(format("INSERT INTO `{}` {} VALUES {}", table, values, data));
}

unique_ptr<SelectResult> Sql::Select(const string & table, const vector<string> & columns, const string & extra)
{
	string select;

	for (szt i = 0; i < columns.size() - 1; i++)
	{
		select += Scape(columns[i], "`", ",");
	}

	select += Scape(columns[columns.size() - 1], "`");

	if (!Query(format("SELECT {} FROM `{}`{}", select, table, extra)))
		return nullptr;

	return make_unique<SelectResult>(this);
}

bool Sql::CheckLogin(const string & user, const string & pass)
{
	// The salt is the stored value's first 32 characters, the SHA-256 the rest
	const auto res = Select("players", { "uid" },
		format(" WHERE `username` = {} AND `password` = CONCAT(LEFT(`password`, 32), SHA2(CONCAT(LEFT(`password`, 32), {}), 256)) LIMIT 1",
			Quote(user), Quote(pass)));
	return res && !res->IsEmpty();
}

bool Sql::CreateAccount(const string & user, const string & pass)
{
	const auto res = Select("players", { "uid" }, format(" WHERE `username` = {} LIMIT 1", Quote(user)));
	if (!res || !res->IsEmpty())
		return false;

	string salt;
	for (int i = 0; i < 32; ++i)
		salt += "0123456789abcdef"[std::uniform_int_distribution<int>(0, 15)(Rng())];

	return Query(format("INSERT INTO `players` (`username`, `password`) VALUES ({}, CONCAT({}, SHA2(CONCAT({}, {}), 256)))",
		Quote(user), Quote(salt), Quote(salt), Quote(pass)));
}

SelectResult::SelectResult(Sql * sql)
{
	_ptr = mysql_store_result(sql->Get());
	if (!_ptr)
		return;

	{
		auto total = (SelectValue)mysql_num_rows(_ptr);
		_rows.reserve(total);
		_lengths.resize(total);
		while (total-- > 0)
		{
			_rows.emplace_back(mysql_fetch_row(_ptr));

			auto lengths = mysql_fetch_lengths(_ptr);
			for (szt i = 0; i < mysql_num_fields(_ptr); ++i)
			{
				_lengths[_rows.size() - 1].emplace_back(lengths[i]);
			}
		}
	}

	{
		mysql_field_seek(_ptr, 0);
		const auto count = mysql_num_fields(_ptr);
		_fields.reserve(count);
		for (unsigned i = 0; i < count; ++i)
		{
			_fields.emplace_back(mysql_fetch_field(_ptr));
			_fields_indexes[_fields.back()->name] = i;
		}
	}
}

SelectResult::~SelectResult()
{
	if (_ptr)
		mysql_free_result(_ptr);
}
}
