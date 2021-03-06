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

#ifndef _TLVF_WFA_MAP_TLVCLIENTCAPABILITYREPORT_H_
#define _TLVF_WFA_MAP_TLVCLIENTCAPABILITYREPORT_H_

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


class tlvClientCapabilityReport : public BaseClass
{
    public:
        tlvClientCapabilityReport(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvClientCapabilityReport(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvClientCapabilityReport();

        enum eResultCode: uint8_t {
            SUCCESS = 0x0,
            FAILURE = 0x1,
        };
        // Enum AutoPrint generated code snippet begining- DON'T EDIT!
        // clang-format off
        static const char *eResultCode_str(eResultCode enum_value) {
            switch (enum_value) {
            case SUCCESS: return "SUCCESS";
            case FAILURE: return "FAILURE";
            }
            static std::string out_str = std::to_string(int(enum_value));
            return out_str.c_str();
        }
        friend inline std::ostream &operator<<(std::ostream &out, eResultCode value) { return out << eResultCode_str(value); }
        // clang-format on
        // Enum AutoPrint generated code snippet end
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        eResultCode& result_code();
        size_t association_frame_length() { return m_association_frame_idx__ * sizeof(uint8_t); }
        uint8_t* association_frame(size_t idx = 0);
        bool set_association_frame(const void* buffer, size_t size);
        bool alloc_association_frame(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eResultCode* m_result_code = nullptr;
        uint8_t* m_association_frame = nullptr;
        size_t m_association_frame_idx__ = 0;
        int m_lock_order_counter__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVCLIENTCAPABILITYREPORT_H_
