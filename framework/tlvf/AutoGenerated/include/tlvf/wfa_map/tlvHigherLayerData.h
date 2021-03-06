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

#ifndef _TLVF_WFA_MAP_TLVHIGHERLAYERDATA_H_
#define _TLVF_WFA_MAP_TLVHIGHERLAYERDATA_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include <tuple>
#include <ostream>

namespace wfa_map {


class tlvHigherLayerData : public BaseClass
{
    public:
        tlvHigherLayerData(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvHigherLayerData(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvHigherLayerData();

        enum eProtocol: uint8_t {
            TR_181 = 0x1,
        };
        // Enum AutoPrint generated code snippet begining- DON'T EDIT!
        // clang-format off
        static const char *eProtocol_str(eProtocol enum_value) {
            switch (enum_value) {
            case TR_181: return "TR_181";
            }
            static std::string out_str = std::to_string(int(enum_value));
            return out_str.c_str();
        }
        friend inline std::ostream &operator<<(std::ostream &out, eProtocol value) { return out << eProtocol_str(value); }
        // clang-format on
        // Enum AutoPrint generated code snippet end
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        eProtocol& protocol();
        size_t payload_length() { return m_payload_idx__ * sizeof(uint8_t); }
        uint8_t* payload(size_t idx = 0);
        bool set_payload(const void* buffer, size_t size);
        bool alloc_payload(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eProtocol* m_protocol = nullptr;
        uint8_t* m_payload = nullptr;
        size_t m_payload_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVHIGHERLAYERDATA_H_
