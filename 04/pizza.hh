#ifndef PIZZA_H
#define PIZZA_H

#include <array>
#include <type_traits>
#include <cstddef>
#include <utility>
#include <functional>
#include <typeinfo>

namespace {
/* argmax type used when finding best mix of two Pizzas */
using argmax_t = size_t;

/*-- BEGIN Check if type has a static yumminess() method --*/
template <typename... Other>
struct can_be_mixed; 

template <typename U, typename... Other>
struct can_be_mixed<U, Other...> {
    static constexpr bool value =
        can_be_mixed<U>::value && can_be_mixed<Other...>::value;
};

template <typename U>
struct can_be_mixed<U> {
    static constexpr bool value = std::is_arithmetic<decltype(U::yumminess(
                                      std::declval<std::size_t>()))>() &&
                                  U::yumminess(0) == 0;
};
/*-- END Check if type has a static yumminess() method --*/

/*-- BEGIN Check parameters uniqness --*/

template <typename F, typename... T>
constexpr bool is_one_of =
    (std::is_same<F, T>{} || ...);

template <typename...>
constexpr bool is_unique = true;

template <typename F, typename... T>
constexpr bool is_unique<F, T...> = is_unique<T...> && !is_one_of<F, T...>;
/*-- END Check parameters uniqness --*/

/* Find index of a type on a list of types, e.g.:
* get_type_index<napoli, (supreme, napoli)>() = 1 */
template <typename T, typename U = void, typename... Types>
constexpr size_t get_type_index() {
    return std::is_same<T, U>::value ? 0 : 1 + get_type_index<T, Types...>();
}

/* Finds argmax using a constexpr PizzaKind:yumminess() method,
 * iterating over <0,SIZE> range */
template <typename PizzaKind>
constexpr argmax_t find_arg_max(size_t SIZE) {
    using return_type = decltype(PizzaKind::yumminess(0));
    argmax_t argmax = 0;
    return_type max_value = PizzaKind::yumminess(0);
    for (size_t i = 1; i <= SIZE; ++i) {
        return_type current = PizzaKind::yumminess(i);
        if (current > max_value) {
            max_value = current;
            argmax = i;
        }
    }
    return argmax;
}
} /* anonymous namespace */

/* Type representing Pizzeria offering kinds of pizza provided in Kinds variadic
 * template arguments */
template <typename... Kinds>
struct Pizzeria {
    /* Check if we are creating Pizzeria using distinct Kinds of pizza */
    static_assert(
        is_unique<Kinds...>,
        "You cannot create Pizzeria with more than 1 pizza of one kind");

    /* Type representing Pizza served by our Pizzeria;
     * PizzaKinds is a list with counts of slices */
    template <size_t... PizzaKinds>
    struct Pizza {
        /* Type used to check whether two Pizzas come from the same Pizzeria
         * (and can be mixed) */
        using pizzeria_type = Pizzeria<Kinds...>;

        /* Type used to check whether both Pizzas have yumminess() method
         * defined in a correct format */
        using can_be_mixed_type = can_be_mixed<Kinds...>;

        static const size_t PIZZA_DIVIDER = 2;
        /* Type used to divide all slices of a given pizza into two */
        using sliced_type = Pizza<(PizzaKinds * PIZZA_DIVIDER)...>;

        /* Returns an array with number of pizza slices for a given kind in our
         * pizza; the order of kinds is the same as order of Kinds... in the
         * Pizzeria */
        static constexpr std::array<size_t, sizeof...(Kinds)> as_array() {
            return {{PizzaKinds...}};
        }

        /* Returns count of slices of a given Kind for our Pizza */
        template <typename Kind>
        static constexpr size_t count() {
            return ((std::is_same<Kind, Kinds>::value ? PizzaKinds : 0) + ...);
        }

        /* Type representing a Pizza with the best possible mix of pizzas Pizza1
         * and Pizza2 */
        template <typename Pizza1, typename Pizza2>
        struct MixedPizza {
            using type = Pizza<(
                find_arg_max<Kinds>(Pizza1::template count<Kinds>() +
                                    Pizza2::template count<Kinds>()))...>;
        };
    };

    template <typename Kind>
    struct make_pizza {
        /* Check if we are trying to create Pizza of a kind available in our
         * Pizzeria */
        static_assert(
            is_one_of<Kind, Kinds...>,
            "Our pizzeria does not serve the kind of pizza requested");

        static const size_t DEFAULT_SLICE_COUNT = 8;
        using type = Pizza<(
            std::is_same<Kind, Kinds>::value ? DEFAULT_SLICE_COUNT : 0)...>;
    };
};

/* Type representing a Pizza with the best possible mix of pizzas Pizza1
 * and Pizza2 (available from outside the Pizzeria's scope, whereas
 * MixedPizza is available only from Pizzeria's/Pizza's scope */
template <typename Pizza1, typename Pizza2>
struct best_mix {
    /* Check if it is possible to create the mix */
    static_assert(Pizza1::can_be_mixed_type::value &&
                      Pizza2::can_be_mixed_type::value,
                  "Some of the kinds of Pizza in the mixed pizzas don't have "
                  "yumminess() method defined in a correct format");

    /* Check if both pizzas come from the same pizzeria */
    static_assert(std::is_same<typename Pizza1::pizzeria_type,
                               typename Pizza2::pizzeria_type>(),
                  "You cannot create a mix out of Pizzas from two "
                  "different Pizzerias");

    using type = typename Pizza1::template MixedPizza<Pizza1, Pizza2>::type;
};

#endif /* PIZZA_H */
