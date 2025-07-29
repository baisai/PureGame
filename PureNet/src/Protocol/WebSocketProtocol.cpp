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

#include "PureCore/ByteOrder.h"
#include "PureCore/OsHelper.h"
#include "PureCore/StringRef.h"
#include "PureEncrypt/PureMD5.h"
#include "PureEncrypt/PureBase64.h"
#include "PureEncrypt/PureSha1.h"
#include "PureEncrypt/EncryptErrorDesc.h"
#include "PureNet/NetErrorDesc.h"
#include "PureNet/Protocol/WebSocketProtocol.h"
#include "PureNet/Link.h"
#include "PureNet/PureNetReacter.h"

namespace PureNet {
static const std::string sWebSocketUrl = "/chat";
static const std::string sWebSocketUpgradeKey = "Upgrade";
static const std::string sWebSocketUpgradeValue = "websocket";
static const std::string sWebSocketConnectionKey = "Connection";
static const std::string sWebSocketConnectionValue = "Upgrade";
static const std::string sWebSocketOriginKey = "Origin";
static const std::string sWebSocketOriginValue = "null";
static const std::string sWebSocketProtocolKey = "Sec-WebSocket-Protocol";
static const std::string sWebSocketProtocolValue = "chat";
static const std::string sWebSocketVersionKey = "Sec-WebSocket-Version";
static const std::string sWebSocketVersionValue = "13";
static const std::string sWebSocketKeyKey = "Sec-WebSocket-Key";
static const std::string sWebSocketAcceptKey = "Sec-WebSocket-Accept";
static const std::string sWebSocketGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

///////////////////////////////////////////////////////////////////////////
// WebSocketHead
//////////////////////////////////////////////////////////////////////////
void WebSocketHead::clear() {
    mFin = 0;
    mRsv = 0;
    mOpcode = 0;
    mMask = 0;
    mPayload = 0;
}

int WebSocketHead::unpack(PureCore::DataRef src) {
    if (src.size() < 2) {
        return ErrorInvalidArg;
    }
    mFin = src[0] >> 7u;
    mRsv = src[0] & 0x70u;
    mOpcode = src[0] & 0xf;
    mMask = src[1] >> 7u;
    mPayload = src[1] & 0x7f;
    return Success;
}
int WebSocketHead::pack(PureCore::DataRef dest) const {
    if (dest.size() < 2) {
        return ErrorInvalidArg;
    }
    dest[0] = mFin << 7u;
    dest[0] |= (0x70u & mRsv);
    dest[0] |= (0xf & mOpcode);
    dest[1] |= (mMask << 7u);
    dest[1] |= (0x7f & mPayload);
    return Success;
}

///////////////////////////////////////////////////////////////////////////
// WebSocketHandshakeHttp
//////////////////////////////////////////////////////////////////////////
void WebSocketHandshakeHttp::clear() {
    mOwner = nullptr;
    mStatusCode = 0;
    mClietKey.clear();
    mData.clear();
    mHeads.clear();
}

int WebSocketHandshakeHttp::read(PureCore::IBuffer& buffer) {
    if (mOwner == nullptr) {
        return ErrorNullPointer;
    }
    if (mData.size() + buffer.size() > 1024) {
        return ErrorHttpParseFailed;
    }
    auto data = buffer.data();
    mData.append(data.data(), data.size());
    auto pos = mData.rfind("\r\n\r\n");
    if (pos == std::string::npos) {
        return Success;
    }
    size_t s = 0, e = 0;
    while (s < pos) {
        e = mData.find("\r\n", s);
        std::string line = mData.substr(s, e);
        auto idx = line.find(':');
        if (idx == std::string::npos) {
            auto sIdx = line.find(' ');
            auto eIdx = line.rfind(' ');
            if (sIdx == std::string::npos || sIdx >= eIdx) {
                return ErrorHttpParseFailed;
            }
            std::string p1 = line.substr(0, sIdx);
            std::string p2 = line.substr(sIdx + 1, eIdx - sIdx - 1);
            for (auto& c : p1) {
                c = char(std::toupper(c));
            }
            if (p1.size() > 4 && PureCore::StringRef(p1.data(), 4) == "HTTP") {
                mStatusCode = int16_t(std::stoi(p2));
            }
        } else {
            std::string key = line.substr(0, idx);
            std::string val = line.substr(idx + 1);
            auto sIdx = key.find_first_not_of(' ');
            auto eIdx = key.find_last_not_of(' ');
            if (sIdx < key.size()) {
                key = key.substr(sIdx, eIdx - sIdx + 1);
            } else {
                key.clear();
            }
            sIdx = val.find_first_not_of(' ');
            eIdx = val.find_last_not_of(' ');
            if (sIdx < val.size()) {
                val = val.substr(sIdx, eIdx - sIdx + 1);
            } else {
                val.clear();
            }
            if (!key.empty() && !val.empty()) {
                mHeads[key] = std::move(val);
            }
        }
        if (e >= pos) {
            break;
        }
        s = e + 2;
    }
    return mOwner->handshake();
}

void WebSocketHandshakeHttp::set_owner(WebSocketProtocol* owner) { mOwner = owner; }

void WebSocketHandshakeHttp::set_client_key(PureCore::StringRef key) { mClietKey.assign(key.data(), key.size()); }

PureCore::StringRef WebSocketHandshakeHttp::get_client_key() const { return mClietKey; }

bool WebSocketHandshakeHttp::check_head(const std::string& key, const std::string& val) const {
    auto iter = mHeads.find(key);
    if (iter != mHeads.end()) {
        return iter->second == val;
    }
    return false;
}

static const std::string sEmpty;
const std::string& WebSocketHandshakeHttp::get_head(const std::string& key) {
    auto iter = mHeads.find(key);
    if (iter != mHeads.end()) {
        return iter->second;
    }
    return sEmpty;
}

void WebSocketHandshakeHttp::add_head(const std::string& key, const std::string& val) { mHeads[key] = val; }

int16_t WebSocketHandshakeHttp::get_status_code() const { return mStatusCode; }

void WebSocketHandshakeHttp::set_status_code(int16_t c) { mStatusCode = c; }

std::string WebSocketHandshakeHttp::get_http() const {
    std::string http;
    if (mStatusCode) {
        fmt::format_to(std::back_inserter(http), "HTTP/1.1 {} SWITCHING_PROTOCOLS", 101);
    } else {
        fmt::format_to(std::back_inserter(http), "GET {} HTTP/1.1", sWebSocketUrl);
    }
    return std::move(http);
}

///////////////////////////////////////////////////////////////////////////
// WebSocketProtocol
//////////////////////////////////////////////////////////////////////////
int WebSocketProtocol::start(Link* l) {
    mLink = l;
    mState = EWebSocketConnected;
    mHttp = tHttpPool.get();
    mHttp->set_owner(this);
    return handshake();
}

int WebSocketProtocol::read(Link* l, PureCore::IBuffer& buffer) {
    if (next() == nullptr) {
        return ErrorNullPointer;
    }
    if (!is_handshake_ok()) {
        if (mHttp == nullptr) {
            return ErrorWSNotHandshake;
        }
        return mHttp->read(buffer);
    }
    while (buffer.size() > 0) {
        if (mNeedSize == 0u) {
            mReadMaskIdx = 0u;
            mReadingHead.clear();
        }
        int err = read_head(buffer);
        if (err != Success) {
            return err;
        }
        if (mNeedHeadSize == 0u) {
            EWebSockeOpcode opcode = get_opcode();
            if (opcode == EWebSocketOpcodeDisconn) {
                l->link_mgr().close_link(l, Success);
                return Success;
            }
            if (opcode == EWebSocketOpcodeEmpty || opcode == EWebSocketOpcodePing || opcode == EWebSocketOpcodePong) {
                mNeedHeadSize = 0;
                mNeedSize = 0;
                return Success;
            }
            if (mNeedSize > 0u) {
                const char* mask = get_mask();
                auto data = buffer.data();
                if (mNeedSize < data.size()) {
                    data.reset(data.data(), mNeedSize);
                }
                PureCore::ReferBuffer ref(data);
                ref.write_pos(data.size());
                if (l->is_server()) {
                    if (mask == nullptr) {
                        return ErrorProtocolDataInvalid;
                    }
                    for (size_t i = 0; i < data.size(); ++i) {
                        data[i] = data[i] ^ mask[mReadMaskIdx++];
                        mReadMaskIdx &= 0x3u;
                    }
                } else {
                    if (mask != nullptr) {
                        return ErrorProtocolDataInvalid;
                    }
                }
                err = next()->read(l, ref);
                if (err != Success) {
                    return err;
                }
                mNeedSize -= data.size();
                buffer.read_pos(buffer.read_pos() + data.size());
            }
        }
    }
    return Success;
}

int WebSocketProtocol::write(Link* l, PureCore::IBuffer& buffer, int64_t leftSize, int64_t totalSize) {
    if (pre() == nullptr) {
        return ErrorNullPointer;
    }
    if (!is_handshake_ok()) {
        return ErrorWSNotHandshake;
    }
    char* maskBuffer = nullptr;
    union {
        uint32_t num;
        char data[4];
    } mask;
    if (!l->is_server()) {
        mask.num = l->reacter()->randomer().gen_int();
        maskBuffer = mask.data;
    }
    int err = write_head(EWebSocketOpcodeBin, maskBuffer, buffer.size());
    if (err != Success) {
        return err;
    }
    err = pre()->write(l, mWritingHead, leftSize, totalSize + mWritingHead.size());
    if (err != Success) {
        return err;
    }
    if (maskBuffer == nullptr) {
        err = pre()->write(l, buffer, 0, totalSize + mWritingHead.size());
        if (err != Success) {
            return err;
        }
    } else {
        uint8_t maskIdx = 0;
        auto data = buffer.data();
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = data[i] ^ mask.data[maskIdx++];
            maskIdx &= 0x3u;
        }
        err = pre()->write(l, buffer, 0, totalSize + mWritingHead.size());
        if (err != Success) {
            return err;
        }
    }
    return Success;
}

