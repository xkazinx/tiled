#pragma once

namespace Client
{
class Units final : public Broadcaster<UnitOp>
{
public:
	using Op = UnitOp;

	Units();

	void NetOperationProcess() override;

	void Tick();

private:
	void NetOp_Action();
};
}

