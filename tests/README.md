This directory contains the end-to-end tests of prplMesh

## API for writing tests

Several helper classes are available that abstract the entities involved in the tests.
The test environment prepares these before the tests are started.

- `env.controller`, `env.agents[0]` and `env.agents[1]` are `ALEntity` objects with the following attributes:
    - `mac`: the AL-MAC address of the entity, to be used e.g. when sending a CMDU to it.
    - `radios`: on agents only, list of radios on this agent (see below).
    - `command(...)`: run command with arguments on the device, return its output as bytes.
    - `prplmesh_command(...)`: run the prplmesh command (e.g. `beerocks_cli`) with arguments on the device, return its output as bytes.
      This is essentially the same as `command` but it prepends the installation directory of prplmesh.
    - `wait_for_log(regex, start_line, timeout)`: wait for maximum `timeout` seconds for `regex` to appear in the log of the device on a line after `start_line`.
      This looks in the `beerocks_controller.log` or `beerocks_agent.log` file.
      Don't use this function directly, use `self.wait_for_log` or `self.check_log` instead (see below).
    - `cmd_reply(command)`: Send CAPI `command` string to the entity, return its reply as a dictionary.
      If the reply is INVALID or ERROR, an exception is raised.
    - `dev_get_parameter(parameter, **additional_parameters)`: Send the DEV_GET_PARAMETER CAPI command to get `parameter`.
      `additional_parameters` are required for some parameters.
      Returns the value of that parameter as a string.
    - `dev_send_1905(dest, message_type, *tlvs)`: send the DEV_SEND_1905 CAPI command to triger the sending of a CMDU of type `message_type` to the `dest` MAC address and returns the MID.
      `tlvs` are objects of type `tlv` which specify the TLVs to include.
      `tlv(type, length, value)` constructs a TLV.
      `type` and `length` are integers, `value` is a string with hex values (including 0x) or MAC addresses and optional `{}` delimiters.
- `Radio` objects are accessed through the agents and have the following attributes:
    - `mac`:  The radio UID of this radio.
    - `vaps`: list of VAPs configured on this radio, see below.
    - `get_current_channel`: read the current channel from the radio.
       Returns a tuple (channel, bandwidth, center_channel).
       Note that 80+80 bandwidth is not supported: bandwidth is reported as an integer, and 80+80 requires two center channels.
    - `wait_for_log(regex, start_line, timeout)`: wait for maximum `timeout` seconds for `regex` to appear in the log of the device on a line after `start_line`.
      This looks in the `beerocks_agent_wlanX.log` file, where wlanX corresponds to the radio.
      Don't use this function directly, use `self.wait_for_log` or `self.check_log` instead (see below).
- `VirtualAP` objects are accessed through radios and have the following attributes:
    - `bssid`: The BSSID of this VAP.
    - `associate(sta)`: associate a Station (see below) with this VAP.
      Only possible if an SSID is configured on this VAP.
    - `disassociate(sta)`: disassociate a Station (see below) from this VAP.
- `Station` objects are created with `Station.create()`.
  Every time a test needs to use a station, a new one should be generated.
  This makes sure every test uses a unique station MAC address and different tests don't influence each other.
  The station's MAC address is accessible through the `mac` attribute.
- `env.wired_sniffer` is a capture of the 1905.1 communication between agents.
  This will be documented once https://github.com/prplfoundation/prplMesh/pull/1145 is merged.
- The base class of the test has a number of helpers that can be accessed through `self`:
    - `self.fail(msg)`: Report a test failure with `msg`.
    - `self.wait_for_log(entity_or_radio, regex, start_line, timeout)`: call wait_for_log on the entity or radio.
      If it fails, mark it as a failure (cfr. `self.fail()`).
    - `self.check_log(entity_or_radio, regex, start_line)`: check if `regex` can be found in the log of `entity_or_radio` after `start_line`.
      If it fails, mark it as a failure (cfr. `self.fail()`).
      `start_line` is optional.

## Description of tests