int WebSocketProtocol::end(Link* l) {
    if (mHttp != nullptr) {
        tHttpPool.free(mHttp);
        mHttp = nullptr;
    }

    if (pre() != nullptr && is_handshake_ok()) {
        if (write_head(EWebSocketOpcodeDisconn, nullptr, 0u) == Success) {
            pre()->write(l, mWritingHead, 0, mWritingHead.size());
        }
    }

    mLink = nullptr;
    mState = EWebSocketInvalid;
    mNeedSize = 0;
    mNeedHeadSize = 0;
    mReadMaskIdx = 0;
    mReadHead.clear();
    mReadingHead.clear();
    mWritingHead.clear();
    if (pre() == nullptr) {
        return Success;
    }
    return pre()->end(l);
}

bool WebSocketProtocol::is_handshake_ok() { return mState == EWebSocketHandshakeOK; }

int WebSocketProtocol::read_head(PureCore::IBuffer& buffer) {
    if (mNeedSize > 0) {
        return Success;
    }
    size_t readSize = 0;
    while (mNeedSize == 0u && buffer.size() > 0) {
        if (mNeedHeadSize == 0u) {
            mNeedHeadSize = 2u;
        }
        if (mNeedHeadSize > 0u) {
            readSize = buffer.size();
            if (mNeedHeadSize <= readSize) {
                readSize = mNeedHeadSize;
            }
            mReadingHead.write(PureCore::DataRef(buffer.data().data(), readSize));
            mNeedHeadSize -= uint16_t(readSize);
            buffer.read_pos(buffer.read_pos() + readSize);
        }
        if (mReadingHead.size() >= 2u) {
            int err = mReadHead.unpack(mReadingHead.data());
            if (err != Success) {
                return err;
            }
            readSize = 2u;
            if (mReadHead.mMask) {
                readSize += 4u;
            }
            if (mReadHead.mPayload == 126u) {
                readSize += 2u;
            } else if (mReadHead.mPayload == 127u) {
                readSize += 8u;
            }
            if (mReadingHead.size() > readSize) {
                mReadingHead.write_pos(mReadingHead.read_pos() + readSize);
                mNeedHeadSize = 0u;
            } else {
                mNeedHeadSize = uint16_t(readSize - mReadingHead.size());
            }
        }
        if (mNeedHeadSize == 0u) {
            mReadingHead.read_pos(2u);  // skip head
            mNeedSize = get_data_size();
        }
    }
    return Success;
}

