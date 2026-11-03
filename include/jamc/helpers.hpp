#ifndef JAMC_HELPERS_HPP
#define JAMC_HELPERS_HPP

#include <concepts>
#include <expected>
#include <filesystem>
#include <format>
#include <meta>
#include <print>
#include <string>
#include <string_view>
#include <vector>

namespace jamc {
// errors.hpp includes this file
// so instead of including errors.hpp
// forward declare the structs
// (and prevent cyclic inclusion)
struct Error;
struct FileReadError;

template <typename E>
concept FormatableError =
    std::derived_from<E, Error> && std::formattable<E, char>;

// NOTE - delete ifdefs when clang implements reflection
#ifdef __cpp_impl_reflection
template <typename E>
// This needs to be an expression (enclosure in parentheses makes it one)
    requires(std::meta::is_enum_type(^^E))
constexpr auto ToString(E e) -> std::string_view {
    namespace meta = std::meta;
    namespace rg = std::ranges;
    namespace rv = rg::views;

    // Have to wrap enumerators_of in define_static_array because enumerators_of
    // returns a vector and this has to be deallocated by the end of the
    // evaluation of this expression.
    static auto constexpr enum_vals_info{
        std::define_static_array(meta::enumerators_of(^^E))};

    static auto constexpr lookup_table = []() consteval {
        std::array<std::pair<E, std::string_view>, enum_vals_info.size()>
            table{};
        auto i{0UZ};
        template for (auto constexpr enum_val_info : enum_vals_info) {
            table[i].first = [:enum_val_info:];
            table[i].second = meta::identifier_of(enum_val_info);
            ++i;
        }
        return table;
    }();

    template for (auto constexpr enum_pair : lookup_table) {
        if (enum_pair.first == e) {
            return enum_pair.second;
        }
    }

    return [] consteval {
        std::string unknown_enum = "Unknown";
        unknown_enum += std::string{meta::identifier_of(^^E)};
        return std::define_static_string(unknown_enum);
    }();
}
#else
// Just a stub so that clang shuts up (it's compiled with gcc anyway)
template <typename E>
    requires std::is_enum_v<E>
constexpr auto ToString(E /*unused*/) -> std::string_view {
    return "ReflectionUnavailable";
}
#endif

template <FormatableError E>
void ReportErrors(std::vector<E> errs) {
    for (auto i{0UZ}; i < errs.size(); ++i) {
        std::print(stderr, "{}", errs[i]);
        if (i != errs.size() - 1) {
            std::print(stderr, ", ");
        } else {
            std::println(stderr);
        }
    }
}

template <FormatableError E>
void ReportError(E const &err) {
    std::println(stderr, "{}", err);
}

/**
 * @note - We need a string (not string_view) probably
 * because the OS requires null terminated filename
 * (string_view doesn't need to be null terminated).
 */
auto ReadFile(std::filesystem::path const &filename)
    -> std::expected<std::string, FileReadError>;
}  // namespace jamc

#endif
