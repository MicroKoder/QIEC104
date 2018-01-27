#include <QString>
#include <QtTest>

#include "..\ciecsignal.h"
#include "..\cp56time.h"
#include "..\ctools.h"
class TestParserTest : public QObject
{
    Q_OBJECT

public:
    TestParserTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();

    void testCaseIOA_data();
    void testCaseIOA();

    void testCaseFloat_data();
    void testCaseFloat();
};

TestParserTest::TestParserTest()
{

}

void TestParserTest::testCase1_data()
{
    QTest::addColumn<QByteArray>("rawData");
    QTest::addColumn<int>("count");


    char data[] = {104,21, 0, 0, 0, 0, 30, char(129), 3, 0, 1, 0, 4, 0, 0, 1, 24, 31, 52, 17, 7,  12, 17};
    QTest::newRow("count=1") << QByteArray(data,23) << 1;

    char data1[] = {104, 17, 2, 0, 2, 0, 1,char(132), 20, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1};
    QTest::newRow("count=4") << QByteArray(data1,19) << 4;

}

void TestParserTest::testCase1()
{
    QFETCH(QByteArray, rawData);
    QFETCH(int, count);

    QList<CIECSignal> *result = IEC104Tools::ParseData(rawData,0);
    //QCOMPARE(true,true);
    QCOMPARE(result->count(),count);
    //QVERIFY2(true, "Failure");
}
void TestParserTest::testCaseIOA_data()
{
    QTest::addColumn<QByteArray>("rawData");
    QTest::addColumn<quint32>("IOA");

    char data1[] = {104,21, 0, 0, 0, 0, 30, char(129), 3, 0, 1, 0, 20, 0, 0, 1, 24, 31, 52, 17, 7,  12, 17};
    QTest::newRow("sq=1, 1 byte IOA") << QByteArray(data1,23) << quint32(20);

    char data2[] = {104,21, 0, 0, 0, 0, 30, char(129), 3, 0, 1, 0, 20, 20, 0, 1, 24, 31, 52, 17, 7,  12, 17};
    QTest::newRow("sq=1, 2 byte IOA") << QByteArray(data2,23) << quint32(5140);

    char data3[] = {104,21, 0, 0, 0, 0, 30, char(129), 3, 0, 1, 0, 20, 20, 20, 1, 24, 31, 52, 17, 7,  12, 17};
    QTest::newRow("sq=1, 3 bytes IOA") << QByteArray(data3,23) << quint32(0x141414);
}

void TestParserTest::testCaseIOA()
{
    QFETCH(QByteArray, rawData);
    QFETCH(quint32, IOA);

    QList<CIECSignal> *result = IEC104Tools::ParseData(rawData,0);
    CIECSignal signal = result->at(0);

    QCOMPARE(signal.GetAddress(), IOA);
}

void TestParserTest::testCaseFloat_data()
{
    QTest::addColumn<QByteArray>("rawData");
    QTest::addColumn<float>("value");

    char data1[] = {0x68, 0x12, 0x06, 0x00, 0x02, 0x00, 0x0D, (char)0x81, 0x14, 0x00, 0x01, 0x00, 0x0C, 0x00, 0x00, (char)0xC3, (char)0xF5, 0x48, 0x40, 0x00};
    QTest::newRow("sq=1, 1 float") << QByteArray(data1,20) << 3.14f;

}

void TestParserTest::testCaseFloat()
{
    QFETCH(QByteArray, rawData);
    QFETCH(float, value);

    QList<CIECSignal> *result = IEC104Tools::ParseData(rawData,0);
    CIECSignal signal = result->at(0);

    QCOMPARE(signal.value.toFloat(), value);
}
//float 3.14
//68 12 06 00 02 00 0D 81 14 00 01 00 0C 00 00 C3 F5 48 40 00
QTEST_APPLESS_MAIN(TestParserTest)

#include "tst_testparsertest.moc"
