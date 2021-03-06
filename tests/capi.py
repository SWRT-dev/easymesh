#!/usr/bin/env python3
import argparse
import logging
import socket
import re
import time
from enum import Enum
from typing import Dict, Union
from opts import debug

logger = logging.getLogger(__name__)


class CAPIReply(Enum):
    """An enum representing the possible CAPI replies."""
    RUNNING = "status,RUNNING"
    COMPLETE = "status,COMPLETE"  # parameters may follow
    INVALID = "status,INVALID"
    ERROR = "status,ERROR"


class tlv:
    """Representation of an 1905.1 TLV."""

    def __init__(self, type_: int, value: str):
        """A TLV has a type, a length and a value.

        Parameters
        ----------
        type_ : int
            The TLV type.

        length : int
            The length of the TLV. This must correspond to the length of `value`.

        value : str
            The TLV value, as a string. It must be formatted according to the UCC rules, i.e. with
            curly braces and hexadecimal numbers.
        """
        self.type = type_
        self.length = self.get_byte_length(value)
        self.value = value

    def get_byte_length(self, value: str) -> int:
        """Return tlv length for a given value

        Parameters
        ----------
        value : str
            The TLV value, as a string. It must be formatted according to the UCC rules, i.e. with
            curly braces and hexadecimal numbers.

        Returns
        -------
        int
            The length of the given value
        """
        byte_length = 0
        hex_values = re.split(' |:', re.sub('({|})', '', value))
        for num in hex_values:
            if num:
                if len(num) > 4:
                    byte_length += len(num[4:])/2
                byte_length += 1
        return int(byte_length)

    def format(self, tlv_num: int = 0) -> str:
        """Format the TLV for the dev_send_1905 CAPI command.

        Parameters
        ----------
        tlv_num : int
            The TLV counter. In the CAPI command, if there are multiple TLVs, they must be numbered
            starting from 1. `tlv_num` is that number. If there is only a single TLV, set it to
            zero.

        Returns
        -------
        str
            The string representation of the TLV according to CAPI definition.
        """
        if tlv_num:
            tlv_num_str = str(tlv_num)
        else:
            tlv_num_str = ''
        return "tlv_type{tlv_num_str},0x{tlv_type:02x}," \
               "tlv_length{tlv_num_str},0x{tlv_length:04x}," \
               "tlv_value{tlv_num_str},{tlv_value}".format(tlv_num_str=tlv_num_str,
                                                           tlv_type=self.type,
                                                           tlv_length=self.length,
                                                           tlv_value=self.value)


