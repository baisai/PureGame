/*
 * Copyright (c) 2023-present ChenDong, email <baisaichen@live.com>. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

#include "PureCore/Buffer/ArrayBuffer.h"
#include "PureCore/Memory/ObjectCache.h"
#include "PureNet/Protocol/Protocol.h"

namespace PureNet {
enum EWebSocketState {
    EWebSocketInvalid = 0,
    EWebSocketConnected = 1,
    EWebSocketUpgrade = 2,
    EWebSocketHandshakeOK = 4,
};

// 	 0                   1                   2                   3
// 	 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// 	+-+-+-+-+-------+-+-------------+-------------------------------+
// 	|F|R|R|R| opcode|M| Payload len |    Extended payload length    |
// 	|I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
// 	|N|V|V|V|       |S|             |   (if payload len==126/127)   |
// 	| |1|2|3|       |K|             |                               |
// 	+-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
// 	|     Extended payload length continued, if payload len == 127  |
// 	+ - - - - - - - - - - - - - - - +-------------------------------+
// 	|                               |Masking-key, if MASK set to 1  |
// 	+-------------------------------+-------------------------------+
// 	| Masking-key (continued)       |          Payload Data         |
// 	+-------------------------------- - - - - - - - - - - - - - - - +
// 	:                     Payload Data continued ...                :
// 	+ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
// 	|                     Payload Data continued ...                |
// 	+---------------------------------------------------------------+
struct WebSocketHead {
    uint8_t mFin : 1;
    uint8_t mRsv : 3;
    uint8_t mOpcode : 4;
    uint8_t mMask : 1;
    uint8_t mPayload : 7;

    void clear();
    int unpack(PureCore::DataRef src);
    int pack(PureCore::DataRef dest) const;
};

static const uint32_t sWebSocketHeadMaxSize = 14u;
static const uint8_t sWebSocketMaskSize = 4u;

enum EWebSockeOpcode {
    EWebSocketOpcodeEmpty = 0x0,
    EWebSocketOpcodeText = 0x1,
    EWebSocketOpcodeBin = 0x2,

    EWebSocketOpcodeDisconn = 0x8,
    EWebSocketOpcodePing = 0x9,
    EWebSocketOpcodePong = 0xa,

    EWebSocketOpcodeUnknown = 0xff,
};

class WebSocketProtocol;
class PURENET_API WebSocketHandshakeHttp {
public:
    WebSocketHandshakeHttp() = default;
    ~WebSocketHandshakeHttp() = default;

    void clear();

    int read(PureCore::IBuffer& buffer);

    void set_owner(WebSocketProtocol* owner);

    void set_client_key(PureCore::StringRef key);
    PureCore::StringRef get_client_key() const;

    bool check_head(const std::string& key, const std::string& val) const;
    const std::string& get_head(const std::string& key);
    void add_head(const std::string& key, const std::string& val);

    int16_t get_status_code() const;
    void set_status_code(int16_t c);

    std::string get_http() const;

private:
    WebSocketProtocol* mOwner = nullptr;
    std::string mData;
    int16_t mStatusCode = 0;
    std::unordered_map<std::string, std::string> mHeads;
    std::string mClietKey;
};

class Link;
class NetMsg;
class PURENET_API WebSocketProtocol : public Protocol {
public:
    friend class WebSocketHandshakeHttp;
    WebSocketProtocol() = default;
    virtual ~WebSocketProtocol() = default;

    virtual int start(Link* l) override;
    virtual int read(Link* l, PureCore::IBuffer& buffer) override;
    virtual int write(Link* l, PureCore::IBuffer& buffer, int64_t leftSize, int64_t totalSize) override;
    virtual int end(Link* l) override;

    bool is_handshake_ok();

private:
    int read_head(PureCore::IBuffer& buffer);
    bool has_head() const;
    bool is_ws_finish() const;
    EWebSockeOpcode get_opcode() const;
    uint64_t get_data_size() const;
    const char* get_mask() const;
    int write_head(EWebSockeOpcode opcode, char mask[sWebSocketMaskSize], size_t dataSize);

private:
    int handshake();
    int on_handshake_finish();

protected:
    Link* mLink = nullptr;
    WebSocketHandshakeHttp* mHttp = nullptr;
    EWebSocketState mState = EWebSocketInvalid;
    PureCore::ArrayBuffer<sWebSocketHeadMaxSize> mWritingHead;
    uint64_t mNeedSize = 0;
    uint16_t mNeedHeadSize = 0;
    uint8_t mReadMaskIdx = 0;
    WebSocketHead mReadHead;
    PureCore::ArrayBuffer<sWebSocketHeadMaxSize> mReadingHead;

private:
    static thread_local PureCore::ObjectCache<WebSocketHandshakeHttp, 128> tHttpPool;

    PURE_DISABLE_COPY(WebSocketProtocol)
};

}  // namespace PureNet