bool WebSocketProtocol::has_head() const { return mNeedHeadSize == 0u; }

bool WebSocketProtocol::is_ws_finish() const {
    if (!has_head()) {
        return false;
    }
    return mReadHead.mFin != 0;
}

EWebSockeOpcode WebSocketProtocol::get_opcode() const {
    if (!has_head()) {
        return EWebSocketOpcodeUnknown;
    }
    return EWebSockeOpcode(mReadHead.mOpcode);
}

uint64_t WebSocketProtocol::get_data_size() const {
    if (!has_head()) {
        return 0u;
    }
    if (mReadHead.mPayload <= 125u) {
        return mReadHead.mPayload;
    } else if (mReadHead.mPayload == 126u) {
        if (mReadingHead.size() >= 2u) {
            uint16_t len = 0;
            _pure_byte_load16(uint16_t, mReadingHead.data().data(), &len);
            return len;
        }
    } else if (mReadHead.mPayload == 127u) {
        if (mReadingHead.size() >= 8u) {
            uint64_t len = 0;
            _pure_byte_load64(uint64_t, mReadingHead.data().data(), &len);
            return len;
        }
    }
    return 0u;
}

const char* WebSocketProtocol::get_mask() const {
    if (!has_head()) {
        return nullptr;
    }
    if (mReadHead.mMask == 0) {
        return nullptr;
    }
    if (mReadHead.mPayload <= 125u) {
        if (mReadingHead.size() >= 4u) {
            return mReadingHead.data().data();
        }
    } else if (mReadHead.mPayload == 126u) {
        if (mReadingHead.size() >= 6u) {
            return mReadingHead.data().data() + 2u;
        }
    } else if (mReadHead.mPayload == 127u) {
        if (mReadingHead.size() >= 12u) {
            return mReadingHead.data().data() + 8u;
        }
    }
    return nullptr;
}

