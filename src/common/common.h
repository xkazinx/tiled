#pragma once
#include <cassert>
#include <netops.h>

#define DEBUG_PACKET

class Socket;

namespace common
{
namespace tile
{
	enum class Type : u8
	{
		Grass,
		Water,
		Rock,
		Max
	};

	using LineSize = u8;
	static constexpr float kSize = 32.f;
	static constexpr float kHalfSize = kSize / 2.f;

#ifdef CLIENT
	using ColorArray = const array<sf::Color, (szt)Type::Max>;
	extern ColorArray kColors;
#endif

}

namespace player
{
	using Idx = u16;
	constexpr Idx kNullIdx = 0;
	constexpr Idx kMax = 512;
}

namespace packet
{
	class Out;
	class In;

	using OutPacketPtr = std::unique_ptr<Out>;
	using Size = enet_uint32;

	class Serializable
	{
	public:
		// virtual ~Serializable() {} // #todo?
		virtual void Write(Out& out) const = 0;
		virtual void Read(In* in) = 0;
		virtual Size GetPacketSize() const = 0;
	};

	#define SERIALIZE_IMPL(params)					\
		void Write(Out & out) const override	\
		{											\
			out.Write(params);						\
		}											\
		void Read(In * in) override			\
		{											\
			in->Read(params);						\
		}											\
		Size GetPacketSize() const override			\
		{											\
			return Sizer::Get(params);				\
		}

	class Base
	{
		char* _ptr;
		bool _overflow;
		Size _size;

		virtual void Move(const Size sz) = 0;

	public:
		virtual ~Base() = default;
		Base(ENetPacket* enet_packet, const Size sz);
		
		template<typename T = typename std::enable_if_t<!std::is_pointer_v<T>>>
		auto GetPointer() const { return reinterpret_cast<T*>(_ptr); }
		bool IsOverflow() const;

		// #todo
		static std::string GetIP(ENetPeer* peer);

		class Sizer
		{
		public:
			template<typename T, typename ... Args, typename = std::enable_if_t<!std::is_pointer_v<T>>>
			static Size Get(const T& val, const Args & ... args)
			{
				return _Get(val) + Get(std::forward<const Args>(args)...);
			}

		private:
			static Size Get() { return 0; }

			template <typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
			static Size _Get(const T& val)
			{
				if constexpr (std::is_same_v<T, std::string>)
				{
					return sizeof(enet_uint16) + val.size();
				}
				else if constexpr (std::is_base_of_v<Serializable, T>)
				{
					return val.GetPacketSize();
				}
				else if constexpr (std::is_same_v<char*, T>)
				{
					return 5;
				}
				else
				{
					return sizeof(val);
				}
			}

		};

	protected:
		bool ProcessOverflow(const bool extra_condition);
		virtual bool HasOverflowed(const Size sz) = 0;

		Size GetSize() const { return _size; }
		Size& GetLength() const { return _enet_packet->dataLength; }
		void _Move(const Size pos, const bool is_out);

		ENetPacket* _enet_packet;
	};

	class Out final : public Base
	{
	public:
		// Create by size and then write in another argument
		Out(const Size sz);
		~Out() override;

		// #todo packet::CreateStatic // packet::CreateDynamic
		template<typename T, typename = std::enable_if_t<!std::is_pointer<T>::value>, typename ... Args>
		static OutPacketPtr CreateStatic(const NetOp net_op, const T& val, const Args & ... args)
		{
			auto pkt = CreateDynamic(net_op, Sizer::Get(val, std::forward<const Args>(args)...));
			pkt->Write(val, std::forward<const Args>(args)...);
			return pkt;
		}

		static OutPacketPtr CreateDynamic(const NetOp net_op, const Size sz)
		{
			auto pkt = std::make_unique<Out>(sz + sizeof(net_op));
			pkt->Write(net_op);
			return pkt;
		}

		void Send(ENetPeer* peer, const enet_uint8 channel = 0);
		void Send(class ::Socket* sock, const enet_uint8 channel = 0);
		
		void Move(const Size sz) override;
		bool HasOverflowed(const Size sz) override;

		template<typename T, typename ... Args>
		void Write(const T& t, const Args & ...args)
		{
			_Write(t);
			Write(std::forward<const Args>(args)...);
		}

