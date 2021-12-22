import sys
import time
import serial
import serial.tools.list_ports
import threading
import multiprocessing
from Loggers import cDebugLogger

print('comport.py run as:', __name__)
##------------------------------------------------------------------------------
class SerialPort():

    def __init__(self):
        self.using_port = None
        self.aval_ports = []
        self.LoggerCallback = None
        self.write_queue = multiprocessing.Queue()
        self.read_queue = multiprocessing.Queue()

    def SetLoggerCallback(self, LoggerCallback):
        ''' Установка calback функции логгера '''
        self.Logger('SetLoggerCallback()')
        self.LoggerCallback = LoggerCallback

    def GetFuncName(self):
        print(f'Print from {sys._getframe().f_code.co_name}')

    def Logger(self, info):
##        s = self.__class__.__name__ + ': ' + info
        s = info.replace('\n','')
        if self.LoggerCallback != None:
            print('LoggerCallback:' + s)
            self.LoggerCallback(s)
        else:
            print(s)

    def SetPort(self, port_name):
        ''' Установка порта '''
        f_name = f'{sys._getframe().f_code.co_name}():' # TODO это определение имени функции можно вставить в другие функции
        self.Logger(f_name + port_name)
        self.using_port = serial.Serial()
        self.using_port.setPort(port_name)
        self.using_port.baudrate = 115200
        self.using_port.writeTimeout = 0.1
        self.using_port.readTimeout = 0.1
        self.using_port.timeout = 0.1
##        self.using_port = serial.Serial(
##                port = port_name,
##                baudrate = 9600, #115200
##                parity = serial.PARITY_NONE,
##                stopbits = serial.STOPBITS_ONE,
##                bytesize = serial.EIGHTBITS,
##                timeout = 0)
##        self.Logger('BAUDRATES:' + str(self.using_port.BAUDRATES))
##        self.Logger('BYTESIZES:' + str(self.using_port.BYTESIZES))
##        self.Logger('PARITIES:' + str(self.using_port.PARITIES))
##        self.Logger('STOPBITS:' + str(self.using_port.STOPBITS))
        self.Logger(f_name + str(self.using_port))
        self.Logger(f_name + ' - exit')

    def ClosePort(self):
        ''' Закрытие порта '''
        self.Logger('ClosePort()')
        self.using_port.close()

    def OpenPort(self):
        ''' Открытие порта '''
        if self.using_port == None:
            self.Logger('OpenPort() pleas select port name')
            return
        self.Logger('OpenPort() ' + self.using_port.port)
        if self.using_port.isOpen():
            self.Logger('OpenPort() ' + self.using_port.port + ' is open')
            return
        try:
            self.Logger('OpenPort() Try OpenPort:' + self.using_port.port)
            self.using_port.open()
            self.Logger(str(self.using_port))
            th_read = threading.Thread(name = 'read thread', target = self.ReadFromPortThread)
            th_read.start()
            th_write = threading.Thread(name = 'write thread', target = self.WritePortThread)
            th_write.start()
        except(OSError, serial.SerialException):
            self.Logger('OpenPort() Error open port:' + str(OSError))

    def ReadFromPortThread(self):
        ''' Чтение из порта '''
        f_name = f'{sys._getframe().f_code.co_name}():' # название функции
        self.Logger(f_name + ' strart read from port')
        while self.using_port.isOpen():
            data = self.using_port.read()
            if data != b'':
                for b in data:
                    rx_data = bytearray(b'')
                    self.read_queue.put(b)
                    if b == 10 or self.read_queue.qsize() > 50: # 10 = '\n'
                        while self.read_queue.qsize() > 0:
                            rx_data.append(self.read_queue.get())
                        self.Logger('\nRX: ' + str(rx_data, 'cp1252'))

        self.Logger(f_name + ' stop read from port:' + self.using_port.port)
        self.Logger(f_name + ' exit')

    def WriteData(self, data_bytes):
        ''' Запись байт данных в очередь для отправки '''
        self.Logger('\nTX: ' + str(data_bytes))
        for d in data_bytes:
            self.write_queue.put(bytes([d]))

    def WritePortThread(self):
        ''' Функция потока отправки данных '''
        f_name = f'{sys._getframe().f_code.co_name}():'
        self.Logger(f_name + 'start')
        while self.using_port.isOpen():
            time.sleep(0.1)
            while self.write_queue.qsize() > 0:
                data = self.write_queue.get()
                self.using_port.write(data)
        self.Logger(f_name + 'stop')

    def ClosePort(self):
        ''' Закрывает порт '''
        f_name = f'{sys._getframe().f_code.co_name}():'
        self.Logger(f_name + str(self.using_port))
        self.using_port.close()
        self.Logger(f_name + self.using_port.port)

    def GetListAvaliablePorts(self):
        ''' Отдаёт список проверенных доступных портов '''
        self.Logger('GetListAvaliablePorts()')
        return self.aval_ports

    def GetUsingPort(self):
        ''' Отдаёт используемый в данный момент порт '''
        self.Logger('GetUsingPort()')
        return self.using_port

    def GetAllPorts(self):
        ''' Получение всех портов системы '''
        self.Logger('GetAllPorts(self) - Получение всех портов системы')
        self.UpdatePortsList()
        port_list = []
        for port_name, desc, hwid in sorted(self.port_list):
            port_list.append(port_name)
