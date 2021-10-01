#pragma once

namespace common
{
class Auth : public packet::Serializable
{
public:
	Auth();
	Auth(InPkt * in) { Read(in); }

	~Auth() = default;

	void Write(packet::Out& out) const override {
		out.Write(_user, _pass);
	}

	void Read(packet::In * in) override {
		in->Read(_user, _pass);
	}

	packet::Size GetPacketSize() const override {
		return _user.GetPacketSize() + _pass.GetPacketSize();
	}

	// #todo #auth
	auto IsValid() const {
		return true;
	}
	
	NetString _user, _pass;
};
}