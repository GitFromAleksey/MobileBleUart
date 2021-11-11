# Введение
В этом проекте представлен исходный код тестового приложения tion-remote, используемого в отделе разработок для тестирования BLE-протокола разрабатываемых устройств, а так же для работы с уже разработанными устройствами через BLE. Ниже будет представлена инструкция для разработчиков описывающая процесс добавления поддержки новых устройств. Инструкция будет написана на примере реализации поддержки устройств IQ.

# Подготовка

Для начала необходимо создать все базовые классы и формы. В корневом каталоге создаём папку с именем устройства "IQ". После этого в Qt создаем форму основного окна устройства (Файл->Создать Файл или проект...->Qt->Класс формы Qt Designer). Выбираем шаблон Widget, далее называем класс iq_mainwindow. В заголовочном файле iq_mainwindow.h подключаем файл lib/tiondevicebaselogic.h и меняем базовый класс с QWidget на TionDeviceBaseLogic. В итоге файл будет выглядеть следующим образом:
```c++
#ifndef IQ_MAINWINDOW_H
#define IQ_MAINWINDOW_H

#include <QWidget>
#include "lib/tiondevicebaselogic.h"

namespace Ui {
class iq_mainwindow;
}

class iq_mainwindow : public TionDeviceBaseLogic
{
    Q_OBJECT

public:
    explicit iq_mainwindow(QWidget *parent = nullptr);
    ~iq_mainwindow();

private:
    Ui::iq_mainwindow *ui;
};

#endif // IQ_MAINWINDOW_H
```

Далее необходимо создать класс обработчика протокола BLE. Создаём новый класс C++ (Файл->Создать Файл или проект...->С++->Класс С++), называем класс IQ_Rc, имя файлов iq_protocol.c++ и iq_protocol.h соответственно, базовым классом указываем TionDeviceBaseLogic:

```c++
#ifndef IQ_RC_H
#define IQ_RC_H

#include <lib/mabaseprotocol.h>

class IQ_Rc : public MABaseProtocol
{
private:

public:
    IQ_Rc();
};

#endif // IQ_RC_H

```

# Реализация протокола устройства

В этом разделе будет показан пример реализации протокола устройства IQ на уровне команд. Выполняется это в следующей последовательности:
1. Реализуются виртуальные функции, необходимые для работы протокола;
2. Создаются необходимые типы данных для хранения статуса устройства и прочей информации;
3. Создаются необходимые public функции для доступа к этим данным извне (сеттеры и геттеры);
4. Создаются private функции для обработки каждой команды протокола;
5. Реализуется обработчик фреймов, который в зависимости от пришедшей команды вызывает необходимый обработчик;

Рассмотрим реализацию на примере команды 0x3230 (принять параметры устройства) протокола устройств IQ. В классе IQ_Rc создаём private перечисление для хранения кодов комманд и добавляем комманду:

```c++
private:
    enum{
        BLE_FRAME_TYPE_GET_MODE_RESP = 0x3231
    };
```

Далее необходимо создать необходимые типы данных и переменные для хранения данных, передаваемых этой командой. Поскольку все байты по BLE передаются друг за другом пакет нужно упаковать директивой #pragma pack:

```c++
    typedef struct
    {
        uint16_t  PowerState            :1;
        uint16_t  SndIndicationState    :1;
        uint16_t  LEDIndicationState    :2;
        uint16_t  ChildLock             :1;
        uint16_t  reserved1             :2;
        uint16_t  LastComSource         :1;
        uint16_t  FilterWornout         :1;
        uint16_t  MagicAir              :1;
        uint16_t  MA_Auto               :1;
        uint16_t  ActiveTimer           :1;
        uint16_t  ADLayerExpire         :1;
        uint16_t  Reserved              :3;
    }IQ_Sys_Flags_t;

    typedef struct
    {
      int32_t   HardwareCounter;
      int32_t   FanCounter;
      int32_t   FilterCounter;
      int32_t   AntibacterialLayerCounter;
    } IQ_sys_counters_t;
    
#pragma pack(push, 1)
    typedef struct _tagIQ_RC_RF_Response_TypeDef
    {
        uint32_t LastCmdId;

        IQ_Sys_Flags_t      Flags;
        int8_t              SpeedLimit;
        int8_t              FanSpeed;
        int8_t              SleepTimerValue;
        int16_t             PM25Val;
        int16_t             VOCVal;
        IQ_sys_counters_t   Counters;
        int16_t             FilterCCM;
        uint32_t            Errors;
    }IQ_RC_RF_Response_TypeDef;
#pragma pack(pop)

IQ_RC_RF_Response_t CurrentParams;
```

Переменная CurrentParams хранит текущие параметры устройства, полученные по BLE.

При получении ответа от устройства приложение разбирает фрейм и передает тело команды в функцию void frameRecieveCallback(ble_frame_t frame), поэтому для начала необходимо реализовать эту функцию. 

iq_protocol.h
```c++
private:
 virtual void frameRecieveCallback(ble_frame_t frame) override;
```

iq_protocol.cpp
```c++
void IQ_Rc::frameRecieveCallback(ble_frame_t frame)
{
    switch(frame.type)
    {
        case BLE_FRAME_TYPE_GET_MODE_RESP:

        break;
    }
}
```

Далее необходимо реализовать функцию обработчик команды.

iq_protocol.h
```c++
private:
IQ_RC_RF_Response_t iq_rem_get_params_from_frame(ble_frame_t frame);
```

iq_protocol.cpp
```c++
IQ_Rc::IQ_RC_RF_Response_t
IQ_Rc::iq_rem_get_params_from_frame (ble_frame_t frame)
{
    IQ_RC_RF_Response_t ret;

    memcpy(&ret,frame.data, sizeof(IQ_RC_RF_Response_t));
    
    return ret;
}
```

После этого добавляем функцию в обработчик команд:

iq_protocol.cpp
```c++
void IQ_Rc::frameRecieveCallback(ble_frame_t frame)
{
    switch(frame.type)
    {
        case BLE_FRAME_TYPE_GET_MODE_RESP:
		CurrentParams = iq_rem_get_params_from_frame (frame);
        break;
    }
}
```

Таким образом обрабатываются все команды ответы.

# Получение значений из внешних классов

Для каждого хранимого параметра необходимо создать функции-геттеры чтобы получать их значения из других классов. Рассмотрим на примере флага режима работы устройства:

iq_protocol.h
```c++
public:
    bool powerState() const;
```

iq_protocol.cpp
```c++
bool IQ_Rc::powerState() const
{
    return CurrentParams.Flags.PowerState;
}
``` 

Таким образом реализуются get-функции для каждого параметра в отдельности.

# Отправка запросов

Отправка параметров на устройство производится при изменении какого-либо параметра через пользовательский интерфейс. Поэтому для начала необходимо реализовать базовую логику и пользовательский интерфейс устройства в приложении.

### Реализация основной логики

Подключаем нужные библиотеки:

iq_mainwindow.h
```c++
#include "iq_protocol.h"
#include "lib/maupdater.h"

#include <QBoxLayout>
```

Для начала в классе iq_mainwindow необходимо реализовать виртуальные функции базового класса и нужные переменные:

iq_mainwindow.h
```c++
virtual void updateDeviceParams(const QLowEnergyCharacteristic &c,
                                    const QByteArray &value) override ;
virtual IQ_Rc *getMaRc() override ;
virtual void sendRequest() override;

private:
    void setUiState(bool st);
    IQ_Rc BR4S_Rc;
    MAUpdater maUpd;
    QVBoxLayout* parent_layout;
```

iq_mainwindow.cpp
```c++

```