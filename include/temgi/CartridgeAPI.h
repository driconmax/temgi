#pragma once

namespace temgi
{
    class Console;

    class CartridgeAPI {
        public:
            virtual ~CartridgeAPI() = default;

            virtual void start(Console& console) = 0;
            virtual void update(Console& console) = 0;
    };
} // namespace temgi
