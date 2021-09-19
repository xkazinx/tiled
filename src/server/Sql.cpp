#include "stdafx.h"
#include "Sql.h"

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