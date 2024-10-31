#pragma once
#ifndef __JW_NETWORK_H__
#define __JW_NETWORK_H__
#include "Singleton.hpp"
#include <memory>

namespace jw
{
    class Network : public Singleton<Network>
    {
    public:
        bool Initialize();

    protected:
        Network();
        ~Network();
        Network(const Network&) = delete;
        Network& operator=(const Network&) = delete;
    private:
        bool initializeWSASocketFunc();

        struct Impl;
        std::unique_ptr<Impl> _pImpl;
        friend class Singleton<Network>;
    };
}

#define NETWORK jw::Network::GetInstance

#endif // __JW_NETWORK_H__

