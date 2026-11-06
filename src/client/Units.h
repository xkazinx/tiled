#pragma once

namespace Client
{
struct Units final : Broadcaster<UnitOp>
{
	using Op = UnitOp;

	Units();

	void NetOperationProcess() override;

	void Tick();

private:
	void NetOp_Action();
	void NetOp_Spawn();
	void NetOp_Remove();
};
}
