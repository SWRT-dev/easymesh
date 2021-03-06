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

#ifndef _TLVF_WFA_MAP_TLVPROFILE2STEERINGREQUEST_H_
#define _TLVF_WFA_MAP_TLVPROFILE2STEERINGREQUEST_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"
#include "tlvf/common/sMacAddr.h"
#include <tuple>
#include <asm/byteorder.h>
#include <ostream>

namespace wfa_map {


class tlvProfile2SteeringRequest : public BaseClass
{
    public:
        tlvProfile2SteeringRequest(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvProfile2SteeringRequest(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvProfile2SteeringRequest();

        typedef struct sRequestFlags {
            #if defined(__LITTLE_ENDIAN_BITFIELD)
            uint8_t reserved : 5;
            uint8_t btm_abridged_bit : 1;
            uint8_t btm_disassociation_imminent_bit : 1;
            uint8_t request_mode : 1;
            #elif defined(__BIG_ENDIAN_BITFIELD)
            uint8_t request_mode : 1;
            uint8_t btm_disassociation_imminent_bit : 1;
            uint8_t btm_abridged_bit : 1;
            uint8_t reserved : 5;
            #else
            #error "Bitfield macros are not defined"
            #endif
            void struct_swap(){
            }
            void struct_init(){
                reserved = 0x0;
            }
        } __attribute__((packed)) sRequestFlags;
        
        enum eRequestMode {
            REQUEST_IS_A_STEERING_OPPORTUNITY = 0x0,
            REQUEST_IS_A_STEERING_MANDATE_TO_TRIGGER_STEERING = 0x1,
        };
        // Enum AutoPrint generated code snippet begining- DON'T EDIT!
        // clang-format off
        static const char *eRequestMode_str(eRequestMode enum_value) {
            switch (enum_value) {
            case REQUEST_IS_A_STEERING_OPPORTUNITY:                 return "REQUEST_IS_A_STEERING_OPPORTUNITY";
            case REQUEST_IS_A_STEERING_MANDATE_TO_TRIGGER_STEERING: return "REQUEST_IS_A_STEERING_MANDATE_TO_TRIGGER_STEERING";
            }
            static std::string out_str = std::to_string(int(enum_value));
            return out_str.c_str();
        }
        friend inline std::ostream &operator<<(std::ostream &out, eRequestMode value) { return out << eRequestMode_str(value); }
        // clang-format on
        // Enum AutoPrint generated code snippet end
        
        typedef struct sTargetBssidInfo {
            //Wildcard BSSID is represented by FF:FF:FF:FF:FF:FF.
            sMacAddr target_bssid;
            uint8_t target_bss_operating_class;
            uint8_t target_bss_channel_number;
            uint8_t target_bss_reason_code;
            void struct_swap(){
                target_bssid.struct_swap();
            }
            void struct_init(){
            }
        } __attribute__((packed)) sTargetBssidInfo;
        
        const eTlvTypeMap& type();
        const uint16_t& length();
        sMacAddr& bssid();
        sRequestFlags& request_flags();
        //Time period in seconds (from reception of the Steering Request message) for which the request is valid.
        //If Request Mode bit is 1, then this field is ignored 
        uint16_t& steering_opportunity_window_sec();
        //Time period in TUs of the disassociation timer in the BTM Request.
        uint16_t& btm_disassociation_timer_ms();
        uint8_t& sta_list_length();
        std::tuple<bool, sMacAddr&> sta_list(size_t idx);
        bool alloc_sta_list(size_t count = 1);
        uint8_t& target_bssid_list_length();
        std::tuple<bool, sTargetBssidInfo&> target_bssid_list(size_t idx);
        bool alloc_target_bssid_list(size_t count = 1);
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        sMacAddr* m_bssid = nullptr;
        sRequestFlags* m_request_flags = nullptr;
        uint16_t* m_steering_opportunity_window_sec = nullptr;
        uint16_t* m_btm_disassociation_timer_ms = nullptr;
        uint8_t* m_sta_list_length = nullptr;
        sMacAddr* m_sta_list = nullptr;
        size_t m_sta_list_idx__ = 0;
        int m_lock_order_counter__ = 0;
        uint8_t* m_target_bssid_list_length = nullptr;
        sTargetBssidInfo* m_target_bssid_list = nullptr;
        size_t m_target_bssid_list_idx__ = 0;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVPROFILE2STEERINGREQUEST_H_
