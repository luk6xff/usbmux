import sys
sys.path.insert(0, '/API/')

from API import mux_connector_api

right_mux = mux_connector_api.MuxConnectorApi(port_name="COM4")
left_mux = mux_connector_api.MuxConnectorApi(port_name="COM5")

state = mux_connector_api.off_on


left_mux.switch_relay(0,state.OFF)
left_mux.switch_relay(1,state.OFF)
right_mux.switch_relay(0,state.OFF)
right_mux.switch_relay(1,state.OFF)
print("Default state OFF has been set, proceeding to make noise...")

# Weird clock example
for x in range(5):
    print(left_mux.switch_relay(0,state.ON))
    print(right_mux.switch_relay(0,state.ON))
    print(left_mux.switch_relay(1,state.ON))
    print(right_mux.switch_relay(1,state.ON))
    print(left_mux.switch_relay(0,state.OFF))
    print(right_mux.switch_relay(0,state.OFF))
    print(left_mux.switch_relay(1,state.OFF))
    print(right_mux.switch_relay(1,state.OFF))