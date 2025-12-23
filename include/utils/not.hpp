#pragma once
template < class Goal, class Test >
struct NotTemplate
{
	using type = void;
	constexpr static bool value = true;
};
template < class Goal >
struct NotTemplate< Goal, Goal >
{
	constexpr static bool value = false;
};
template < class Goal, class Test >
struct NotArrayTemplate
{
	using type = void;
};
template < class Goal, class Test >
struct NotArrayTemplate< Goal, Test[] >
{
	using type = typename NotArrayTemplate< Goal, Test >::type;
};
template < class Goal, class Test, unsigned long long int N >
struct NotArrayTemplate< Goal, Test[N] >
{
	using type = typename NotArrayTemplate< Goal, Test >::type;
};
template < class Goal >
struct NotArrayTemplate< Goal, Goal[] >
{
};
template < class Goal, unsigned long long int N >
struct NotArrayTemplate< Goal, Goal[N] >
{
};
template < class G, class T >
using Not = typename NotTemplate< G, T >::type;


template < class G, class T >
inline constexpr bool NotV = NotTemplate< G, T >::value;

template < class G, class T >
using NotArray = typename NotArrayTemplate< G, T >::type;
