
#pragma once

#include <winsock2.h>
#include <windows.h>
#include <string>

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    bool Init();
    bool SendCancelBlackScreen();

private:
    SOCKET m_udpSocket;
    sockaddr_in m_broadcastAddr;
    bool m_isInitialized;
};