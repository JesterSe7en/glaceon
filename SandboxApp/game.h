#pragma once

#include "Glaceon.h"

class SandBoxApplication : public Glaceon::Application
{
    public:
        virtual void onStart() override;
        virtual void onUpdate() override;
        virtual void onShutdown() override;
};