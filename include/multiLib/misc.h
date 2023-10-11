#pragma once

namespace multiLib
{
    enum class initSystems : int { SDL = 0, Image, TTF, Mixer};

    class init
    {
        public:
        static init& getInit();

        init& initSystem(initSystems system);
        inline bool calledInit(initSystems system);

        private:
        init() = default;

        init(const init& copyFrom) = delete;
        init& operator=(const init& From) = delete;

        init(init&& moveFrom) = delete;
        init& operator=(init&& moveFrom) = delete;

        ~init();

        bool inits[4]{false};
    };
} // multiLib
