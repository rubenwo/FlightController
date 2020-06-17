#pragma once

#include <array>
#include <functional>

template <typename State, typename Symbol, size_t NUM_STATE, size_t NUM_SYMBOLS>
class FSM
{
public:
    struct Transition
    {
        std::function<void()> handler;
        State target;
    };

    template <typename Func>
    void addTransition(Symbol symbol, State from, State target, Func handler)
    {
        Transition transition{handler, target};
        this->stt_[from][symbol] = transition;
    }

    void raise(Symbol input)
    {
        Transition &transition = this->stt_[this->currentState_][input];

        transition.handler();
        this->currentState_ = transition.target;
    }

private:
    std::array<std::array<Transition, NUM_SYMBOLS>, NUM_STATE> stt_;
    State currentState_;
};