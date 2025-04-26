/*
 * Copyright (C) 2025 Zayrullin Azat / zayruaz@gmail.com
 * SPDX-License-Identifier: CC0-1.0
 * See LICENSE file for details.
 */
#ifndef CP56TIME_H
#define CP56TIME_H
#include <QByteArray>
#include <QString>
class CP56Time{
  public:
    CP56Time();
    ///
    /// \brief CP56Time
    /// \param data - пакет данных содержащий метку времени
    /// \param s - смещение
    ///
    CP56Time(QByteArray data,int offset);
    quint16 GetMS();
    char GetMinute();
    char GetHour();
    char GetDay();
    char GetMonth();
    char GetYear();
    QString GetTimeString();
    bool IsValid();

    ///
    /// \brief GetTimestamp - возвращает метку времени с текущим временем (для команд)
    /// \return
    ///
    static QByteArray GetTimestamp();
private:
    char rawData[7];
};

#endif // CP56TIME_H
