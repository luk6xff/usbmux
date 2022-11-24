
# Mux Connector API

Provides custom methodes for usbmux ([original repo](https://github.com/luk6xff/usbmux/tree/power_relays))





## Dependencies
- Python >= 3.9
- pyserial lib
## Important

Mux Connector Api uses Mux Finder to discover and automaticly
hook itself to Mux (you can obviously direct it to desired Mux in configuration if you dont want this feature)- Those two files by default need to be in the same
directory. If you want to want to change it, you need to change path in 7th line of mux_connector_api.py:

```sys.path.insert(0, './') ``` 

It points to a directory that contains mux_finder.api
## Quick example (check /Examples for better showcase)

```javascript
import sys
sys.path.insert(0, '/api_directory/')

from API import mux_connector_api

example = mux_connector_api.MuxConnectorApi()
print(example.<METHODE>)

```




## Methodes


- ```reboot()``` Reboots MUX
- ```check_relay_state(relay_id: int)```Checks state of relay
- ```show_inf()``` Returns info message
- ```change_name(mux_name: str)``` Changes MUX name
- ```switch_relay(relay_id: int, relay_state: off_on)``` Switches relay state(on/off)
- ```get_name()``` Returns name of MUX

## Authors

- [@Jabrocki](https://www.github.com/Jabrocki)
- [@Rid1cc](https://www.github.com/Rid1cc)
