namespace temgi
{
    class ConsoleEventSubscriber {
        public:
            virtual ~ConsoleEventSubscriber() = default;

            virtual void onConsoleStart() {}
            virtual void onConsoleStop() {}

            virtual void onCartridgeLoad() {}
            virtual void onCartridgeUnload() {}

            virtual void onFrameStart() {}
            virtual void onFrameEnd() {}

            // Platform-level window request; the console itself has no concept
            // of a window, this just fans the request out to subscribers (e.g.
            // the SDL platform layer) that do.
            virtual void onToggleFullscreenRequested() {}

    };
} // namespace temgi
