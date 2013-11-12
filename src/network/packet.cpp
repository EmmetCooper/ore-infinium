/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#include "packet.h"

#include "src/packet.pb.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "src/debug.h"

#include <sstream>

std::string Packet::serialize(google::protobuf::Message* message, uint32_t packetType)
{

    std::string stringPacketContents;
    google::protobuf::io::StringOutputStream stringStreamPacketContents(&stringPacketContents);

    serializeStreamContents(&stringStreamPacketContents, message, packetType);

    std::string stringPacketHeader;
    google::protobuf::io::StringOutputStream stringStreamPacketHeader(&stringPacketHeader);

    serializeStreamHeader(&stringStreamPacketHeader, packetType);

    assert(stringPacketContents.size() > 0);

    std::stringstream ss(std::stringstream::out | std::stringstream::binary);
    ss << stringPacketHeader;
    ss << stringPacketContents;

    return ss.str();
}

void Packet::serializeStreamHeader(google::protobuf::io::StringOutputStream* stringOut, uint32_t packetType)
{
    google::protobuf::io::CodedOutputStream coded_out(stringOut);

    std::string headerString;

    // write packet header, containing type of message we're sending
    PacketBuf::Packet p;
    p.set_type(packetType);
    p.SerializeToString(&headerString);

    // write the size of the serialized packet header and the contents itself
    coded_out.WriteVarint32(headerString.size());
    coded_out.WriteRaw(headerString.data(), headerString.size());
}

void Packet::serializeStreamContents(google::protobuf::io::StringOutputStream* stringOut, google::protobuf::Message* message, uint32_t packetType)
{
    google::protobuf::io::CodedOutputStream coded_out(stringOut);

    std::string contentsString;
    // write actual contents
    message->SerializeToString(&contentsString);

    coded_out.WriteVarint32(contentsString.size());
    coded_out.WriteString(contentsString);
}

uint32_t Packet::deserializePacketType(const std::string& packet)
{
    std::stringstream ss(packet);

    google::protobuf::io::IstreamInputStream raw_in(&ss);
    google::protobuf::io::CodedInputStream coded_in(&raw_in);

    std::string s;

    assert(ss.str().size() > 0);

    //packet header
    uint32_t msgSize;

    bool result = coded_in.ReadVarint32(&msgSize);
    assert(result);

    assert(msgSize > 0);

    if (coded_in.ReadString(&s, msgSize)) {
        PacketBuf::Packet p;
        p.ParseFromString(s);

        return p.type();
    } else {
        assert(0);
    }
}

void Packet::deserialize(const std::string& packetToDeserialize, google::protobuf::Message* message)
{
    std::stringstream ss(packetToDeserialize);

    google::protobuf::io::IstreamInputStream raw_in(&ss);
    google::protobuf::io::CodedInputStream coded_in(&raw_in);

    std::string s;

    assert(ss.str().size() > 0);

    //packet header
    uint32_t msgSize;
    coded_in.ReadVarint32(&msgSize);
    assert(msgSize > 0);

    if (coded_in.ReadString(&s, msgSize)) {
        PacketBuf::Packet p;
        p.ParseFromString(s);
    } else {
        assert(0);
    }

    // retrieve the size of the uncompressed message..this size is part of the uncompressed data
    coded_in.ReadVarint32(&msgSize);

    //packet contents
    if (coded_in.ReadString(&s, msgSize)) {
        message->ParseFromString(s);
    } else {
        assert(0);
    }
}

void Packet::sendPacket(ENetPeer* peer, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(peer && message);

    std::string packetContents = Packet::serialize(message, packetType);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_peer_send(peer, 0, packet);
}

void Packet::sendPacketBroadcast(ENetHost* host, google::protobuf::Message* message, uint32_t packetType, uint32_t enetPacketType)
{
    assert(host && message);

    std::string packetContents = Packet::serialize(message, packetType);

    ENetPacket *packet = enet_packet_create(packetContents.data(), packetContents.size(), enetPacketType);
    assert(packet);

    enet_host_broadcast(host, 0, packet);
}
