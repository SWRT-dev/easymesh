///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvTimestamp.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvTimestamp::tlvTimestamp(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvTimestamp::tlvTimestamp(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvTimestamp::~tlvTimestamp() {
}
const eTlvTypeMap& tlvTimestamp::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvTimestamp::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvTimestamp::timestamp_length() {
    return (uint8_t&)(*m_timestamp_length);
}

std::string tlvTimestamp::timestamp_str() {
    char *timestamp_ = timestamp();
    if (!timestamp_) { return std::string(); }
    auto str = std::string(timestamp_, m_timestamp_idx__);
    auto pos = str.find_first_of('\0');
    if (pos != std::string::npos) {
        str.erase(pos);
    }
    return str;
}

char* tlvTimestamp::timestamp(size_t length) {
    if( (m_timestamp_idx__ == 0) || (m_timestamp_idx__ < length) ) {
        TLVF_LOG(ERROR) << "timestamp length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_timestamp);
}

bool tlvTimestamp::set_timestamp(const std::string& str) { return set_timestamp(str.c_str(), str.size()); }
bool tlvTimestamp::set_timestamp(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_timestamp received a null pointer.";
        return false;
    }
    if (m_timestamp_idx__ != 0) {
        TLVF_LOG(ERROR) << "set_timestamp was already allocated!";
        return false;
    }
    if (!alloc_timestamp(size)) { return false; }
    std::copy(str, str + size, m_timestamp);
    return true;
}
bool tlvTimestamp::alloc_timestamp(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list timestamp, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_timestamp[*m_timestamp_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_timestamp_idx__ += count;
    *m_timestamp_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvTimestamp::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
}

bool tlvTimestamp::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t tlvTimestamp::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // timestamp_length
    return class_size;
}

bool tlvTimestamp::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_TIMESTAMP;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_timestamp_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_timestamp_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_timestamp = reinterpret_cast<char*>(m_buff_ptr__);
    uint8_t timestamp_length = *m_timestamp_length;
    m_timestamp_idx__ = timestamp_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (timestamp_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (timestamp_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_TIMESTAMP) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_TIMESTAMP) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


