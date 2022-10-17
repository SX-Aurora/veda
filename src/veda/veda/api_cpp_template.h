#pragma once
#include <regex>

//------------------------------------------------------------------------------
class vedaModuleGetCPPFunction {
	using Stream = std::ostringstream;
	using Lookup = std::map<size_t, std::string>;
	
	static inline void mangleName(Stream& ss, const std::string name) {
		// check if name is valid
		static const std::regex checkValidNames("[A-Za-z_]+[A-Za-z0-9_:]*");
		std::smatch smatch;
		if(!std::regex_match(name, smatch, checkValidNames))
			throw VEDA_ERROR_INVALID_ARGS;

		// we don't need to check for reserved words as they anyway would return
		// no function pointer
		const std::string_view view(name);
		size_t start = 0;
		while(true) {
			size_t end = view.find("::", start);
			if(end == std::string::npos) {
				auto sub = view.substr(start);
				ss << sub.size() << sub;
				break;
			} else {
				auto sub = view.substr(start, end-start);
				ss << sub.size() << sub;
				start = end + 2;
			}
		}
	}

	template<typename T>
	static constexpr char typeChar(void) {
		#define TYPE(TT, C)	else if constexpr (std::is_same<T, TT>::value) return C;
		if constexpr (false);
		TYPE(void,			'v')
		TYPE(bool,			'b')
		TYPE(char,			'c')
		TYPE(short,			's')
		TYPE(int,			'i')
		TYPE(long,			'l')
		TYPE(long long,			'x')
		TYPE(unsigned char,		'h')
		TYPE(unsigned short,		't')
		TYPE(unsigned int,		'j')
		TYPE(unsigned long,		'm')
		TYPE(unsigned long long,	'y')
		TYPE(float,			'f')
		TYPE(double,			'd')
		// TODO: else static_assert(!std::is_same<T, T>::value);
		#undef TYPE
		return 0;
	}

	template<char SUBSTITUTE, int START, typename... X>
	class Mangle {
		static std::string s_mangled;

		template<typename T>
		static inline std::string type(void) {
			using TP = typename std::remove_pointer<T>::type;
			using TC = typename std::remove_const<T>::type;

			constexpr bool is_ptr	= std::is_pointer<T>::value;
			constexpr bool is_const	= is_ptr && std::is_const<TP>::value;	// const only gets set if they are also pointers

			std::string type;
			if constexpr (is_ptr)	type.push_back('P');
			if constexpr (is_const)	type.push_back('K');
			type.push_back(typeChar<TC>());

			return type;
		}

		/*template<int value>
		static inline std::string type(void) {
			std::string type("L");
			type.push_back(typeChar<int>());

			if constexpr (value < 0);
				type.push_back('n');

			type.append(std::to_string(std::abs(value)));
			type.push_back('E');

			return type;
		}*/

		template<typename T>
		static inline std::string substitute(Lookup& l, int& cnt) {
			auto hash	= typeid(T).hash_code();
			auto x		= l.emplace(hash, "");
			auto& r		= x.first->second;
			if(x.second) {
				r.push_back(SUBSTITUTE);
				if(cnt >= 0)
					r.append(std::to_string(cnt));
				r.push_back('_');
				cnt++;
				return type<T>();
			}
			return r;
		}

		template<typename T>
		static inline void mangleType2(Stream& ss, Lookup& l, int& cnt) {
			using TP = typename std::remove_pointer<T>::type;
			using TC = typename std::remove_const<T>::type;

			constexpr bool is_ptr	= std::is_pointer<T>::value;
			constexpr bool is_const	= is_ptr && std::is_const<TP>::value;

			if constexpr (SUBSTITUTE == 'T' || (!is_ptr && !is_const)) {
				ss << type<T>();
			} else {
				if constexpr (is_const && is_ptr) {
					substitute<TP>(l, cnt);
					ss << substitute<T>(l, cnt);
				} else {
					ss << substitute<T>(l, cnt);
				}
			}
		}

		template<typename T, typename... Remaining>
		static inline void mangleType(Stream& ss, Lookup& l, int cnt) {
			mangleType2<T>(ss, l, cnt);
			if constexpr (sizeof...(Remaining))
				mangleType<Remaining...>(ss, l, cnt);
		}
	public:
		static inline const std::string& get(Lookup& l) {
			if constexpr (sizeof...(X)) {
				if(s_mangled.empty()) {
					Stream ss;
					mangleType<X...>(ss, l, START);
					s_mangled = ss.str();
				}
			}
			return s_mangled;
		}
	};

public:
	template<typename R>
	struct Return {

	};

	template<typename... T>
	struct Template {
		template<typename R>
		struct Return {
			template<typename... A>
			static inline VEDAresult Args(VEDAfunction* func, VEDAmodule mod, const char* name) {
				Stream ss;
				Lookup l;
				ss << "_Z";
				mangleName(ss, name);

				auto t = Mangle<'T', -1, T...>::get(l);
				if(t.size())
					ss << 'I' << t << 'E';

				auto a = Mangle<'S', 0, R, A...>::get(l);
				ss << a;

				auto sname = ss.str();
				printf("%s R: %c T: %s A: %s -> %s\n", name, typeChar<R>(), t.c_str(), a.c_str(), sname.c_str());
				return vedaModuleGetFunction(func, mod, sname.c_str());
			}
		};
	};
};

template<char SUBSTITUTE, int START, typename... X> std::string vedaModuleGetCPPFunction::Mangle<SUBSTITUTE, START, X...>::s_mangled;

template<typename T, T V>
struct vedaLiteral {};

//------------------------------------------------------------------------------