		template<typename T, std::size_t N>
		void Write(const std::array<T, N>& arr)
		{
			for (const auto& val : arr)
				Write(val);
		}

		template<typename T, std::size_t N>
		void Write(const T(&arr)[N])
		{
			for (std::size_t i = 0; i < N; i++)
				Write(arr[i]);
		}

	private:
		void Write() {}

		template<typename T, typename = std::enable_if_t<!std::is_pointer<T>::value>>
		void _Write(const T& v)
		{
			if constexpr (std::is_same<T, std::string>::value)
			{
				const auto sz = (enet_uint16)v.size();
				_Write(sz);

				if (HasOverflowed(sz))
				{
					return;
				}

				std::copy(&v[0], &v[0] + sz, GetPointer<char>());
				Move(sz);
			}
			else if constexpr (std::is_base_of_v<Serializable, T>)
			{
				v.write(*this);
			}
			else if constexpr(std::is_same_v<char*, T>)
			{
				// #todo
			}
			else
			{
				const auto sz = Sizer::Get(v);
				if (HasOverflowed(sz))
				{
					return;
				}

				auto ptr = GetPointer<T>();
				*ptr = v;
				Move(sz);
			}
		}
	};

	class In final : public Base
	{
	public:
		In(ENetPacket* p);
		~In() override;

		bool HasOverflowed(const Size sz) override;
		void Move(const Size sz) override;

		template<typename T, typename ... Args>
		void Read(T& t, Args & ...args)
		{
			_Read(t);
			Read(args...);
		}

		template<typename T, std::size_t N>
		void Read(std::array<T, N>& arr)
		{
			for (auto& val : arr)
				Read(val);
		}

		template<typename T, std::size_t N>
		void Read(T(&arr)[N])
		{
			for (std::size_t i = 0; i < N; i++)
				Read(arr[i]);
		}

		template<typename T>
		T ReadAs()
		{
			T value;
			Read(value);
			return value;
		}

	private:
		void Read() {}

		template<typename T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
		void _Read(T& v)
		{
			if constexpr (std::is_same_v<T, std::string>)
			{
				if (HasOverflowed(sizeof(enet_uint16)))
					return;

				enet_uint16 sz;
				Read(sz);

				if (HasOverflowed(sz))
					return;

				v.resize(sz);
				std::copy(GetPointer<char>(), GetPointer<char>() + sz, &v[0]);
				Move(sz);
			}
			else if constexpr (std::is_base_of_v<Serializable, T>)
			{
				v.Read(this);
			}
			else if constexpr (std::is_same_v<char*, T>)
			{
				// #todo
			}
			else
			{
				const auto sz = Sizer::Get(v);
				if (HasOverflowed(sz))
				{
					return;
				}

				v = *GetPointer<T>();
				Move(sz);

#ifdef DEBUG_PACKET
				// #todo std
				if constexpr (std::is_same_v<NetOp, T>)
				{
					cout << "Reading NetOp: " << (int)v << endl;
				}
#endif
			}
		}
	};
}

namespace broadcaster
{
	class InOperationBase
	{
	public:
		InOperationBase() = default;
		explicit InOperationBase(packet::In & pkt) : _pkt(&pkt) {}
		auto GetPacket() const { return _pkt; }

	private:
		packet::In * _pkt = nullptr;
	};

	enum class DummyOp { None };

	template<class Op>
	class Base
	{
	public:
		using Peer = ENetPeer;

		explicit Base(const NetOp net_op) : _net_op(net_op) {}
		virtual ~Base() = default;

		auto GetOperation() const { return _op; }

		template<typename ... Args>
		packet::OutPacketPtr&& StaticPacket(Op op, Args &&... args) const
		{
			return std::move(packet::Out::CreateStatic(_net_op, op, args...)); // #why not forward here?
		}

		template<typename ... Args>
		packet::OutPacketPtr&& DynamicPacket(Op op, Args &&... args) const
		{
			return std::move(packet::Out::CreateDynamic(_net_op, op, std::forward<Args>(args)...));
		}
		
	protected:
		Op _op;
		NetOp _net_op;
	};
}

using PlayerIdx = player::Idx;
using InPkt = packet::In;
using OutPkt = packet::Out;
using OutPktRef = packet::OutPacketPtr&;
}

using namespace common;