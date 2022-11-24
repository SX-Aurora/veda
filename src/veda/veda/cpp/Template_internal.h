#pragma once

namespace veda {
	namespace cpp {
//------------------------------------------------------------------------------
struct Data {
	std::string			ss;
	std::vector<std::string>	namespaces;
	std::vector<size_t>		substitute;

	inline Data(void) {
		ss.reserve(256);
		namespaces.reserve(16);
		substitute.reserve(32);
	}

	Data(const Data&) = delete;
	Data(const Data&&) = delete;
};

//------------------------------------------------------------------------------
// MangleType
//------------------------------------------------------------------------------
template<typename T> char mangleType(void);
template<> constexpr char mangleType<void>		(void)	{	return 'v';	}
template<> constexpr char mangleType<bool>		(void)	{	return 'b';	}
template<> constexpr char mangleType<char>		(void)	{	return 'c';	}
template<> constexpr char mangleType<short>		(void)	{	return 's';	}
template<> constexpr char mangleType<int>		(void)	{	return 'i';	}
template<> constexpr char mangleType<long>		(void)	{	return 'l';	}
template<> constexpr char mangleType<long long>		(void)	{	return 'x';	}
template<> constexpr char mangleType<unsigned char>	(void)	{	return 'h';	}
template<> constexpr char mangleType<unsigned short>	(void)	{	return 't';	}
template<> constexpr char mangleType<unsigned int>	(void)	{	return 'j';	}
template<> constexpr char mangleType<unsigned long>	(void)	{	return 'm';	}
template<> constexpr char mangleType<unsigned long long>(void)	{	return 'y';	}
template<> constexpr char mangleType<float>		(void)	{	return 'f';	}
template<> constexpr char mangleType<double>		(void)	{	return 'd';	}

//------------------------------------------------------------------------------
struct Template {};
struct Literal {};

//------------------------------------------------------------------------------
template<char c>
inline void mangleIdentifier(Data& data, const int idx) {
	data.ss += c;
	if(idx > 0)
		data.ss += std::to_string(idx-1);
	data.ss += '_';
}

//------------------------------------------------------------------------------
template<typename T>
inline void mangleType(Data& data) {
	if constexpr (std::is_const<T>::value && std::is_pointer<typename std::remove_const<T>::type>::value) {
		mangleType<typename std::remove_const<T>::type>(data);
	} else if constexpr (std::is_const<T>::value) {
		data.ss += 'K';
		mangleType<typename std::remove_const<T>::type>(data);
	} else if constexpr (std::is_pointer<T>::value) {
		data.ss += 'P';
		mangleType<typename std::remove_pointer	<T>::type>(data);
	} else if constexpr (std::is_fundamental<T>::value) {
		data.ss += mangleType<T>();
	} else if constexpr (std::is_base_of<Template, T>::value) {
		mangleIdentifier<'T'>(data, T::idx);
	} else if constexpr (std::is_base_of<Literal, T>::value) {
		data.ss += 'L';
		data.ss += mangleType<typename std::remove_const<decltype(T::value)>::type>();
		data.ss += std::to_string(T::value);
		data.ss += 'E';
	} else if constexpr (std::is_class<T>::value) {
		data.ss += typeid(T).name();
	} else {
		throw VEDA_ERROR_INVALID_ARGS;
	}
}

//------------------------------------------------------------------------------
// Substitution
//------------------------------------------------------------------------------
template<typename T, size_t idx=0>
inline void addSubstituteInner(Data& data) {
	auto code = typeid(T).hash_code();
	for(auto&& current : data.substitute)
		if(current == code) // already exists
			return;

	data.substitute.emplace_back(code);
}

//------------------------------------------------------------------------------
inline void addSubstitute(Data& data, const std::string_view sub) {
	for(auto&& it : data.namespaces)
		if(it == sub)
			return;

	data.namespaces.emplace_back(sub);
	data.substitute.emplace_back(-data.namespaces.size());
}

//------------------------------------------------------------------------------
template<typename T>
inline void addSubstitute(Data& data) {
	if constexpr(std::is_pointer<T>::value) {
		addSubstitute<typename std::remove_pointer<T>::type>(data);
		addSubstituteInner<T>(data);
	} else if constexpr(std::is_const<T>::value) {
		addSubstitute<typename std::remove_const<T>::type>(data);
		addSubstituteInner<T>(data);
	} else if constexpr(std::is_fundamental<T>::value) {
		// fundamentals don't get substituted
	} else if constexpr(std::is_base_of<Template, T>::value) {
		addSubstituteInner<T>(data);
	} else if constexpr(std::is_base_of<Literal, T>::value) {
		// literals don't get substituted
	} else if constexpr(std::is_class<T>::value) {
		const std::string_view name(typeid(T).name());
		if(name[0] == 'N') {
			size_t pos = 1;
			std::string_view view;
			while(true) {
				int offset = 0;
				int cnt = 0;
				while(true) {
					char c = name[pos + offset];
					if(c >= '0' && c <= '9') {
						cnt = cnt * 10 + int(c - '0');
						offset++;
					} else {
						if(c == 'I') { // template within struct name
							auto start	= pos + offset;
							auto end	= name.find('E', start);
							auto tmpl	= name.substr(start, end - start + 1);
							addSubstitute(data, tmpl);
						}
						break;
					}
				}

				if(cnt) {
					if(view.size())
						addSubstitute(data, view);
					view = name.substr(pos, cnt + offset);
					pos = pos + cnt + offset;
				} else {
					break;
				}
			}
		}
		addSubstituteInner<T>(data);
	} else {
		throw VEDA_ERROR_INVALID_ARGS;
	}
}

//------------------------------------------------------------------------------
// MangleName
//------------------------------------------------------------------------------
inline void checkName(const std::string_view name) {
	const auto c = name[0];
	if(!(
		(c >= 'A' && c <= 'Z') ||
		(c >= 'a' && c <= 'z') ||
			c == '_'
	))
		throw VEDA_ERROR_INVALID_ARGS;

	for(auto c : name) {
		if(!(
			(c >= 'A' && c <= 'Z') ||
			(c >= 'a' && c <= 'z') ||
			(c >= '0' && c <= '9') ||
			c == '_' ||
			c == ':'
		))
			throw VEDA_ERROR_INVALID_ARGS;
	}
}

//------------------------------------------------------------------------------
inline bool mangleName(Data& data, const std::string_view view) {
	constexpr auto npos = std::string_view::npos;

	bool is_namespace = false;
	size_t start = 0;
	std::string ns;
	while(true) {
		size_t end = view.find("::", start);
		if(end == npos) {
			auto sub = view.substr(start);
			data.ss += ns;
			data.ss += std::to_string(sub.size());
			data.ss += sub;
			break;
		} else {
			if(!is_namespace) {
				is_namespace = true;
				ns.reserve(64);
				data.ss += 'N';
			}
			auto sub = view.substr(start, end-start);
			auto len = std::to_string(sub.size());
			ns += len;
			ns += sub;
			addSubstitute(data, ns);
			start = end + 2;
		}
	}

	return is_namespace;
}

//------------------------------------------------------------------------------
// Mangle Template
//------------------------------------------------------------------------------
template<typename T, typename... R>
inline void mangleTemplate(Data& data) {
	mangleType<T>(data);
	addSubstitute<T>(data);
	if constexpr (sizeof...(R) > 0)
		mangleTemplate<R...>(data);
}

//------------------------------------------------------------------------------
// MangleArgs
//------------------------------------------------------------------------------
template<typename T>
inline bool mangleArgsSubstitute(Data& data) {
	size_t idx = 0;
	for(auto&& current : data.substitute) {
		if(current == typeid(T).hash_code()) {
			mangleIdentifier<'S'>(data, idx);
			return false;
		}
		idx++;
	}
	return true;
}

//------------------------------------------------------------------------------
template<typename T>
inline bool mangleArgsTemplate(Data& data) {
	if constexpr (std::is_base_of<Template, T>::value) {
		if(mangleArgsSubstitute<T>(data)) {
			mangleIdentifier<'T'>(data, T::idx);
			addSubstituteInner<T>(data);
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
template<typename T>
struct remove_const_pointer : public
	std::conditional<
		std::is_const<T>::value && std::is_pointer<typename std::remove_const<T>::type>::value,
		typename std::remove_const<T>::type, T
	>
{};

//------------------------------------------------------------------------------
template<typename T, typename... R>
inline void mangleArgs(Data& data) {
	if constexpr (std::is_reference<T>::value)
		throw VEDA_ERROR_INVALID_ARGS;

	using TT = typename remove_const_pointer<T>::type;

	if(mangleArgsTemplate<TT>(data)) {
		// nothing else to do
	} else if(mangleArgsSubstitute<TT>(data)) {
		mangleType<TT>(data);
		addSubstitute<TT>(data);
	}

	if constexpr (sizeof...(R) > 0)
		mangleArgs<R...>(data);	
}

//------------------------------------------------------------------------------
	}
}
