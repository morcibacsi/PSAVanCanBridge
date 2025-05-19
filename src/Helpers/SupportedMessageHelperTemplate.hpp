#pragma once

#include <array>
#include <tuple>
#include <cstdint>

template<typename Tuple, std::size_t... I>
constexpr std::array<uint32_t, sizeof...(I)> ExtractMessageIdsImpl(std::index_sequence<I...>) {
    return { std::tuple_element_t<I, Tuple>::MessageId... };
}

template<typename Tuple>
constexpr auto ExtractMessageIds() {
    return ExtractMessageIdsImpl<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}
