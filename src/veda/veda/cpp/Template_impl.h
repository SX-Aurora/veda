using cR	= typename clear<R>::type;
using RFunction = CFunction::Return<cR>;

template<typename... A>
struct Args : public RFunction {
	static inline RFunction init(const Module mod, const std::string_view name) {
		if(!name.begin())
			VEDA_THROW(VEDA_ERROR_INVALID_ARGS);
		
		static std::mutex mutex;
		static std::map<Module, std::map<std::string, RFunction>> map;

		const std::lock_guard<std::mutex> __lock__(mutex); // ensure thread safety

		auto& ref = map[mod];
		auto it   = ref.emplace(name, RFunction());

		if(it.second) {	
			try {
				cpp::checkName(name);
				cpp::Data data;

				data.ss += "_Z"; // function
				const auto is_namespace		= cpp::mangleName(data, name);
				constexpr auto is_template	= sizeof...(T) > 0;
				if constexpr (is_template) {
					data.ss += 'I';
					cpp::addSubstitute(data, ""); // I don't think we need to know the template
					cpp::mangleTemplate<T...>(data);
					data.ss += 'E';
					}

				if(is_namespace)
					data.ss += 'E';

				if constexpr (is_template)
					cpp::mangleArgs<R>(data);
				
				if constexpr (sizeof...(A) == 0)	cpp::mangleArgs<void>(data);
				else					cpp::mangleArgs<A...>(data);
				
				it.first->second = RFunction(mod, data.ss);
			} catch(const cpp::Exception& e) {
				ref.erase(it.first);
				throw e;
			}
		}

		return it.first->second;
	}

	inline Args(const Module mod, const std::string_view name) :
		RFunction(init(mod, name))
	{}

	struct Launcher final : public RFunction::Launcher {
		inline cpp::RType<cR> operator()(typename clear<A>::type... args) {
			return RFunction::Launcher::operator()(args...);
		}

		using RFunction::Launcher::Launcher;
		friend class Args<A...>;
	};
	
	inline Launcher operator[](const StreamId stream) const {
		return {*this, stream};
	}

	inline cpp::RType<cR> operator()(typename clear<A>::type... args) const {
		return operator[](0)(args...);
	}
};