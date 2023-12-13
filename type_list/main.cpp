#include <type_traits>

/******************************************************************************/
/*                                 type list                                  */
/******************************************************************************/

template<typename ...TYPES>
struct list_types {};

struct none_type {};

/******************************************************************************/
/*                                  has type                                  */
/******************************************************************************/

template<typename T>
struct has_type {
    using type = T;
};

/******************************************************************************/
/*                                     if                                     */
/******************************************************************************/

template<bool COND, typename THEN, typename ELSE>
struct if_cond;

template<typename THEN, typename ELSE>
struct if_cond<false, THEN, ELSE> : has_type<ELSE> {};

template<typename THEN, typename ELSE>
struct if_cond<true, THEN, ELSE> : has_type<THEN> {};

template<bool COND, typename THEN, typename ELSE>
using if_cond_t = typename if_cond<COND, THEN, ELSE>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<int, if_cond_t<(1 < 10), int, void>>);
static_assert(std::is_same_v<void, if_cond_t<(10 < 1), int, void>>);

/******************************************************************************/
/*                                    size                                    */
/******************************************************************************/

/* var_size *******************************************************************/
template<typename ...TYPES>
struct var_size;

template<typename H, typename ...TYPES>
struct var_size<H, TYPES...> {
    static constexpr std::size_t value = 1 + var_size<TYPES...>::value;
};

template<>
struct var_size<> {
    static constexpr std::size_t value = 0;
};

template<typename ...TYPES>
constexpr std::size_t var_size_v = var_size<TYPES...>::value;

/* asserts ********************************************************************/
static_assert(var_size_v<> == 0);
static_assert(var_size_v<int> == 1);
static_assert(var_size_v<int, double> == 2);

/* size ***********************************************************************/
template<typename LIST>
struct size;

template<template<typename ...> class LIST, typename ...TYPES>
struct size<LIST<TYPES...>> : var_size<TYPES...> {};

template<typename LIST>
constexpr std::size_t size_v = size<LIST>::value;

/* asserts ********************************************************************/
static_assert(size_v<list_types<>> == 0);
static_assert(size_v<list_types<int, int>> == 2);

/******************************************************************************/
/*                                  is empty                                  */
/******************************************************************************/

template<typename LIST>
struct is_empty : std::false_type {};

template<template<typename ...> class LIST>
struct is_empty<LIST<>> : std::true_type {};

template<typename LIST>
using is_empty_t = typename is_empty<LIST>::type;

template<typename LIST>
struct is_empty_predicate {
    static constexpr bool value = std::is_same_v<std::true_type, is_empty_t<LIST>>;
};

/* asserts ********************************************************************/
static_assert(std::is_same_v<std::true_type, is_empty_t<list_types<>>>);
static_assert(std::is_same_v<std::false_type, is_empty_t<list_types<int>>>);

/******************************************************************************/
/*                                   front                                    */
/******************************************************************************/

template<typename LIST>
struct front;

template<template<typename ...> class LIST, typename H, typename ...TYPES>
struct front<LIST<H, TYPES...>> {
    using type = H;
};

template<template<typename ...> class LIST>
struct front<LIST<>> {
    using type = none_type;
};

template<typename LIST>
using front_t = typename front<LIST>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<none_type, front_t<list_types<>>>);
static_assert(std::is_same_v<int, front_t<list_types<int>>>);

/******************************************************************************/
/*                                 push front                                 */
/******************************************************************************/

template<typename LIST, typename T>
struct push_front;

template<template<typename ...> class LIST, typename ...TYPES, typename T>
struct push_front<LIST<TYPES...>, T> : has_type<LIST<T, TYPES...>> {};

template<typename LIST, typename T>
using push_front_t = typename push_front<LIST, T>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<list_types<int>, push_front_t<list_types<>, int>>);
static_assert(std::is_same_v<list_types<double, int>, push_front_t<list_types<int>, double>>);

/******************************************************************************/
/*                                 pop front                                  */
/******************************************************************************/

template<typename LIST>
struct pop_front;

template<template<typename ...> class LIST, typename H, typename ...TYPES>
struct pop_front<LIST<H, TYPES...>> {
    using type = LIST<TYPES...>;
};

template<template<typename ...> class LIST>
struct pop_front<LIST<>> {
    using type = LIST<>;
};

template<typename LIST>
using pop_front_t = typename pop_front<LIST>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<list_types<>, pop_front_t<list_types<>>>);
static_assert(std::is_same_v<list_types<>, pop_front_t<list_types<int>>>);
static_assert(std::is_same_v<list_types<double>, pop_front_t<list_types<int, double>>>);

/******************************************************************************/
/*                                    back                                    */
/******************************************************************************/

template<typename LIST>
struct back;

template<template<typename ...> class LIST>
struct back<LIST<>> : has_type<none_type> {};

template<template<typename ...> class LIST, typename H, typename ...TYPES>
struct back<LIST<H, TYPES...>> : if_cond<
                                    // COND
                                    (var_size_v<TYPES...> == 0),
                                    // THEN
                                    H,
                                    // ELSE
                                    typename back<LIST<TYPES...>>::type
                                > {};

template<typename LIST>
using back_t = typename back<LIST>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<none_type, back_t<list_types<>>>);
static_assert(std::is_same_v<int, back_t<list_types<int>>>);
static_assert(std::is_same_v<double, back_t<list_types<bool, double>>>);

/******************************************************************************/
/*                                 push back                                  */
/******************************************************************************/

template<typename LIST, typename T>
struct push_back;

template<template<typename ...> class LIST, typename T, typename ...TYPES>
struct push_back<LIST<TYPES...>, T> : has_type<LIST<TYPES..., T>> {};

template<typename LIST, typename T>
using push_back_t = typename push_back<LIST, T>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<list_types<int>, push_back_t<list_types<>, int>>);
static_assert(std::is_same_v<list_types<int, double>, push_back_t<list_types<int>, double>>);
static_assert(std::is_same_v<list_types<int, double, bool>, push_back_t<list_types<int, double>, bool>>);

/******************************************************************************/
/*                                  pop back                                  */
/******************************************************************************/

template<typename LIST>
struct pop_back : has_type<LIST> {};

template<template<typename ...> class LIST, typename H, typename ...TYPES>
struct pop_back<LIST<H, TYPES...>> :
    if_cond<
        // COND
        (var_size_v<TYPES...> == 0),
        // THEN
        LIST<>,
        // ELSE
        typename push_front<typename pop_back<LIST<TYPES...>>::type, H>::type
    > {};

template<typename LIST>
using pop_back_t = typename pop_back<LIST>::type;

/* asserts ********************************************************************/
static_assert(std::is_same_v<list_types<>, pop_back_t<list_types<>>>);
static_assert(std::is_same_v<list_types<>, pop_back_t<list_types<int>>>);
static_assert(std::is_same_v<list_types<int>, pop_back_t<list_types<int, double>>>);
static_assert(std::is_same_v<list_types<int, double>, pop_back_t<list_types<int, double, bool>>>);

/******************************************************************************/
/*                                    main                                    */
/******************************************************************************/

int main(int, char**) {
    return 0;
}
