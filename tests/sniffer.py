###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

import os
import json
from collections import OrderedDict
import subprocess
import re
from opts import debug, err, status
from typing import Callable


class TlvStruct:
    '''Represents part of an IEEE1905.1 TLV in a Packet.'''

    def __init__(self, captured, level):
        '''Convert the dict from tshark JSON into a TlvStruct.'''
        for name, value in captured.items():
            name = name.split('.', level)[-1].lower()
            name = name.translate(str.maketrans('. ', '__'))
            if name.endswith('_list') and isinstance(value, dict):
                name = name[:-len('_list')]
                # Normally, wireshark converts list fields to JSON as follows:
                # {"Operating classes list": {"Operating class 0": {...}, "Operating class 1": ...}}
                # However, there seems to be a bug for an operating class's channel list in the
                # channel preference TLV: the middle dictionary is left out, and only a single
                # channel is shown. To work around this, treat the channel list specially.
                if not isinstance(list(value.values())[0], dict):
                    value = [TlvStruct(value, level + 1)]
                else:
                    value = [TlvStruct(v, level + 1) for k, v in value.items()]
            name = self.valid_var_name(name)
            setattr(self, name, value)

    def __repr__(self):
        return "{" + ", ".join(["{!r}: {!r}".format(k, v) for k, v in self._d().items()]) + "}"

    def _d(self):
        return {k: v for k, v in self.__dict__.items() if not k.startswith('_')}

    def __eq__(self, other):
        return self._d() == other._d()

    def valid_var_name(self, varStr): return re.sub(r'\W|^(?=\d)', '_', varStr)


class Tlv(TlvStruct):
    '''Represents an IEEE1905.1 TLV in a Packet.'''

    def __init__(self, captured):
        '''Convert the dict from tshark JSON into a Tlv.'''
        super().__init__(captured, 1)
        self.tlv_type = int(self.tlv_type)
        self.tlv_length = int(self.tlv_length)


class Packet:
    '''Represents a single (IEEE1905.1) packet.'''

    def __init__(self, captured):
        '''Convert the dict from tshark JSON into a Packet.'''
        # Only the layers are of interest, discard the rest
        self.layers = captured['_source']['layers']
        self.frame_number = int(self.layers['frame']['frame.number'], 0)
        self.frame_time_epoch = float(self.layers['frame']['frame.time_epoch'])
        self.frame_time = self.layers['frame']['frame.time']
        self.eth_src = self.layers['eth']['eth.src']
        self.eth_dst = self.layers['eth']['eth.dst']
        ieee1905 = self.layers.get('ieee1905')
        if ieee1905:
            self.ieee1905 = True
            self.ieee1905_message_type = int(ieee1905['ieee1905.message_type'], 0)
            self.ieee1905_mid = int(ieee1905['ieee1905.message_id'], 0)
            self.ieee1905_fragment_id = int(ieee1905['ieee1905.fragment_id'], 0)
            flags_tree = ieee1905['ieee1905.flags_tree']
            self.ieee1905_last_fragment = bool(int(flags_tree['ieee1905.last_fragment']))
            self.ieee1905_relay_indicator = bool(int(flags_tree['ieee1905.relay_indicator']))
            self.ieee1905_tlvs = [Tlv(fields)
                                  for name, fields in ieee1905.items()
                                  if not name.startswith('ieee1905.') and
                                  fields['ieee1905.tlv_type'] != '0']
        else:
            self.ieee1905 = False

    def __repr__(self):
        d = {
            "frame_number": self.frame_number,
            "frame_time": self.frame_time,
            "eth_src": self.eth_src,
            "eth_dst": self.eth_dst
        }
        if self.ieee1905:
            d["message_type"] = hex(self.ieee1905_message_type)
            d["mid"] = hex(self.ieee1905_mid)
            if self.ieee1905_relay_indicator:
                d["relay"] = "1"
            d["tlvs"] = self.ieee1905_tlvs
        return "{" + ", ".join(["{!r}: {!r}".format(k, v) for k, v in d.items()]) + "}"


