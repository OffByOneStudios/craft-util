
/* T:
See these tutorials:
* http://eli.thegreenplace.net/2014/variadic-templates-in-c/
*/

/******************************************************************************
** Detection Templates
******************************************************************************/

// From http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf
// The *_v methods appear to crash MSVC
// Also:
//   * http://stackoverflow.com/a/30860338
//   * http://stackoverflow.com/a/30848101

inline namespace _t
{
    
	template< class... >
	using
		void_t = void;


	// `detect`

	template < class, template<class> class, class = void_t< > >
	struct
		detect
		: std::false_type { };
	template < class T, template<class> class Op>
	struct
		detect< T, Op, void_t<Op<T>> >
		: std::true_type { };

	// `detector`

	template 
		< class Default
		, class // Always void
		, template<class...> class Op
		, class... Args
		>
	struct
		detector
	{
		using value_t = std::false_type;
		using type = Default;
	};

	template
	< class Default
		, template<class...> class Op
		, class... Args
	>
	struct
		detector< Default, void_t< Op<Args...> >, Op, Args...>
	{
		using value_t = std::true_type;
		using type = Op<Args...>;
	};


	// `nonesuch`

	struct
		nonesuch
	{
		nonesuch() = delete;
		~nonesuch() = delete;
		nonesuch(nonesuch const&) = delete;
		void 
			operator = (nonesuch const&) = delete;
	};


	// `is_detected`

	template< template<class...> class Op, class... Args >
	using
		is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

	/*
	template< template<class...> class Op, class... Args >
	constexpr bool
		is_detected_v = is_detected<Op, Args...>::value;
	*/


	// `detected_t`

	template< template<class...> class Op, class... Args >
	using
		detected_t = typename detector<nonesuch, void, Op, Args...>::type;


	// `detected_or`

	template< class Default, template<class...> class Op, class... Args >
	using
		detected_or = detector<Default, void, Op, Args...>;

	template< class Default, template<class...> class Op, class... Args >
	using
		detected_or_t = typename detected_or<Default, Op, Args...>::type;


	// `is_detected_exact`

	template< class Expected, template<class...> class Op, class... Args >
	using
		is_detected_exact = std::is_same< Expected, detected_t<Op, Args...> >;

	/*
	template< class Expected, template<class...> class Op, class... Args >
	constexpr bool
		is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;
	*/


	// `is_detected_convertable`

	template< class To, template<class...> class Op, class... Args >
	using
		is_detected_convertable = std::is_convertible< detected_t<Op, Args...>, To >;

	/*
	template< class To, template<class...> class Op, class... Args >
	constexpr bool
	is_detected_convertable_v = is_detected_convertable<To, Op, Args...>::value;
	*/
}

/******************************************************************************
** Template Aserrt Helpers
******************************************************************************/

// See http://stackoverflow.com/a/13366183

template <typename Assertion>
struct _T_assertValue
{
	static_assert(Assertion::value, "Assertion failed <see below for more information>");
	static bool const value = Assertion::value;
};

template <typename BadType>
struct _T_assertBad : std::false_type
{
	static_assert(std::is_same<BadType, _t::nonesuch>::value, "Assertion failed bad type <see below for type>");
};


/******************************************************************************
** Offset of replacement
******************************************************************************/

// See: https://gist.github.com/graphitemaster/494f21190bb2c63c5516

template <typename T1, typename T2>
struct offset_of_impl {
	static T2 object;
	static constexpr size_t offset(T1 T2::*member) {
		return size_t(&(offset_of_impl<T1, T2>::object.*member)) -
			size_t(&offset_of_impl<T1, T2>::object);
	}
};

template <typename T1, typename T2>
T2 offset_of_impl<T1, T2>::object;

template <typename T1, typename T2>
inline constexpr size_t offset_of(T1 T2::*member) {
	return offset_of_impl<T1, T2>::offset(member);
}

/******************************************************************************
** bind extension
******************************************************************************/

namespace stdext
{
    namespace pl = std::placeholders;
    
	template<typename TThis, typename TRet, typename TClass>
	inline std::function<TRet()> 
		bindmem(TThis _this, TRet(TClass::*f)())
	{
		return std::bind(f, static_cast<TClass*>(_this));
	}

	template<typename TThis, typename TRet, typename TClass
		, typename TParam1>
	inline std::function<TRet(TParam1)> 
		bindmem(TThis _this, TRet(TClass::*f)(TParam1))
	{
		return std::bind(f, static_cast<TClass*>(_this), pl::_1);
	}

	template<typename TThis, typename TRet, typename TClass
		, typename TParam1, typename TParam2>
	inline std::function<TRet(TParam1, TParam2)> 
		bindmem(TThis _this, TRet(TClass::*f)(TParam1, TParam2))
	{
        return std::bind(f, static_cast<TClass*>(_this), pl::_1, pl::_2);
	}

	template<typename TThis, typename TRet, typename TClass
		, typename TParam1, typename TParam2, typename TParam3>
	inline std::function<TRet(TParam1, TParam2, TParam3)>
		bindmem(TThis _this, TRet(TClass::*f)(TParam1, TParam2, TParam3))
	{
        return std::bind(f, static_cast<TClass*>(_this), pl::_1, pl::_2, pl::_3);
	}

	template<typename TThis, typename TRet, typename TClass
		, typename TParam1, typename TParam2, typename TParam3, typename TParam4>
	inline std::function<TRet(TParam1, TParam2, TParam3, TParam4)> 
		bindmem(TThis _this, TRet(TClass::*f)(TParam1, TParam2, TParam3, TParam4))
	{
		return std::bind(f, static_cast<TClass*>(_this), pl::_1, pl::_2, pl::_3, pl::_4);
	}
}

/******************************************************************************
** nonassign
******************************************************************************/

namespace stdext
{
	class nonassign {
	public:
		nonassign(nonassign const&) = delete;
		nonassign& operator=(nonassign const&) = delete;
		nonassign() {}
	};
}

/******************************************************************************
** template helpers
******************************************************************************/

namespace stdext
{
	template<typename TLeft, typename TRight>
	struct typechange {
		typedef TRight type;
	};

	// is the type complete (does it exist)
	// * https://stackoverflow.com/a/44229779
	// * https://stackoverflow.com/a/37193089

	template <class T, class = void>
	struct is_complete : std::false_type
	{};

	template <class T>
	struct is_complete< T, decltype(void(sizeof(T))) > : std::true_type
	{};

	// as reference based off of remove_pointer
	template< class T > struct as_reference { typedef T& type; };
	template< class T > struct as_reference<T&> { typedef T& type; };
	template< class T > struct as_reference<T const&> { typedef T const& type; };
	template< class T > struct as_reference<T*> { typedef T& type; };
	template< class T > struct as_reference<T const*> { typedef T const& type; };
	template< class T > struct as_reference<T volatile*> { typedef T& type; };
	template< class T > struct as_reference<T const volatile*> { typedef T const& type; };
}
