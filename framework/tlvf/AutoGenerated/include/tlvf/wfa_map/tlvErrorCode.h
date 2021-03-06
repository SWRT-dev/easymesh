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

#ifndef _TLVF_WFA_MAP_TLVERRORCODE_H_
#define _TLVF_WFA_MAP_TLVERRORCODE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <ostream>

namespace wfa_map {


class tlvErrorCode : public BaseClass
{
    public:
        tlvErrorCode(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvErrorCode(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvErrorCode();

        enum eReasonCode: uint8_t {
            RESERVED = 0x0,
            STA_ASSOCIATED_WITH_A_BSS_OPERATED_BY_THE_AGENT = 0x1,
            STA_NOT_ASSOCIATED_WITH_ANY_BSS_OPERATED_BY_THE_AGENT = 0x2,
            CLIENT_CAPABILITY_REPORT_UNSPECIFIED_FAILURE = 0x3,
            BACKHAUL_STEERING_REQUEST_REJECTED_CANNOT_OPERATE_ON_CHANNEL_SPECIFIED = 0x4,
            BACKHAUL_STEERING_REQUEST_REJECTED_TARGET_BSS_SIGNAL_NOT_SUITABLE = 0x5,
            BACKHAUL_STEERING_REQUEST_AUTHENTICATION_OR_ASSOCIATION_REJECTED = 0x6,
        };
        // Enum AutoPrint generated code snippet begining- DON'T EDIT!
        // clang-format off
        static const char *eReasonCode_str(eReasonCode enum_value) {
            switch (enum_value) {
            case RESERVED:                                                               return "RESERVED";
            case STA_ASSOCIATED_WITH_A_BSS_OPERATED_BY_THE_AGENT:                        return "STA_ASSOCIATED_WITH_A_BSS_OPERATED_BY_THE_AGENT";
            case STA_NOT_ASSOCIATED_WITH_ANY_BSS_OPERATED_BY_THE_AGENT:                  return "STA_NOT_ASSOCIATED_WITH_ANY_BSS_OPERATED_BY_THE_AGENT";
            case CLIENT_CAPABILITY_REPORT_UNSPECIFIED_FAILURE:                           return "CLIENT_CAPABILITY_REPORT_UNSPECIFIED_FAILURE";
            case BACKHAUL_STEERING_REQUEST_REJECTED_CANNOT_OPERATE_ON_CHANNEL_SPECIFIED: return "BACKHAUL_STEERING_REQUEST_REJECTED_CANNOT_OPERATE_ON_CHANNEL_SPECIFIED";
            case BACKHAUL_STEERING_REQUEST_REJECTED_TARGET_BSS_SIGNAL_NOT_SUITABLE:      return "BACKHAUL_STEERING_REQUEST_REJECTED_TARGET_BSS_SIGNAL_NOT_SUITABLE";
            case BACKHAUL_STEERING_REQUEST_AUTHENTICATION_OR_ASSOCIATION_REJECTED:       return "BACKHAUL_STEERING_REQUEST_AUTHENTICATION_OR_ASSOCIATION_REJECTED";
            }
            static std::string out_str = std::to_string(int(enum_value));
            return out_str.c_str();
        }
        friend inline std::ostream &operator<<(std::ostream &out, eReasonCode value) { return out << eReasonCode_str(value); }
        // clang-format on
        // Enum AutoPrint generated code snippet end
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        eReasonCode& reason_code();
        sMacAddr& sta_mac();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        eReasonCode* m_reason_code = nullptr;
        sMacAddr* m_sta_mac = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVERRORCODE_H_
