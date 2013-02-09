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

#include "src/network/protobuf-compiled/packet.pb.h"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include "src/debug.h"

#include <iostream>
#include <fstream>

Packet::Packet()
{

}

Packet::~Packet()
{

}

void Packet::serialize(std::stringstream& out)
{
    google::protobuf::io::ZeroCopyOutputStream *raw_out = new ::google::protobuf::io::OstreamOutputStream(&out);
    google::protobuf::io::CodedOutputStream *coded_out = new ::google::protobuf::io::CodedOutputStream(raw_out);

    std::string s;

    // write packet header, containing type of message we're sending
    PacketBuf::Packet p;
    p.set_type(7);
    p.SerializeToString(&s);

    coded_out->WriteVarint32(s.size());
    coded_out->WriteRaw(s.data(), s.size()); // ->WriteString(s)

    // write actual contents
    PacketBuf::ChatMessage msg;
    msg.set_message("THIS IS A CHAT MSG");
    msg .SerializeToString(&s);

    coded_out->WriteVarint32(s.size());
    coded_out->WriteString(s);

    delete coded_out;
    delete raw_out;
}

void Packet::deserialize(std::stringstream& in)
{
    google::protobuf::io::ZeroCopyInputStream *raw_in = new ::google::protobuf::io::IstreamInputStream(&in);
    google::protobuf::io::CodedInputStream *coded_in = new ::google::protobuf::io::CodedInputStream(raw_in);

    std::string s;

    //packet header
    uint32_t msgSize;
    coded_in->ReadVarint32(&msgSize);
    assert(msgSize > 0);

    if (coded_in->ReadString(&s, msgSize)) {
        PacketBuf::Packet p;
        p.ParseFromString(s);
    } else {
        assert(0);
    }

    //packet contents
    coded_in->ReadVarint32(&msgSize);
    if (coded_in->ReadString(&s, msgSize)) {
        PacketBuf::ChatMessage msg;
        msg.ParseFromString(s);
        std::cout << "PACKET CONTENTS CHATMSG: " << msg.message() << "\n";
    } else {
        assert(0);
    }


    delete coded_in;
    delete raw_in;
}
