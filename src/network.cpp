#include "network.h"
#include <iostream>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")

NetworkManager::NetworkManager()
    : m_udpSocket(INVALID_SOCKET), m_isInitialized(false) {
    ZeroMemory(&m_broadcastAddr, sizeof(m_broadcastAddr));
}

NetworkManager::~NetworkManager() {
    if (m_udpSocket != INVALID_SOCKET) {
        closesocket(m_udpSocket);
    }
}

bool NetworkManager::Init() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return false;
    }

    m_udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_udpSocket == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }

    BOOL broadcast = TRUE;
    if (setsockopt(m_udpSocket, SOL_SOCKET, SO_BROADCAST, (char*)&broadcast, sizeof(broadcast)) == SOCKET_ERROR) {
        closesocket(m_udpSocket);
        WSACleanup();
        return false;
    }

    m_broadcastAddr.sin_family = AF_INET;
    m_broadcastAddr.sin_port = htons(7000);
    m_broadcastAddr.sin_addr.s_addr = INADDR_BROADCAST;

    m_isInitialized = true;
    return true;
}

bool NetworkManager::SendCancelBlackScreen() {
    if (!m_isInitialized) return false;

    // 模拟指令包（需逆向实际协议）
    unsigned char packet[] = {
        0xAA, 0xAA,
        0x01,
        0x1A,
        0x00, 0x00, 0x00, 0x00
    };
    int packetSize = sizeof(packet);

    int result = sendto(m_udpSocket, (char*)packet, packetSize, 0,
                        (sockaddr*)&m_broadcastAddr, sizeof(m_broadcastAddr));

    if (result == SOCKET_ERROR) {
        std::cerr << "[!] sendto 失败，错误码: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}