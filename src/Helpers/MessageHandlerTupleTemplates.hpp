#pragma once

#include <array>
#include <tuple>
#include <cstdint>
#include "CarState.hpp"
#include "../Protocol/BusMessage.hpp"
#include "../Protocol/MessageScheduler.hpp"

// For Parse method processing

template <typename HandlerT>
inline void processHandler(HandlerT &h, CarState *carState, const BusMessage &msg)
{
    if constexpr (std::is_integral_v<decltype(HandlerT::MessageId)>)
    {
        if (HandlerT::MessageId == msg.id)
        {
            h.Parse(carState, msg);
        }
    }
    else
    {
        // fallback if MessageId isn't integral (adjust if needed)
        if (std::remove_reference_t<decltype(h)>::MessageId == msg.id)
        {
            h.Parse(carState, msg);
        }
    }
}

// expand tuple without capturing lambdas — iterative via index_sequence
template <typename Tuple, size_t... I>
inline void processHandlersTupleImpl(Tuple &t, CarState *carState, const BusMessage &msg, std::index_sequence<I...>)
{
    // The comma fold calls processHandler for each index. No captures.
    (void)std::initializer_list<int>
    {
        (processHandler(std::get<I>(t), carState, msg), 0)...
    };
}

template <typename Tuple>
inline void processHandlersTuple(Tuple &t, CarState *carState, const BusMessage &msg)
{
    processHandlersTupleImpl(t, carState, msg, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}

// For Generate method processing

template <typename HandlerT>
inline void processGenerator(HandlerT &h, CarState *carState, MessageScheduler *_scheduler)
{
    BusMessage msg = h.Generate(carState);
    _scheduler->AddOrUpdateMessage(msg, carState->CurrenTime);
}

template <typename Tuple, size_t... I>
inline void processGeneratorsTupleImpl(Tuple &t, CarState *carState, MessageScheduler *_scheduler, std::index_sequence<I...>)
{
    (void)std::initializer_list<int>{
        (processGenerator(std::get<I>(t), carState, _scheduler), 0)...
    };
}

template <typename Tuple>
inline void processGeneratorsTuple(Tuple &t, CarState *carState, MessageScheduler *_scheduler)
{
    processGeneratorsTupleImpl(t, carState, _scheduler, std::make_index_sequence<std::tuple_size_v<Tuple>>{});
}