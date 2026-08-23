#include "temgi/Input.h"

namespace temgi
{
    
    bool Input::pressed(Button button) const
    {
        const auto index = static_cast<std::size_t>(button);
        return current_[index] && !previous_[index];
    }
    
    bool Input::held(Button button) const
    {
        const auto index = static_cast<std::size_t>(button);
        return current_[index] && previous_[index];
    }
    
    bool Input::released(Button button) const
    {
        const auto index = static_cast<std::size_t>(button);
        return !current_[index] && previous_[index];
    }
    
    void Input::setButton(Button button, bool state)
    {
        const auto index = static_cast<std::size_t>(button);
        current_[index] = state;
    }
    
    void Input::nextFrame()
    {
        previous_ = current_;
    }
    
} // namespace temgi