int WebSocketProtocol::write_head(EWebSockeOpcode opcode, char mask[sWebSocketMaskSize], size_t dataSize) {
    mWritingHead.clear();
    mWritingHead.write_pos(2u);
    WebSocketHead head;
    head.mFin = 1u;
    head.mRsv = 0u;
    head.mOpcode = opcode;
    head.mMask = mask == nullptr ? 0u : 1u;
    if (dataSize <= 125u) {
        head.mPayload = dataSize;
    } else if (dataSize <= UINT16_MAX) {
        head.mPayload = 126u;
        _pure_byte_store16(mWritingHead.free_buffer().data(), uint16_t(dataSize));
        mWritingHead.write_pos(mWritingHead.write_pos() + 2u);
    } else {
        head.mPayload = 127u;
        _pure_byte_store64(mWritingHead.free_buffer().data(), dataSize);
        mWritingHead.write_pos(mWritingHead.write_pos() + 8u);
    }
    if (mask != nullptr) {
        int err = mWritingHead.write(PureCore::DataRef(mask, sWebSocketMaskSize));
        if (err != PureCore::Success) {
            return ErrorCoreBufferFailed;
        }
    }
    head.pack(mWritingHead.data());
    return Success;
}

int WebSocketProtocol::handshake() {
    if (pre() == nullptr) {
        return ErrorNullPointer;
    }
    if (mLink == nullptr || mHttp == nullptr) {
        return ErrorWSHandshakeFailed;
    }
    switch (mState) {
        case EWebSocketConnected:
            if (!mLink->is_server()) {
                char data[64] = {0};
                auto s = fmt::format_to_n(data, sizeof(data), "{}|{}|{}", mLink->reacter()->randomer().gen_int(), PureCore::get_process_id(),
                                          PureCore::system_milli_s());
                PureEncrypt::PureMD5 md5;
                if (md5.encode(PureCore::DataRef(s.out, s.size)) != PureEncrypt::Success) {
                    return ErrorWSHandshakeFailed;
                }
                PureEncrypt::PureBase64 base64;
                if (base64.encode(md5.get_output()) != PureEncrypt::Success) {
                    return ErrorWSHandshakeFailed;
                }
                mHttp->set_client_key(base64.get_output().data().bytes());
                WebSocketHandshakeHttp handMsg;
                handMsg.add_head(sWebSocketUpgradeKey, sWebSocketUpgradeValue);
                handMsg.add_head(sWebSocketConnectionKey, sWebSocketConnectionValue);
                handMsg.add_head(sWebSocketOriginKey, sWebSocketOriginValue);
                handMsg.add_head(sWebSocketProtocolKey, sWebSocketProtocolValue);
                handMsg.add_head(sWebSocketVersionKey, sWebSocketVersionValue);
                handMsg.add_head(sWebSocketKeyKey, mHttp->get_client_key());
                std::string httpData = handMsg.get_http();
                PureCore::ReferBuffer ref(httpData);
                ref.write_pos(httpData.size());
                int err = pre()->write(mLink, ref, 0, ref.size());
                if (err != Success) {
                    return err;
                }
            }
            mState = EWebSocketUpgrade;
            return Success;
            break;
        case EWebSocketUpgrade: {
            if (mHttp->check_head(sWebSocketUpgradeKey, sWebSocketUpgradeValue) && mHttp->check_head(sWebSocketConnectionKey, sWebSocketConnectionValue) &&
                mHttp->check_head(sWebSocketVersionKey, sWebSocketVersionValue)) {
                if (mLink->is_server()) {
                    std::string key = mHttp->get_head(sWebSocketKeyKey);
                    if (key.empty()) {
                        return ErrorWSHandshakeFailed;
                    }
                    key.append(sWebSocketGUID);
                    PureEncrypt::PureSha1 sha1;
                    if (sha1.encode(key) != PureEncrypt::Success) {
                        return ErrorWSHandshakeFailed;
                    }
                    PureEncrypt::PureBase64 base64;
                    if (base64.encode(sha1.get_output()) != PureEncrypt::Success) {
                        return ErrorWSHandshakeFailed;
                    }

                    WebSocketHandshakeHttp handMsg;
                    handMsg.set_status_code(101);
                    handMsg.add_head(sWebSocketUpgradeKey, sWebSocketUpgradeValue);
                    handMsg.add_head(sWebSocketConnectionKey, sWebSocketConnectionValue);
                    if (mHttp->check_head(sWebSocketProtocolKey, sWebSocketProtocolValue)) {
                        handMsg.add_head(sWebSocketProtocolKey, sWebSocketProtocolValue);
                    }
                    handMsg.add_head(sWebSocketVersionKey, sWebSocketVersionValue);
                    auto data = base64.get_output().data();
                    handMsg.add_head(sWebSocketAcceptKey, std::string(data.data(), data.size()));
                    std::string httpData = handMsg.get_http();
                    PureCore::ReferBuffer ref(httpData);
                    ref.write_pos(httpData.size());
                    int err = pre()->write(mLink, ref, 0, ref.size());
                    if (err == Success) {
                        err = on_handshake_finish();
                    }
                    return err;
                } else {
                    if (mHttp->get_status_code() != 101) {
                        return ErrorWSHandshakeFailed;
                    }
                    auto acceptKey = mHttp->get_head(sWebSocketAcceptKey);
                    std::string key = mHttp->get_client_key();
                    if (acceptKey.empty() || key.empty()) {
                        return ErrorWSHandshakeFailed;
                    }
                    key.append(sWebSocketGUID);
                    PureEncrypt::PureSha1 sha1;
                    if (sha1.encode(key) != PureEncrypt::Success) {
                        return ErrorWSHandshakeFailed;
                    }
                    PureEncrypt::PureBase64 base64;
                    if (base64.encode(sha1.get_output()) != PureEncrypt::Success) {
                        return ErrorWSHandshakeFailed;
                    }
                    if (PureCore::StringRef(acceptKey) != base64.get_output().data().bytes()) {
                        return ErrorWSHandshakeFailed;
                    }
                    return on_handshake_finish();
                }
            }
        }
            return ErrorWSHandshakeFailed;
            break;
        default:
            break;
    }
    return ErrorWSHandshakeFailed;
}

int WebSocketProtocol::on_handshake_finish() {
    if (mLink == nullptr) {
        return ErrorNullPointer;
    }
    mState = EWebSocketHandshakeOK;
    tHttpPool.free(mHttp);
    mHttp = nullptr;
    if (next() == nullptr) {
        return Success;
    }
    return next()->start(mLink);
}

thread_local PureCore::ObjectCache<WebSocketHandshakeHttp, 128> WebSocketProtocol::tHttpPool{};

}  // namespace PureNet
