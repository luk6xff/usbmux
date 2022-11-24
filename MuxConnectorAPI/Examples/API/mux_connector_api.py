import sys
sys.path.insert(0, './')

from enum import Enum
from time import sleep

import serial.tools.list_ports
# imports mux_finder methods
from mux_finder_api import find_all_muxes, find_mux_with_name


class off_on(Enum):
    """Enum for off and on

    Args:
        Enum (str): enum for off and on
    """
    ON = '1'
    OFF = '0'


class MuxConnectorApi:

    
    
    def __init__(self, mux_name: str = None, port_name: str = None):
        """Constructor of class

        Args:
            mux_name (str): name of mux we are connecting with
            port_name (str): port name of port connected with mux
        """
        mux_list = find_all_muxes()

        self.port_name = port_name
        self.mux_name = mux_name
        
        if mux_name is None:
            for p_name, m_name in mux_list:
                if p_name == port_name:
                    mux_name = m_name
            if mux_name is not None:
                self.port_name = port_name
                self.mux_name = mux_name
            else:
                self.port_name = None
                self.mux_name = None
        elif port_name is None:
            self.port_name = find_mux_with_name(mux_name)
            self.mux_name = mux_name
        
        self.ser = None
    def __connect__(self):
        if self.port_name is not None:
            self.ser = serial.Serial(
                self.port_name,
                baudrate=115200,
                timeout=1,
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE
            )
        else:
            print('Could not create a mux handler')
    def __disconnect__(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None
        else:
            print('There is no connection')

    def check_relay_state(self, relay_id: int):
        """Checks state of relay

        Args:
            relay_id (int): ID of the relay

        Returns:
            state (off_on): enum of relay state: off_on.ON/off_on.OFF
        """
        self.__connect__()
        try:
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            state = f'st,{relay_id}\n'
            self.ser.write(bytes(state, encoding='utf-8'))
            sleep(1)
            data = self.ser.read(self.ser.in_waiting)

            lines = data.decode('UTF-8').split('\r\n')

            for line in lines:
                if line == f"PowerRelay[id:{relay_id}] state SET to: RELAY_ON":
                    print(f"PowerRelay[id:{relay_id}] state SET to: RELAY_ON")
                    self.__disconnect__()
                    return off_on.ON
            print(f"PowerRelay[id:{relay_id}] state SET to: RELAY_OFF")
            self.__disconnect__()
            return off_on.OFF
        except Exception as err:
            print(err, f"happened at port {self.port_name}")
            print()
            self.__disconnect__()
            return None
        

    def show_inf(self):
        """Returns info message

        Returns:
            lines (List[str]): returns info message
        """
        self.__connect__()
        try:
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            self.ser.write(b'inf\n')
            sleep(1)
            # read all input buffer
            data = self.ser.read(self.ser.in_waiting)
            # decode bytes data into string and split lines
            lines = data.decode('UTF-8').split('\r\n')
            self.__disconnect__()
            return lines

        except Exception as err:
            print(err, f"happened at port {self.port_name}")
            print()
            self.__disconnect__()
            return None

    def reboot(self):
        """Reboots MUX
        """
        self.__connect__()
        try:
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            self.ser.write(b'r\n')
            sleep(1)
            print("REBOOTED")
            self.__disconnect__()
        except Exception as err:
            print(err, f"happened at port {self.port_name}")
            print()
            self.__disconnect__()

    def change_name(self, mux_name: str):
        """Changes MUX name

        Args:
            mux_name (str): new name of MUX
        """
        self.__connect__()
        try:
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            test = f'n,{mux_name}\n'
            self.ser.write(bytes(test, encoding='utf-8'))
            sleep(1)
            print("RENAMED")
            self.__disconnect__()

        except Exception as err:
            print(err, f"happened at port {self.port_name}")
            print()
            self.__disconnect__()

    def switch_relay(self, relay_id: int, relay_state: off_on):
        """Switches relay state(on/off)

        Args:
            relay_id (int): id of relay
            relay_state (off_on): relay state in enum off_on
        """
        self.__connect__()
        try:
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            power = f'pwr,{relay_id},{relay_state.value}\n'
            self.ser.write(bytes(power, encoding='utf-8'))
            sleep(1)
            if relay_state == off_on.ON:
                print("Relay ON")
                self.__disconnect__()
            if relay_state == off_on.OFF:
                print("Relay OFF")
                self.__disconnect__()
        except Exception as err:
            print(err, f"happened at port {self.port_name}")
            print()
            self.__disconnect__()

    def get_name(self):
        """Returns name of MUX

        Returns:
            name (str): returns name of MUX
        """
        self.__connect__()
        try:
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()
            self.ser.write(b'inf\n')
            sleep(1)
            data = self.ser.read(self.ser.in_waiting)  # read all input buffer
            lines = data.decode('UTF-8').split('\r\n')
            for line in lines:
                if line[0:5] == "Name:":
                    self.__disconnect__()
                    return line[6:]
            self.__disconnect__()
            return None
        except Exception as err:
            print(err, f"happened at port {self.port_name}")
            print()
            self.__disconnect__()
            return None