class UCCSocket:
    """Abstraction of the target listening socket.

    It connects to the listener and it sends and receives
    CAPI commands from it.
    """

    def __init__(self, host: str, port: int, timeout: int = 30):
        """Constructor for UCCSocket

        Parameters
        ----------
        host: str
            The host to connect to. Can either be an ip or a hostname.
        port: str
            The port to connect to.
        timeout: int
            (optional) The timeout for both creating a connection,
            and receiving or sending data.
        """
        self.host = host
        self.port = port
        self.timeout = timeout

    def __enter__(self):
        self.conn = socket.create_connection((self.host, self.port), self.timeout)
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.conn.close()

    def send_cmd(self, command: str) -> None:
        """Send a new CAPI command to the device.
        If previous replies from the server were available, they will be discarded.

        Parameters
        ----------
        command : str
            The command to send. If it does not end with a newline, a new line will be appended.
        """
        if command[-1] != "\n":
            command += "\n"
        self.conn.send(command.encode("utf-8"))

    def get_reply(self, verbose: bool = False, timeout: Union[int, None] = 120) -> Dict[str, str]:
        """Wait until the server replies with a `CAPIReply` message other than `CAPIReply.RUNNING`.

        The replies from the server will be printed as they are received.
        Note that this method only returns once a `CAPIReply.COMPLETE`, `CAPIReply.INVALID`,
        or `CAPIReply.ERROR` message has been received from the server.

        Parameters
        ----------
        verbose : bool
            If True, print out the valid replies (RUNNING and COMPLETE) as they arrive.
        timeout : int
            The maximum amount of time (in seconds) to wait for a reply. Defaults to 120.
            Note that this method is not guaranteed to return exactly after `timeout` seconds. It
            will abort after the next read call returns if the timeout is reached, which means that
            it can take more than `timeout` to execute depending on what data is received.
            A value of None can be used to wait until a CAPIReply other than RUNNING is received.
            Regardless of the value of `timeout`, at least one read is always done.

        Returns
        -------
        Dict[str, str]
            A mapping of parameter -> value. The CAPI COMPLETE message is followed by
            parameter,value pairs. These are converted to a dict and returned. If the COMPLETE
            message has no parameters, an empty dict is returned.

        Raises
        ------
        ValueError
            If the reply was something other than `CAPIReply.RUNNING`, or `CAPIReply.COMPLETE`, or
            `timeout` was reached.
        """
        data = bytearray()
        start_time = time.time()
        timed_out = False
        while not timed_out:
            # resetting data to the next line:
            data = data[data.find(b"\n") + 1:]
            while not timed_out and b"\n" not in data:
                # reading until there is a newline
                data.extend(self.conn.recv(256))
                timed_out = time.time() > start_time + timeout if timeout else False
            replies = data.decode("utf-8").split("\n")
            for r in replies:
                if not r:
                    pass  # server replied with an empty line
                elif CAPIReply.RUNNING.value in r:
                    if verbose:
                        print(r)
                elif CAPIReply.COMPLETE.value in r:
                    if verbose:
                        print(r)
                    reply_value_str = r[len(CAPIReply.COMPLETE.value) + 1:].strip()
                    reply_values = reply_value_str.split(',')
                    return {k: v for k, v in zip(reply_values[::2], reply_values[1::2])}
                elif CAPIReply.INVALID.value in r or CAPIReply.ERROR.value in r:
                    raise ValueError("Server replied with {}".format(r))
                else:
                    raise ValueError("Received an unknown reply from the server:\n {}".format(r))
        if timed_out:
            raise ValueError("Timed out when waiting for a reply from the server.")

    def cmd_reply(self, command: str, verbose: bool = False, timeout: int = 120) -> Dict[str, str]:
        """Open the connection, send a command and wait for the reply."""

        debug("Sending CAPI command: '{}'".format(command))
        with self:
            self.send_cmd(command)
            return self.get_reply(verbose, timeout)

    def start_wps_registration(self, band: str) -> None:
        """Call start_wps_registration on a given band.

        Parameters
        ----------
        band : str
            The band on which to start wps

        """
        command = "start_wps_registration,band,{},WpsConfigMethod,PBC".format(band)
        self.cmd_reply(command)

    def dev_get_parameter(self, parameter: str, **additional_parameters: str) -> str:
        """Call dev_get_parameter and return the parameter, or raise KeyError if it is missing.

        Parameters
        ----------
        parameter : str
            The parameter to query. It is the "parameter" argument of the dev_get_parameter command.

        additional_parameters : str
            Additional keyword arguments are passed as additional parameters to the
            dev_get_parameter command. This is needed for example to get the "macaddr" parameter,
            which needs additional "ssid" and "ruid" parameters in the command.

        Returns
        -------
        str
            The value of the requested parameter.
        """
        command = "dev_get_parameter,program,map,parameter,{}".format(parameter)
        if additional_parameters:
            command += ',' + ','.join([','.join(param) for param in additional_parameters.items()])
        return self.cmd_reply(command)[parameter]

    def dev_send_1905(self, dest: str, message_type: int, *tlvs: tlv) -> int:
        """Call dev_send_1905 to `dest` with CMDU type `message_type` and additional `tlvs`.

        Parameters
        ----------
        dest : str
            The AL-MAC address of the recipient, as a string.

        message_type : int
            The message type of the 1905.1 message to be sent, as an integer.

        tlvs : tlv
            Additional arguments are the TLVs in the 1905.1 message, as `tlv` objects.

        Returns
        -------
        The MID of the message, as an integer.
        """
        cmd = "DEV_SEND_1905,DestALid,{dest:s},MessageTypeValue,0x{message_type:04x}"\
            .format(**locals())
        if len(tlvs) > 1:
            formatted_tlvs = [tlv.format(tlv_num + 1) for (tlv_num, tlv) in enumerate(tlvs)]
            cmd += ',' + ','.join(formatted_tlvs)
        elif tlvs:
            cmd += ',' + tlvs[0].format()
        return int(self.cmd_reply(cmd)["mid"], base=0)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Simulated UCC")
    parser.add_argument("host", help="The device hostname or IP.", type=str)
    parser.add_argument("port", help="The listening port on the device.", type=int)
    parser.add_argument("command", help="The CAPI command to send.")
    args = parser.parse_args()
    socket.gethostbyname(args.host)

    UCCSocket(args.host, args.port).cmd_reply(args.command, True)
