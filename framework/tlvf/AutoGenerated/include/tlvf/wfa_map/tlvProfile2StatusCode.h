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

#ifndef _TLVF_WFA_MAP_TLVPROFILE2STATUSCODE_H_
#define _TLVF_WFA_MAP_TLVPROFILE2STATUSCODE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <string.h>
#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include "tlvf/wfa_map/eTlvTypeMap.h"

namespace wfa_map {


class tlvProfile2StatusCode : public BaseClass
{
    public:
        tlvProfile2StatusCode(uint8_t* buff, size_t buff_len, bool parse = false);
        explicit tlvProfile2StatusCode(std::shared_ptr<BaseClass> base, bool parse = false);
        ~tlvProfile2StatusCode();

        const eTlvTypeMap& type();
        const uint16_t& length();
        uint16_t& status_code();
        void class_swap() override;
        bool finalize() override;
        static size_t get_initial_size();

    private:
        bool init();
        eTlvTypeMap* m_type = nullptr;
        uint16_t* m_length = nullptr;
        uint16_t* m_status_code = nullptr;
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_TLVPROFILE2STATUSCODE_H_
