Debugging
=========

kangaru is a heavily templated library. It's commonly known that library that rely a lot on templates are hard to diagnostic,
and as soon as something goes wrong with user defined types, you'll get a huge monologue of errors from the compiler.

Surprisingly, it's not the case for kangaru.

Most of kangaru function are guarded by sfinae constraints. It will recursively detect errors in constructors, dependencies and definitions.
For GCC, we used a small trick with templated constructors and default values that allows us to call a static assertation when a deleted function is invoked.

This is the aclual GCC output when we wrongly send too many parameter in `PathprinterService` in example1:

    example1.cpp: In function 'int main()':
    example1.cpp:44:57: error: use of deleted function 'kgr::detail::Sink kgr::Container::service(kgr::detail::ServiceError<T, kgr::detail::identity_t<Args>...>, Args&& ...) [with T = PathPrinterService; Args = {}; typename std::enable_if<(! std::is_default_constructible<kgr::detail::ServiceError<T, Args ...> >::value), int>::type <anonymous> = 0]'
      auto printer1 = container.service<PathPrinterService>(2);
                                                             ^
    In file included from ../../include/kangaru/kangaru.hpp:4:0,
                     from example1.cpp:5:
    ../../include/kangaru/container.hpp:114:15: note: declared here
      detail::Sink service(detail::ServiceError<T, detail::identity_t<Args>...>, Args&&...) = delete;
                   ^~~~~~~
    In file included from ../../include/kangaru/container.hpp:11:0,
                     from ../../include/kangaru/kangaru.hpp:4,
                     from example1.cpp:5:
    ../../include/kangaru/detail/error.hpp: In instantiation of 'kgr::detail::ServiceError<T, Args>::ServiceError(Arg&&) [with Arg = int; Service = PathPrinterService; typename std::enable_if<((kgr::detail::is_service<Service>::value && (! typename kgr::detail::is_service_constructible_helper<Service, Arg, Args ...>::type:: value)) && typename kgr::detail::is_service_constructible_helper<Service>::type:: value), int>::type <anonymous> = 0; T = PathPrinterService; Args = {}]':
    example1.cpp:44:57:   required from here
    ../../include/kangaru/detail/error.hpp:50:3: error: static assertion failed: The service type is not constructible given passed arguments to kgr::Container::service(...).
       static_assert(false_t<Arg>::value, "The service type is not constructible given passed arguments to kgr::Container::service(...).");

Notice that the last message is the result of a static assertation.

## Explicit debugging

If you don't have GCC or you explicitly want to know what's wrong with a particular service, you can call `kgr::debug::service<T>(Args...)`.
Simply call that function just like it would be `kgr::Container::service<T>()`, and compile!

Let's trigger the same error as above with this code:

    kgr::debug::service<PathPrinterService>(2);

This is the compiler output (Clang):

    In file included from example1.cpp:5:
    In file included from ../../include/kangaru/kangaru.hpp:4:
    In file included from ../../include/kangaru/detail/../container.hpp:11:
    ../../include/kangaru/detail/error.hpp:50:3: error: static_assert failed "The service type is not constructible given passed arguments to kgr::Container::service(...)."
                    static_assert(false_t<Arg>::value, "The service type is not constructible given passed arguments to kgr::Container::service(...).");
                    ^             ~~~~~~~~~~~~~~~~~~~
    ../../include/kangaru/debug.hpp:11:35: note: in instantiation of function template specialization 'kgr::detail::ServiceError<PathPrinterService>::ServiceError<int, PathPrinterService, 0>' requested here
            detail::ServiceError<T, Args...> error{std::forward<U>(u)};
                                             ^
    example1.cpp:50:14: note: in instantiation of function template specialization 'kgr::debug::service<PathPrinterService, int>' requested here
            kgr::debug::service<PathPrinterService>(2);
                        ^
    1 error generated.

As you can see, the compiler is effectively stating that the service is not constructible given passed arguments.

If `kgr::debug::service<T>(Args...)` resolve to a deleted function, it means that there is no error detected with that service.

[Next chapter: Generic Services](section9_generic.md)
