// see: http://stackoverflow.com/a/13182321
// see: http://gcc.gnu.org/ml/gcc-help/2007-10/msg00328.html

/*
template<typename T, typename U> constexpr size_t offsetof_impl(U T::*member)
{
	return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}
*/

/*
template <typename T, typename U>
constexpr size_t offsetof_impl(T const* t, U T::* a)
{
	return (uintptr_t)t - (uintptr_t)&(t->*a) >= 0 ?
		(uintptr_t)t - (uintptr_t)&(t->*a) :
		(uintptr_t)&(t->*a) - (uintptr_t)t;
}
*/

#undef offsetof
#define offsetof(Type_, Attr_)                          \
    (ptrdiff_t)((unsigned char*)(void*)&((*(Type_*)0).Attr_) - (unsigned char*)(void*)&(*(Type_*)0))

namespace stdext
{
	//
	// helpers
	//

	// http://stackoverflow.com/a/22851790
	template <typename M>
	struct memptr {
	private:
		template <typename C, typename T>
		static T get_value_type(T C::*v);
		template <typename C, typename T>
		static C get_object_type(T C::*v);
	public:
		typedef decltype(get_value_type(static_cast<M>(nullptr))) type;
		typedef decltype(get_object_type(static_cast<M>(nullptr))) class_type;
	};

	//
	// templates
	//

	template <class T, template <class...> class Template>
	struct is_specialization : std::false_type {};

	template <template <class...> class Template, class... Args>
	struct is_specialization<Template<Args...>, Template> : std::true_type {};

	//
	// parameter packs
	//

	template <class T1, class ...T>
	struct parampack_first
	{
		typedef T1 type;
	};

	template <class T1, class ...T>
	struct parampack_last
	{
		typedef typename parampack_last<T...>::type type;
	};

	template <class T1>
	struct parampack_last<T1>
	{
		typedef T1 type;
	};

	//
	// iterators
	//

	template<typename T>
	struct has_const_iterator
	{
	private:
		typedef char                      yes;
		typedef struct { char array[2]; } no;

		template<typename C> static yes test(typename C::const_iterator*);
		template<typename C> static no  test(...);
	public:
		static const bool value = sizeof(test<T>(0)) == sizeof(yes);
		typedef T type;
	};

	template <typename T>
	struct has_begin_end
	{
		template<typename C> static char(&f(typename std::enable_if<
			std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
			typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

		template<typename C> static char(&f(...))[2];

		template<typename C> static char(&g(typename std::enable_if<
			std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
			typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

		template<typename C> static char(&g(...))[2];

		static bool const beg_value = sizeof(f<T>(0)) == 1;
		static bool const end_value = sizeof(g<T>(0)) == 1;
	};

	template<typename T>
	struct is_container
		: std::integral_constant<bool,
			has_const_iterator<T>::value
			&& has_begin_end<T>::beg_value
			&& has_begin_end<T>::end_value
		>
	{ };
}