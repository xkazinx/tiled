#pragma once

namespace Server
{
struct Player;

struct Units : Broadcaster<UnitOp>
{
	using Op = UnitOp;

	array<unique_ptr<Player>, player::kMax> _array;
	vector<Player *> _list;

	Units();
	~Units();

	virtual void NetOperationProcess() override;

private:
	void NetOp_Action();
};
}
