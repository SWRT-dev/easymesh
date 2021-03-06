###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

from .prplmesh_base import PrplMeshBase
from environment import VirtualAPDocker
from opts import err


class PrplMeshStationDummy(PrplMeshBase):

    model = "STA_dummy"
    __mac_base = 0

    def __init__(self, *args, **kwargs):
        '''Generate dummy Station.'''
        self.args = args
        self.kwargs = kwargs
        config = kwargs.get("config", kwargs)

        self.name = config.get("name", "station")
        self.mac = config.get("mac", None)
        self.hostname = config.get("hostname", None)
        self.ipv4 = config.get("ipv4", None)
        self.ipv6 = config.get("ipv6", None)
        self.associated_vap = None

        if self.mac is None:
            raise ValueError(err("{} device \"{}\" has no MAC!".format(self.model, self.name)))

    def wifi_connect(self, vap: VirtualAPDocker) -> bool:
        """Connect to the Access Point. Return True if successful."""
        self.associated_vap = vap

        vap.radio.send_bwl_event("EVENT AP-STA-CONNECTED {}".format(self.mac))
        return True

    def wifi_disconnect(self, vap: VirtualAPDocker) -> bool:
        '''Disassociate "sta" from this VAP.'''
        self.associated_vap = None

        vap.radio.send_bwl_event("EVENT AP-STA-DISCONNECTED {}".format(self.mac))
        return True

    def wifi_connect_check(self, vap: VirtualAPDocker) -> bool:
        """Connect and verify connection"""
        return self.wifi_connect(vap)

    def disable_wifi(self) -> bool:
        """Disable wifi connection"""
        return True
