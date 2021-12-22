import os.path
import datetime
import json

## -----------------------------------------------------------------------------
class cLoggerBase:
    LOG_FOLDER = 'logs'
    
    def __init__(self):
        self.log_enable = False
        self.file_name_prefix = ''
        self.file_extension = 'log' # default file extension
##        print('logger:' + self.__class__.__name__ + '.__init__()')
    
    def LoggingSwitch(self, enable):
        self.log_enable = True

    def SetFileNamePrefix(self, prefix):
        self.file_name_prefix = prefix

    def SetFileExtension(self, file_extension):
        self.file_extension = file_extension

    def GetFileName(self):
        dt = datetime.datetime.now()
        f_name = self.file_name_prefix + '_'  + self.__class__.__name__ + '_' + dt.strftime("%d.%m.%Y_%H") + '.' + self.file_extension
        return f_name

    def GetTimeStamp(self):
        return datetime.datetime.now().strftime('[%d-%m-%YT%H:%M:%S.%f]')

    def LogToFile(self, text):
        if os.path.exists(cLoggerBase.LOG_FOLDER) == False:
            os.makedirs(cLoggerBase.LOG_FOLDER)
        log_file_name = cLoggerBase.LOG_FOLDER + '//' + self.GetFileName()
        log_file = open(log_file_name, 'a')
        log_file.write(text + '\r')
        log_file.close()

    def Logging(self, text):
        pass
## -----------------------------------------------------------------------------
class cDebugLogger(cLoggerBase):

    def __init__(self):
        super().__init__()

    def Logging(self, text):
        if self.log_enable == True:
            timestamp = self.GetTimeStamp()
            log_string = timestamp + ',[' + text + ']'
##            print(log_string)
            self.LogToFile(log_string)
## -----------------------------------------------------------------------------
class cJsonLogger(cLoggerBase):
    def __init__(self):
        super().__init__()
        self.SetFileExtension('json')

    def Logging(self, data_dict):
        if self.log_enable == False:
            return
        if type(data_dict) is not dict:
            print('Error:' + self.__class__.__name__, '.Logging(data_dict): data_dict is not dict')
            return
        timestamp = self.GetTimeStamp()
        data_dict['TIME_STUMP'] = timestamp
        log_string = json.dumps(data_dict, ensure_ascii = False)
        log_string = log_string.replace('\\','')
        self.LogToFile(log_string)
## -----------------------------------------------------------------------------

def main():
    dl = cDebugLogger()

    dl.LogToFile('test function LogToFile()')

    print('test function GetFileName():', dl.GetFileName())
    dl.Logging('self.log_enable = False')
    dl.LoggingSwitch(True)
    dl.Logging('test function Logging()')

    jl = cJsonLogger()
    jl.LoggingSwitch(True)
    jl.Logging('e')
    jl.Logging({'value1:':5, 'value2:':6})

## -----------------------------------------------------------------------------
if __name__ == '__main__':
    main()
