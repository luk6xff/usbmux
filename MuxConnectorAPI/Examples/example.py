import sys
sys.path.insert(0, '/API/')

from API import mux_connector_api

example = mux_connector_api.MuxConnectorApi(port_name="COM5")
state = mux_connector_api.off_on

print("-----MUX INFO-----")
for line in example.show_inf():
    print(line)
print("------------------")

example.change_name("Poject_Ver_1_0")
print("NEW NAME " + example.get_name())

print("-----------------")
example.check_relay_state(0)
example.switch_relay(0,state.ON)
example.check_relay_state(1)
example.switch_relay(1,state.ON)
print("-----------------")

example.reboot()

print("Testing procedure complete, goodbye.")