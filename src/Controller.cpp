#include "temgi/Controller.h"
#include "temgi/ControllerBackend.h"

namespace temgi
{
    TriggerHandle::TriggerHandle(ControllerBackend* backend, int controllerIndex, Trigger trigger)
        : backend_(backend), controllerIndex_(controllerIndex), trigger_(trigger)
    {
    }

    void TriggerHandle::setResistance(float startPosition, float strength)
    {
        apply(TriggerEffect{TriggerEffectType::Resistance, startPosition, strength, 0.0f});
    }

    void TriggerHandle::setVibration(float strength, float frequency)
    {
        apply(TriggerEffect{TriggerEffectType::Vibration, 0.0f, strength, frequency});
    }

    void TriggerHandle::clear()
    {
        apply(TriggerEffect{});
    }

    void TriggerHandle::apply(const TriggerEffect& effect)
    {
        if (backend_ == nullptr) return;
        backend_->setTriggerEffect(controllerIndex_, trigger_, effect);
    }

    bool Controller::isConnected() const
    {
        return connected_;
    }

    bool Controller::isPressed(ControllerButton button) const
    {
        const auto index = static_cast<std::size_t>(button);
        return current_[index] && !previous_[index];
    }

    bool Controller::isHeld(ControllerButton button) const
    {
        const auto index = static_cast<std::size_t>(button);
        return current_[index] && previous_[index];
    }

    bool Controller::isReleased(ControllerButton button) const
    {
        const auto index = static_cast<std::size_t>(button);
        return !current_[index] && previous_[index];
    }

    StickState Controller::leftStick() const
    {
        return StickState{leftX_, leftY_};
    }

    StickState Controller::rightStick() const
    {
        return StickState{rightX_, rightY_};
    }

    float Controller::leftTrigger() const
    {
        return leftTrigger_;
    }

    float Controller::rightTrigger() const
    {
        return rightTrigger_;
    }

    void Controller::rumble(float lowFrequency, float highFrequency)
    {
        if (backend_ == nullptr) return;
        backend_->rumble(index_, lowFrequency, highFrequency);
    }

    bool Controller::supportsAdaptiveTriggers() const
    {
        return backend_ != nullptr && backend_->supportsAdaptiveTriggers(index_);
    }

    TriggerHandle Controller::trigger(Trigger which)
    {
        return TriggerHandle(backend_, index_, which);
    }

    void Controller::setConnected(bool connected)
    {
        connected_ = connected;

        if (!connected)
        {
            current_.fill(false);
            previous_.fill(false);
            leftX_ = leftY_ = rightX_ = rightY_ = 0.0f;
            leftTrigger_ = rightTrigger_ = 0.0f;
        }
    }

    void Controller::setButton(ControllerButton button, bool pressed)
    {
        const auto index = static_cast<std::size_t>(button);
        current_[index] = pressed;
    }

    void Controller::setAxis(ControllerAxis axis, float value)
    {
        switch (axis)
        {
            case ControllerAxis::LeftX: leftX_ = value; break;
            case ControllerAxis::LeftY: leftY_ = value; break;
            case ControllerAxis::RightX: rightX_ = value; break;
            case ControllerAxis::RightY: rightY_ = value; break;
            case ControllerAxis::LeftTrigger: leftTrigger_ = value; break;
            case ControllerAxis::RightTrigger: rightTrigger_ = value; break;
            default: break;
        }
    }

    void Controller::nextFrame()
    {
        previous_ = current_;
    }

    void Controller::bindBackend(ControllerBackend* backend, int index)
    {
        backend_ = backend;
        index_ = index;
    }
} // namespace temgi
