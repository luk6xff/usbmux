import concurrent.futures
import re

import serial.tools.list_ports
import queue
from time import sleep
from typing import Tuple, List


def check_if_mux(port_name: str, found_mux_queue: queue.Queue):
    """
    Checks if MUX is connected to port with given port_name.
    If it is - puts result in found_mux_queue in format: (port_name, mux_name)
    """
    with serial.Serial(
            port_name,
            baudrate=115200,
            timeout=1,
            stopbits=serial.STOPBITS_ONE,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE
    ) as ser:
        try:
            ser.reset_input_buffer()
            ser.reset_output_buffer()
            ser.write(b'inf\n')
            sleep(1)
            # read all input buffer
            data = ser.read(ser.in_waiting)
            # decode bytes data into string and split lines
            lines = data.decode('UTF-8').split('\r\n')

            for i, line in enumerate(lines):
                starting_line = 'Received Command: inf'
                # first line should start with proper line
                # there could be artifacts like "inf", so I check with regex:
                if i == 0 and (len(line) < len(starting_line) or
                               len(re.findall(starting_line, line)) != 1):
                    break
                else:
                    text_elements = line.split()

                    # we search for line "Name: [mux_name]
                    if len(text_elements) >= 2 and text_elements[0] == 'Name:':
                        # text_element[1] should contain mux name
                        result = (port_name, text_elements[1])
                        found_mux_queue.put(result)
                        break
        except Exception as err:
            print(err, f"happened at port {port_name}")
            print()


def find_all_muxes() -> List[Tuple[str, str]]:
    """
    Finds all MUX-devices connected to PC and returns it in a list.
    Format of an element in list is: (port_name, mux_name)
    """
    ports = serial.tools.list_ports.comports()
    # list of tuples in format (port, mux_name)
    found_mux_queue: queue.Queue[Tuple[str, str]] = queue.Queue()

    # max_workers in ThreadPoolExecutor must be greater than 0.
    if len(ports) == 0:
        print("No usb-serial devices was found!")
    else:
        with concurrent.futures.ThreadPoolExecutor(max_workers=len(ports)) as executor:
            futures = map(lambda port: executor.submit(
                check_if_mux, port[0], found_mux_queue), ports)
            for future in concurrent.futures.as_completed(futures):
                future.result()
    return list(found_mux_queue.queue)


def find_mux_with_name(target_mux_name: str):
    mux_list = find_all_muxes()

    for port_name, mux_name in mux_list:
        if mux_name == target_mux_name:
            return port_name
    return None
