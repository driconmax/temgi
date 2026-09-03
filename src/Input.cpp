#include "temgi/Input.h"
#include "temgi/ControllerBackend.h"

namespace temgi
{
    Input::Input()
    {
        for (int i = 0; i < MAX_CONTROLLERS; ++i)
        {
            controllers_[static_cast<std::size_t>(i)].bindBackend(nullptr, i);
        }
    }

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

    Controller& Input::controller(int index)
    {
        if (index < 0) index = 0;
        if (index >= MAX_CONTROLLERS) index = MAX_CONTROLLERS - 1;
        return controllers_[static_cast<std::size_t>(index)];
    }

    const Controller& Input::controller(int index) const
    {
        if (index < 0) index = 0;
        if (index >= MAX_CONTROLLERS) index = MAX_CONTROLLERS - 1;
        return controllers_[static_cast<std::size_t>(index)];
    }

    void Input::nextFrame()
    {
        previous_ = current_;

        for (Controller& controller : controllers_)
        {
            controller.nextFrame();
        }
    }

    void Input::setControllerBackend(ControllerBackend* backend)
    {
        for (int i = 0; i < MAX_CONTROLLERS; ++i)
        {
            controllers_[static_cast<std::size_t>(i)].bindBackend(backend, i);
        }
    }

} // namespace temgi

