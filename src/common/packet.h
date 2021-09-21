#pragma once

namespace common
{
	//class Socket;

	namespace packet
	{
		class Out;
		class In;

		using OutPacketPtr = std::unique_ptr<Out>;
		using Size = enet_uint32;

		class Serializable
		{
		public:
			Serializable() = default;
			virtual ~Serializable() = default;

			virtual void Write(Out & out) const = 0;
			virtual void Read(In* in) = 0;

			virtual Size GetPacketSize() const = 0;
		};

		class SerializableString final : public Serializable
		{
		public:
			using Size = enet_uint16;
			static string kDummy;

			~SerializableString() override;

			// String with any length
			explicit SerializableString(string& str)
				: _data(nullptr), _sz(0), _ref(str)
			{

			}

			// String with fixed length
			explicit SerializableString(string& str, Size sz)
				: _data(nullptr), _sz(sz), _ref(str)
			{
				if (_sz == 0)
				{
					throw std::exception("Trying to construct a fixed SerializableString with 0 length");
				}
			}
			
			// char* with any length
			explicit SerializableString(const char* data)
				: _data(data), _sz(0), _ref(kDummy)
			{

			}

			// char* with fixed length
			explicit SerializableString(const char* data, Size sz)
				: _data(data), _sz(sz), _ref(kDummy)
			{
				if (_sz == 0)
				{
					throw std::exception("Trying to construct a fixed char* SerializableString with 0 length");
				}
			}

			void Write(Out & out) const override;

			void Read(In * in) override;

			packet::Size GetPacketSize() const override;

			auto IsString() const
			{
				return &_ref != &kDummy && !_data;
			}

			auto IsFixedString() const
			{
				return !_data && _sz == 0;
			}

			auto IsChar() const
			{
				return _data && _sz != 0;
			}

			auto IsFixedChar() const
			{
				return _data && _sz == 0;
			}

			mutable const char* _data;
			Size _sz;
			string& _ref = kDummy;
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
			Size _size;

			virtual void Move(const Size sz) = 0;

		public:
			virtual ~Base() = default;
			Base(ENetPacket* packet, const Size sz);

			template<typename T = typename enable_if_t<!is_pointer_v<T>>>
			auto GetPointer() const { return reinterpret_cast<T*>(_ptr); }

			class Sizer
			{
			public:
				template<typename T, typename ... Args, typename = enable_if_t<!is_pointer_v<T>>>
				static Size Get(const T& val, const Args & ... args)
				{
					return _Get(val) + Get(forward<const Args>(args)...);
				}

			private:
				static Size Get() { return 0; }

				template <typename T, typename = enable_if_t<!is_pointer_v<T>>>
				static Size _Get(const T& val)
				{
					if constexpr (is_base_of_v<Serializable, T>)
					{
						return val.GetPacketSize();
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
			bool _overflow;
		};

		class Out final : public Base
		{
		public:
			// Create by size and then write in another argument
			Out(const Size sz);
			~Out() override;

			void Move(const Size sz) override;
			bool HasOverflowed(const Size sz) override;

			// #todo packet::CreateStatic // packet::CreateDynamic
			template<typename T, typename = enable_if_t<!std::is_pointer_v<T>>, typename ... Args>
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

			void Send(ENetPeer* peer, const enet_uint8 channel = 0) const;

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

			template<typename T, typename = enable_if_t<!is_pointer_v<T>>>
			void _Write(const T& v)
			{
				if constexpr (is_base_of_v<Serializable, T>)
				{
					v.Write(*this);
				}
				else
				{
					const auto sz = Sizer::Get(v);
					if (HasOverflowed(sz))
						return;

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

			template<typename T, typename = enable_if_t<!is_pointer_v<T>>>
			void _Read(T& v)
			{
				/*constexpr auto is_string = is_same_v<string, T>;
				if constexpr (is_string || is_same_v<Serializable::String, T>)
				{
					if (HasOverflowed(sizeof(enet_uint16)))
						return;

					enet_uint16 sz;
					_Read(sz);

					if (HasOverflowed(sz))
						return;
					
					v.resize(sz);
					std::copy(GetPointer<char>(), GetPointer<char>() + sz, &v[0]);
					Move(sz);
				}*/
				if constexpr (is_base_of_v<Serializable, T>)
				{
					v.Read(this);
				}
				else
				{
					const auto sz = Sizer::Get(v);
					if (HasOverflowed(sz))
						return;

					v = *GetPointer<T>();
					Move(sz);

#ifdef DEBUG_PACKET
					// #todo std
					if constexpr (is_same_v<NetOp, T>)
					{
						cout << "Reading NetOp: " << (int)v << endl;
					}
#endif
				}
			}
		};
	}
	
	using InPkt = packet::In;
	using OutPkt = packet::Out;
	using OutPktRef = packet::OutPacketPtr&;
}