TODO: translate this presentation into markdown format (.md)
[Setup](https://docs.google.com/presentation/d/1F8TLcvl_mHGKWm-gqeGGB0qblWLSgzG97RpX8C3SSYM/edit?usp=sharing)

### ap_config_renew

- send 0x0006 + 0x0001 + 2x 0x0001 (1905 Topology Notification message) to Gateway
- check log wlan0 received credentials SSID 1 bss_type 2
- check log wlan0 received credentials SSID 2 bss_type 1
- check log wlan2 tear down radio

### ap_config_bss_tear_down
- send 0x0006 (1905 Topology Notification message) to repeater 1 as Gateway
- sleep 3
- check log repeater 1 wlan0 received credentials SSID 3 bss_type 2
- check log repeater 1 wlan2 tear down radio
- send 0x0006 (1905 Topology Notification message) to repeater 1 as Gateway
- check log repeater 1 wlan2 tear down radio

### channel_selection
- send 0x8004 (Channel Preference Query message) to repeater 1 as Gateway
- check log repeater 1 wlan0 message CHANNEL_PREFERENCE_QUERY_MESSAGE received
- check log repeater 1 wlan2 message CHANNEL_PREFERENCE_QUERY_MESSAGE received
- send 0x8006 (Channel Selection Request message) to repeater 1 as Gateway
- check log repeater 1 wlan0 message CHANNEL_SELECTION_REQUEST_MESSAGE received
- check log repeater 1 wlan2 message CHANNEL_SELECTION_REQUEST_MESSAGE received

### ap_capability_query
- send 0x8001 (AP Capability Query message) to repeater 1 as Gateway
- check log repeater 1 agent message AP_CAPABILITY_QUERY_MESSAGE received
- check log controller AP_CAPABILITY_REPORT_MESSAGE received

### combined_infra_metrics
- send 0x800B (AP Metrics Query Message) + 0x0007 (AP- Autoconfiguration Search message) to repeater 1 as Gateway
- check log repeater 1 wlan0 AP_METRICS_QUERY_MESSAGE message received
- send 0x800C (AP Metrics Response message) as Repeater 1 to Gateway
- check log controller AP_METRICS_RESPONSE_MESSAGE received
- send 0x800B (AP Metrics Query Message) + 0x0007 (AP- Autoconfiguration Search message) to repeater 2 as Gateway
- check log repeater 2 wlan2 message AP_METRICS_QUERY_MESSAGE received
- send 0x800C (AP Metrics Response message) as Repeater 2 to Gateway
- check log controller AP_METRICS_RESPONSE_MESSAGE received
- send 0x0005 to repeater 1 as Gateway
- check logs repeater 1 wlan0 Received LINK_METRIC_QUERY_MESSAGE
- send 0x0006 to Gateway as repeater 1
- check logs controller Received LINK_METRIC_RESPONSE_MESSAGE
- send 0x8013 (Combined Infrastructure Metrics message) to repeater 1 as Gateway
- check logs repeater 1 agent Received COMBINED_INFRASTRUCTURE_METRICS
- check logs repeater 1 agent Received TLV_TRANSMITTER_LINK_METRIC
- check logs repeater 1 agent Received TLV_RECEIVER_LINK_METRIC

### ap_metrics_response
- send 0x800B (AP Metrics Query Message) + 0x0007 (AP- Autoconfiguration Search message) to repeater 1 as Gateway
- check log repeater 1 wlan0 AP_METRICS_QUERY_MESSAGE message received
- send 0x800C (AP Metrics Response message) as Repeater 1 to Gateway
- check log controller AP_METRICS_RESPONSE_MESSAGE received
- send 0x800B (AP Metrics Query Message) + 0x0007 (AP- Autoconfiguration Search message) to repeater 2 as Gateway
- check log repeater 2 wlan2 message AP_METRICS_QUERY_MESSAGE received
- send 0x800C (AP Metrics Response message) as Repeater 2 to Gateway
- check log controller AP_METRICS_RESPONSE_MESSAGE received

### link_metrics
- send 0x0005 to repeater 1 as Gateway
- check logs repeater 1 wlan0 Received LINK_METRIC_QUERY_MESSAGE
- send 0x0006 to Gateway as repeater 1
- check logs controller Received LINK_METRIC_RESPONSE_MESSAGE

### client_capability_query
- send 0x8009 (Client Capability Query message) as Gateway to repeater 1 with `{sta_mac1}`
- check logs repeater 1 agent CLIENT_CAPABILITY_QUERY_MESSAGE
- check logs controller Received CLIENT_CAPABILITY_REPORT_MESSAGE
- check logs controller Result Code= FAILURE, client MAC = `{sta_mac1}`
- send BWL event repeater 1 wlan0 EVENT AP-STA-CONNECTED `{sta_mac2}`
- send 0x8009 (Client Capability Query message) as Gateway to `{sta_mac2}`
- check logs controller Received CLIENT_CAPABILITY_REPORT_MESSAGE
- check logs controller Result Code= SUCCESS, client MAC = `{sta_mac2}`

### client_association_dummy
- send BWL event repeater 1 wlan0 EVENT AP-STA-CONNECTED
- beerocks CLI client_allow repeater 1 wlan2
- check logs repeater 1 wlan2 Got client allow request
- beerocks CLI client_disallow repeater 1 wlan0
- check logs repeater 1 wlan0 Got client disallow request

### client_steering_mandate
- send 0x0002 (Topology Query message) to repeater 1 as Gateway
- check logs repeater 1 agent TOPOLOGY_QUERY_MESSAGE
- send 0x0002 (Topology Query message) to repeater 2 as Gateway
- check logs repeater 2 agent TOPOLOGY_QUERY_MESSAGE
- send 0x8014 (Client Steering Request message) to repeater 1 as Gateway
- check logs repeater 1 wlan0 Got steer request
- check logs controller CLIENT_STEERING_BTM_REPORT_MESSAGE
- check logs controller BTM_REPORT from source
- check logs repeater 1 wlan0 ACK_MESSAGE
- send 0x8014 (Client Steering Request message) to repeater 1 as Gateway
- check logs repeater 1 wlan0 CLIENT_STEERING_REQUEST_MESSAGE
- check logs controller ACK_MESSAGE
- check logs controller STEERING_COMPLETED_MESSAGE
- check logs repeater 1 wlan0 ACK_MESSAGE

### client_steering_dummy
- send BWL event to repeater 1 wlan0 EVENT AP-STA-CONNECTED
- beerock CLI steer_client repeater1_wlan2
- sleep(1)
- check logs repeater 1 wlan2 Got client allow request
- check logs repeater 1 wlan0 Got client disallow request
- check logs repeater 2 wlan0 Got client disallow request
- check logs repeater 2 wlan2 Got client disallow request
- check logs repeater 1 wlan0 Got steer request
- check logs controller CLIENT_STEERING_BTM_REPORT_MESSAGE
- check logs repeater 1 wlan0 ACK_MESSAGE
- send BWL event repeater 1 wlan0 EVENT AP-STA-DISCONNECTED
- sleep 1
- send BWL event repeater 1 wlan2 EVENT AP-STA-CONNECTED
- check logs controller steering successful for sta
- check logs controller disconnected due to steering request

### client_steering_policy
- send 0x8003 (Multi-AP Policy Config Request message) to repeater 1 as Gateway
- sleep 1
- check logs repeater 1 wlan0 "MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE"
- sleep 1
- check logs controller ACK_MESSAGE, mid

### client_association
- send 0x0002 (1905 Topology Query message) to repeater 1 as Gateway
- check logs repeater 1 agent TOPOLOGY_QUERY_MESSAGE
- send 0x8016 (Client Association Control Request message) to repeater1 wlan0 as Gateway
- check logs repeater 1 wlan0 CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE
- check logs repeater 1 wlan2 CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE
- check logs controller ACK_MESSAGE

### topology
- waits at least 60 sec. to topology gets discovered
- send 0x0002 (Topology Query message) to repeater 1 as Gateway
- check topology contains Controller MAC appears on Agent Neighbor List

### nbapi_access_point
- Add BSSes with following SSIDs, bands and MultiAPMode enabled:
- SSID: "Test-all-bands"; bands enabled: 24G, 5GH, 5GL, 6G; MultiApMode: "Fronthaul"
        ModeEnabled: "WPA2-Personal"; KeyPassphrase: "key_passphrease_value";
- SSID: "Test-5GH-24G"; bands enabled: 24G, 5GH; MultiApMode: "Fronthaul"
- SSID: "Test-5GL"; bands enabled: 5GL; MultiApMode: "Fronthaul"
- SSID: "Test-6G"; bands enabled: 6G; MultiApMode: "Fronthaul"
- SSID: "Test-Fronthaul+Backhaul"; bands enabled: 24G, 5GH; MultiApMode: "Fronthaul", "Backhaul"
- call NBAPI AccessPointCommit method
- sleep 5s
- check logs repeaters 1, 2 radios 0, 1:
    - 'ssid: Test-all-bands auth_type: WPA2-PSK  encr_type: AES
    - network_key: key_passphrease_value fronthaul: true backhaul: false'
- check logs repeaters 1, 2 radios 0, 1:
    - 'ssid: Test-5GH-24G auth_type: NONE  encr_type: NONE
    - network_key:  fronthaul: true backhaul: false'
- check logs repeaters 1, 2 radio 1:
    - 'ssid: Test-5GL auth_type: NONE  encr_type: NONE
    - network_key:  fronthaul: true backhaul: false'
- check logs repeaters 1, 2 radios 0, 1:
    - 'ssid: F+B auth_type: NONE  encr_type: NONE
    - network_key:  fronthaul: true backhaul: true'
- check log wlan1 received credentials SSID Test-all-bands bss_type 2
- check log wlan1 received credentials SSID Test-5GH-24G bss_type 2
- check on UBUS, reapeters 1, 2 radio 1, BSS with SSID "5GL" is enabled
- check on UBUS, reapeters 1, 2 radio 0, BSS with SSID "5GL" is disabled
- check on UBUS, reapeters 1, 2 have BSSes with SSIDs: "Test-all-bands", "Test-5GH-24G",
"Test-Fronthaul+Backhaul" on each radio enabled
- check on UBUS BSS with SSID "Test-6G" does not appear on any radio on any reapeter

## Mapping of certification tests

Flow tests between parenthesis don't fully cover the certification test.
In addition, many certification tests include a configuration phase which is not covered in the flow test.

| Certification test | test flow |
| ------------------ | --------- |
| 4.2.1 Ethernet Onboarding and Initialization | step 1-9: -<br>step 10: (channel_selection)<br>step 11-12: (topology) |
| 4.2.2 Wi-Fi Onboarding and Initialization 1 |
| 4.2.3 Wi-Fi Onboarding and Initialization 2 |
| 4.3.1 Supported Services Discovery |
| 4.3.2 Client association and disassociation | step 1-2: -<br>step 3: (client_association_dummy)<br>step 4: (client_association)<br>step 5-10: - |
| 4.4.1 Initial AP configuration | (client_association_dummy)
| 4.4.2 AP configuration renew | (ap_config_renew) (ap_config_bss_tear_down)
| 4.4.3 Two AP configuration BSS Tear Down | (ap_config_bss_tear_down)
| 4.5.1 Channel Preference Query | (channel_selection)
| 4.5.2 Channel Selection Request Message |
| 4.5.3 DFS Status update report |
| 4.6.1 AP capability report | (ap_capability_query)
| 4.6.2 Client capability reporting | (client_capability_query)
| 4.6.3 Client capability query transmit |
| 4.7.1 Backhaul Link Metric Query test - All Neighbors |
| 4.7.2 Backhaul Wi-Fi Link Metric Query test - Specific Neighbor |
| 4.7.3 Backhaul Ethernet Link Metric Query test - Specific Neighbor |
| 4.7.4 Per-AP Link Metrics Query Message | (combined_infra_metrics)
| 4.7.5 Per-AP Metrics Response Message Controlled by AP Metrics Reporting Interval |
| 4.7.6 Per-AP Metrics Response Controlled by AP Metrics Channel Utilization Reporting Threshold |
| 4.7.7 Associated STA Link Metrics and Counters |
| 4.7.8 Un-Associated STA RSSI Measurements |
| 4.7.9 Beacon Report Query and Response |
| 4.8.1 BTM-based Client Steering Mandate | (client_steering_mandate) (client_steering_dummy)
| 4.8.2 Legacy Client Steering Mandate | (client_steering_policy)
| 4.8.3 Client Steering Opportunity |
| 4.8.4 RCPI Policy-based Steering |
| 4.8.5 Client Association Control |
| 4.9.1 Backhaul STA Steering Request |
| 4.10.1 Data Passing test 1 (one-hop with connected to the CTT Agent) |
| 4.10.2 Data Passing test 2 (two-hop with connected to an Agent) |
| 4.10.3 Data Passing test 3 (one-hop with CTT Agent connected to MAUT) |
| 4.10.4 Data Passing test 4 (two-hop with CTT Agent connected to MAUT) |
| 4.10.5 Data Passing test 5 (with MAUT between CTT Agent1 and CTT Agent 2) |
| 4.10.6 Data Passing test 6 (with a CTT STA connected to MAUT) |
| 4.11.1 Control Messaging Reliability |
| 4.12.1 Higher layer data payload over 1905 trigger | (higher_layer_data_payload_trigger)
| 4.12.2 Higher layer data payload over 1905 |
| 5.3.1 Supported Services Discovery |
| 5.4.1 Initial AP configuration | (client_association_dummy) (client_association)
| 5.4.2 AP configuration renew | ap_config_renew
| 5.4.3 AP configuration BSS Tear Down | ap_config_bss_tear_down
| 5.5.1 Channel Preference Query and Channel Selection Request Message | (channel_selection)
| 5.6.1 AP capability query | (ap_capability_query)
| 5.6.2 Client capability query | (client_capability_query)
| 5.7.1 Combined Infrastructure Metrics | (combined_infra_metrics)
| 5.8.1 Client Steering for Steering Mandate and Steering Opportunity | (client_steering_mandate) (client_steering_dummy)
| 5.8.2 Setting Client Steering Policy | (client_steering_policy)
| 5.8.3 Client Association Control Message |
| 5.10.1 Higher layer data payload over 1905 trigger | (higher_layer_data_payload_trigger)
| 5.10.2 Higher layer data payload over 1905 |
