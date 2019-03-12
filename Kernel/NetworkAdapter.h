#pragma once

#include <AK/ByteBuffer.h>
#include <AK/SinglyLinkedList.h>
#include <AK/Types.h>
#include <Kernel/MACAddress.h>
#include <Kernel/IPv4.h>
#include <Kernel/ARP.h>
#include <Kernel/ICMP.h>

class NetworkAdapter {
public:
    static NetworkAdapter* from_ipv4_address(const IPv4Address&);
    virtual ~NetworkAdapter();

    virtual const char* class_name() const = 0;
    MACAddress mac_address() { return m_mac_address; }
    IPv4Address ipv4_address() const { return m_ipv4_address; }

    void set_ipv4_address(const IPv4Address&);

    void send(const MACAddress&, const ARPPacket&);
    void send_ipv4(const MACAddress&, const IPv4Address&, IPv4Protocol, ByteBuffer&& payload);

    ByteBuffer dequeue_packet();

protected:
    NetworkAdapter();
    void set_mac_address(const MACAddress& mac_address) { m_mac_address = mac_address; }
    virtual void send_raw(const byte*, int) = 0;
    void did_receive(const byte*, int);

private:
    MACAddress m_mac_address;
    IPv4Address m_ipv4_address;
    SinglyLinkedList<ByteBuffer> m_packet_queue;
};