##            self.Logger(port_name)

        self.Logger('GetAllPorts() - exit')
        return port_list

    def UpdatePortsList(self):
        '''Обновляет список доступных в системе COM портов'''
        self.Logger('UpdatePortsList() - Обновляет список доступных в системе COM портов')
        self.port_list = serial.tools.list_ports.comports()
        for port in self.port_list:
            self.Logger('Port:' + str(port))
        self.Logger('UpdatePortsList() - exit')

    def CheckAllPortsThread(self):
        ''' Запуск потока проверки доступности портов '''
        self.Logger('CheckAllPortsThread() - Запуск потока проверки доступности портов')
        th = threading.Thread(target = self.CheckAllPorts)
        th.start()
        th.join()
        self.Logger('\nCheckAllPortsThread() - exit')

    def CheckAllPorts(self):
        '''Составляет список доступных портов'''
        self.Logger('CheckAllPorts() - Составляет список доступных портов')
        self.UpdatePortsList()
        if len(self.port_list) == 0:
            self.Logger('CheckAllPorts() - exit. Список портов пуст')
            return
        for port_name, desc, hwid in sorted(self.port_list):
            if self.CheckPort(port_name) == True:
                self.aval_ports.append(port_name)

        self.Logger('Avaliable ports:'+ str(self.aval_ports))
        self.Logger('CheckAllPorts() - exit')
        return self.aval_ports

    def CheckPort(self, port_name):
        '''Проверка порта на занятость'''
        self.Logger('CheckPort() - Проверка порта на занятость')
        try:
            self.Logger('CheckPort() - '+ port_name)
            s = serial.Serial(port_name, timeout = 0.1)
            s.close()
            self.Logger('CheckPort() - Avaliable')
            return True
        except(OSError, serial.SerialException):
            self.Logger('CheckPort() - Not Avaliable')
            return False

##------------------------------------------------------------------------------
def main():
    serial_port = SerialPort()

    l = cDebugLogger()
    l.LoggingSwitch(True)
    serial_port.SetLoggerCallback(l.Logging)

    print('\nПолучение всех COM портов системы:')
    serial_port.GetAllPorts()
##    serial_port.CheckAllPortsThread()
##    print('\n Список доступных портов: ' + str(serial_port.GetListAvaliablePorts()))
    port_name = input('\nInput port name:')
    serial_port.SetPort(port_name)
    serial_port.OpenPort()

    data_bytes = b'hello from python!\n'
    serial_port.WriteData(data_bytes)
##    time.sleep(5)

    while True:
        time.sleep(1)
##        serial_port.WriteData(data_bytes)
        input()

    serial_port.ClosePort()
    print('exit main')



if __name__ == '__main__':
    main()