class Sniffer:
    '''Captures packets on an interface.'''

    def __init__(self, interface: str, tcpdump_log_dir: str):
        self.interface = interface
        self.tcpdump_log_dir = tcpdump_log_dir
        self.tcpdump_proc = None
        self.current_outputfile = None
        self.checkpoint_frame_number = 0

    def start(self, outputfile_basename):
        '''Start tcpdump to outputfile.'''
        debug("Starting tcpdump, output file {}.pcap".format(outputfile_basename))
        os.makedirs(os.path.join(self.tcpdump_log_dir, 'logs'), exist_ok=True)
        # dumpcap drops its root priviledges and becomes unable to write to log directory.
        # Make log directory accessible to everyone to fix it.
        os.chmod(self.tcpdump_log_dir, 0o777)
        self.current_outputfile = os.path.join(self.tcpdump_log_dir, outputfile_basename) + ".pcap"
        self.checkpoint_frame_number = 0
        # '-q' avoids the output, which we don't need.
        command = ["dumpcap", "-i", self.interface, '-q', '-w', self.current_outputfile, "-f",
                   "ether proto 0x88CC or ether proto 0x893A"]
        self.tcpdump_proc = subprocess.Popen(command, stderr=subprocess.PIPE)
        # dumpcap takes a while to start up. Wait for the appropriate output before continuing.
        # poll() so we exit the loop if dumpcap terminates for any reason.
        while not self.tcpdump_proc.poll():
            line = self.tcpdump_proc.stderr.readline()
            debug(line.decode()[:-1])  # strip off newline
            if line.startswith(b"File: " + self.current_outputfile.encode()):
                # Make sure it doesn't block due to stderr buffering
                self.tcpdump_proc.stderr.close()
                break
        else:
            err("tcpdump terminated")
            for line in self.tcpdump_proc.stderr.readlines():
                err(line.decode())
            self.tcpdump_proc = None
            self.current_outputfile = None

        # give rights to everyone so any cleanup job can delete network dumps
        if self.current_outputfile:
            os.chmod(self.current_outputfile, 0o777)

    def get_packet_capture(self):
        '''Get a list of packets from the last started tcpdump.'''
        if not self.current_outputfile:
            raise ValueError("get_packet_capture but no capture file")

        # tshark just prints "[]" and exits with 0 if .pcap file doesn't exist
        # so verifying that capture file exists externally
        if not os.path.isfile(self.current_outputfile):
            raise ValueError("Capture file {} does not exist".format(self.current_outputfile))

        tshark_command = ['tshark', '-r', self.current_outputfile, '-T', 'json',
                          '-Y', 'frame.number >= {}'.format(self.checkpoint_frame_number)]
        tshark_result = subprocess.run(tshark_command, stdout=subprocess.PIPE,
                                       stderr=subprocess.PIPE)
        if tshark_result.returncode != 0:
            debug(tshark_result.stderr)
            debug("tshark failed: {}".format(tshark_result.returncode))
        # Regardless of the exit code, try to make something of the JSON that comes out, if any.
        try:
            # tlvs which have the same type are all recorded with the same key therefore we lose
            # all but one of them if we use json.loads(tshark_result.stdout) directly.
            # https://stackoverflow.com/questions/29321677/python-json-parser-allow-duplicate-keys
            def rename_duplicate_key(key, dct):
                counter = 0
                unique_key = key

                while unique_key in dct:
                    counter += 1
                    unique_key = '{} {}'.format(key, counter + 1)
                return unique_key

            def rename_duplicates(pairs):
                dct = OrderedDict()
                for key, value in pairs:
                    if key in dct:
                        key = rename_duplicate_key(key, dct)
                    dct[key] = value

                return dct

            decoder = json.JSONDecoder(object_pairs_hook=rename_duplicates)
            capture = decoder.decode(tshark_result.stdout.decode('utf8'))

            return [Packet(x) for x in capture]
        except json.JSONDecodeError as error:
            raise ValueError("capture JSON decoding failed: {}".format(error))

    def get_cmdu_capture(self, match_function: Callable[[Packet], bool]) -> [Packet]:
        """Get a list of filtered IEEE1905.1 packets from the last started tcpdump.

        Parameters
        ----------
        match_function: Callable[[Packet], bool]
            A function that returns True if it is the expected packet. It is called on every packet
            returned by get_packet_capture.

        Returns
        -------
        [Packet]
            The matching packets.
        """
        capture = self.get_packet_capture()
        return [packet for packet in capture if packet.ieee1905 and match_function(packet)]

    def get_cmdu_capture_type(
        self, msg_type: int, eth_src: str, eth_dst: str = None, mid: int = None
    ) -> [Packet]:
        """Get a list of IEEE1905.1 packets matching criteria from the last started tcpdump.

        Parameters
        ----------
        msg_type: int
            CMDU message type that is expected.

        eth_src: str
            MAC address of the sender that is expected.

        eth_dst: str
            MAC address of the destination that is expected. If omitted, the IEEE1905.1 multicast
            MAC address is used.

        mid: int
            Message Identifier that is expected. If omitted, the MID is not checked.

        Returns
        -------
        [Packet]
            The matching packets.
        """
        if eth_dst is None:
            eth_dst = "01:80:c2:00:00:13"
        return self.get_cmdu_capture(
            lambda packet: (packet.eth_src == eth_src and
                            packet.eth_dst == eth_dst and
                            packet.ieee1905_message_type == msg_type and
                            (mid is None or packet.ieee1905_mid == mid)))

    def checkpoint(self):
        '''Checkpoint the capture.

        Any subsequent calls to get_packet_capture will only return packets capture after now.
        '''
        capture = self.get_packet_capture()
        if capture:
            self.checkpoint_frame_number = capture[-1].frame_number + 1
        # else keep last checkpoint

    def stop(self):
        '''Stop tcpdump if it is running.'''
        if self.tcpdump_proc:
            status("Terminating tcpdump")
            self.tcpdump_proc.terminate()
            self.tcpdump_proc = None
            self.current_outputfile = None
