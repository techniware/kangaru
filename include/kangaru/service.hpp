#ifndef KGR_KANGARU_INCLUDE_KANGARU_SERVICE_HPP
#define KGR_KANGARU_INCLUDE_KANGARU_SERVICE_HPP

#include <type_traits>

#include "detail/utils.hpp"
#include "detail/single.hpp"
#include "generic.hpp"

namespace kgr {

template<typename... Args>
struct Dependency {};

template<typename, typename = Dependency<>>
struct SingleService;

template<typename Type, typename... Deps>
struct SingleService<Type, Dependency<Deps...>> : GenericService<SingleService<Type, Dependency<Deps...>>, Type>, Single {
	private: using Parent = GenericService<SingleService<Type, Dependency<Deps...>>, Type>;
	
public:
	using typename Parent::Self;
	using Parent::Parent;
	
	static auto construct(Inject<Deps>... deps) -> decltype(inject(deps.forward()...)) {
		return inject(deps.forward()...);
	}

	Type& forward() {
		return this->instance();
	}
	
	template<typename T, typename... Args>
	static detail::function_result_t<T> call(Type& instance, T method, Args&&... args) {
		return (instance.*method)(std::forward<Args>(args)...);
	}
};

template<typename, typename = Dependency<>>
struct Service;

template<typename Type, typename... Deps>
struct Service<Type, Dependency<Deps...>> : GenericService<Service<Type, Dependency<Deps...>>, Type> {
	private: using Parent = GenericService<Service<Type, Dependency<Deps...>>, Type>;
	
public:
	using typename Parent::Self;
	using Parent::Parent;
	
	template<typename... Args>
	static auto construct(Inject<Deps>... deps, Args&&... args) -> decltype(inject(deps.forward()..., std::declval<Args>()...)) {
		return inject(deps.forward()..., std::forward<Args>(args)...);
	}

	Type forward() {
		return std::move(this->instance());
	}
	
	template<typename T, typename... Args>
	static detail::function_result_t<T> call(Type& instance, T method, Args&&... args) {
		return (instance.*method)(std::forward<Args>(args)...);
	}
};

template<typename, typename = Dependency<>>
struct UniqueService;

template<typename Type, typename... Deps>
struct UniqueService<Type, Dependency<Deps...>> : GenericService<UniqueService<Type, Dependency<Deps...>>, std::unique_ptr<Type>> {
	private: using Parent = GenericService<UniqueService<Type, Dependency<Deps...>>, std::unique_ptr<Type>>;
	
public:
	using typename Parent::Self;
	using Parent::Parent;
	
	template<typename... Args>
	static auto construct(Inject<Deps>... deps, Args&&... args)
	-> decltype(inject(std::unique_ptr<Type>{new Type{deps.forward()..., std::declval<Args>()...}})) {
		return inject(std::unique_ptr<Type>{new Type{deps.forward()..., std::forward<Args>(args)...}});
	}
	
	std::unique_ptr<Type> forward() {
		return std::move(this->instance());
	}
	
	template<typename T, typename... Args>
	static detail::function_result_t<T> call(std::unique_ptr<Type>& instance, T method, Args&&... args) {
		return ((*instance).*method)(std::forward<Args>(args)...);
	}
};

template<typename, typename = Dependency<>>
struct SharedService;

template<typename Type, typename... Deps>
struct SharedService<Type, Dependency<Deps...>> : GenericService<SharedService<Type, Dependency<Deps...>>, std::shared_ptr<Type>>, Single {
	private: using Parent = GenericService<SharedService<Type, Dependency<Deps...>>, std::shared_ptr<Type>>;
	
public:
	using typename Parent::Self;
	using Parent::Parent;

	static auto construct(Inject<Deps>... deps) -> decltype(inject(std::make_shared<Type>(deps.forward()...))) {
		return inject(std::make_shared<Type>(deps.forward()...));
	}
	
	std::shared_ptr<Type> forward() {
		return this->instance();
	}
	
	template<typename T, typename... Args>
	static detail::function_result_t<T> call(std::shared_ptr<Type>& instance, T method, Args&&... args) {
		return ((*instance).*method)(std::forward<Args>(args)...);
	}
};

template<typename T>
struct AbstractService {
	virtual T& forward() = 0;
};

template<typename T>
struct AbstractSharedService {
	virtual std::shared_ptr<T> forward() = 0;
};

} // namespace kgr

#endif // KGR_KANGARU_INCLUDE_KANGARU_SERVICE_HPP
