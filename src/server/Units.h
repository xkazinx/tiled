#pragma once

namespace Server
{
class Player;

class Units : public Broadcaster<UnitOp>
{
public:
	using Op = UnitOp;

	array<unique_ptr<Player>, player::kMax> _array;
	vector<Player*> _list;

	Units();
	~Units();

	virtual void NetOperationProcess() override;

private:
	void NetOp_Action();
};
}