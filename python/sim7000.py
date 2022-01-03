import serial
import time
import RPi.GPIO as GPIO
from dataclasses import dataclass
from typing import List, NoReturn, Union, Optional


@dataclass
class SIM7000:
    port: str = '/dev/serial0'
    baudrate: int = 115200
    bytesize: int = serial.EIGHTBITS
    timeout: int = 1
    stopbits: Union[int, float] = serial.STOPBITS_ONE
    parity: str = serial.PARITY_NONE
    xonxoff: bool = True
    power_key: int = 4

    post_init_on: bool = True

    __ser: Optional[serial.Serial] = None
    __last_message: Optional[str] = None
    __last_command: Optional[str] = None
    __last_info: Optional[str] = None

    def __post_init__(self) -> NoReturn:
        if(self.post_init_on):
            self._connect_to_serial()
            if(self._is_connected()):
                self.__set_gpio()
                self.__sim_on()

    def __del__(self) -> NoReturn:
        self.__sim_off()
        self._disconnet_serial()
        self.__clean_gpio()

    def _connect_to_serial(self) -> NoReturn:
        self.__ser = serial.Serial(port=self.port,
                                   baudrate=self.baudrate,
                                   bytesize=self.bytesize,
                                   timeout=self.timeout,
                                   stopbits=self.stopbits,
                                   parity=self.parity,
                                   xonxoff=self.xonxoff)

    def _disconnet_serial(self) -> NoReturn:
        self.__ser.close()

    def _is_connected(self) -> bool:
        return self.__ser.is_open

    def __set_gpio(self) -> NoReturn:
        GPIO.setmode(GPIO.BCM)
        GPIO.setwarnings(False)
        GPIO.setup(self.power_key, GPIO.OUT)

    def __clean_gpio(self) -> NoReturn:
        GPIO.cleanup()

    def __sim_on(self) -> NoReturn:
        GPIO.output(self.power_key, GPIO.HIGH)
        time.sleep(1)
        GPIO.output(self.power_key, GPIO.LOW)
        time.sleep(1)

        num = 0
        while(num < 10 and self.__last_info != 'OK'):
            num += 1
            self.send_message()
            print(
                f'\nTrying to turn on SIM7000 for {num} time/s. Last info: {self.__last_info}!')

    def __sim_off(self) -> NoReturn:
        GPIO.output(self.power_key, GPIO.HIGH)
        time.sleep(1)
        GPIO.output(self.power_key, GPIO.LOW)
        time.sleep(1)

    @property
    def get_last_message(self) -> Optional[str]:
        return self.__last_message

    @property
    def get_last_command(self) -> Optional[str]:
        return self.__last_command

    @property
    def get_last_info(self) -> Optional[str]:
        return self.__last_info

    def send_message(self, command: str = 'at', buff_size: int = 128) -> str:
        self.__ser.write(bytes(command+'\r', 'utf-8'))
        self.__last_message = self.__ser.read(buff_size).decode('utf-8')
        self.__last_command = self.__last_message.replace(
            '\r\n', '').split('\r')[0]
        self.__last_info = self.__last_message.replace(
            '\r\n', '').split('\r')[1]

        return self.__last_message

    def send_list_messages(self, list_commands: List[str] = ['at'], buff_size: int = 128) -> List[str]:
        list_messages = []

        for cmd in list_commands:
            self.__ser.write(bytes(cmd+'\r', 'utf-8'))
            self.__last_message = self.__ser.read(buff_size).decode('utf-8')
            list_messages.append(self.__last_message)
        self.__last_command = self.__last_message.replace(
            '\r\n', '').split('\r')[0]
        self.__last_info = self.__last_message.replace(
            '\r\n', '').split('\r')[1]

        return list_messages
