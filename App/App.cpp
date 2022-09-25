﻿#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <process.h>
#include <windows.h>

#include "sdk.h"
/*
#pragma comment(lib, "SDK.lib")
等效为在属性，链接，输入中添加该依赖
*/

typedef map<int, wstring> sqlTypes_t;
static sqlTypes_t sqlTypes
    = sqlTypes_t { { 1, L"INTEGER" }, { 2, L"FLOAT" }, { 3, L"TEXT" }, { 4, L"BLOB" }, { 5, L"NULL" } };

void printContacts(ContactMap_t contacts)
{
    wprintf(L"contacts number: %ld\n", contacts.size());
    for (auto it = contacts.begin(); it != contacts.end(); it++) {
        wprintf(L"%s\t%s\t%s\t%s\t%s\t%s\t%s\r\n", it->second.wxId.c_str(), it->second.wxCode.c_str(),
                it->second.wxName.c_str(), it->second.wxGender.c_str(), it->second.wxCountry.c_str(),
                it->second.wxProvince.c_str(), it->second.wxCity.c_str());
    }
}

void printDbNames(vector<wstring> vDbs)
{
    wprintf(L"db numbers: %ld\n", vDbs.size());
    for (auto it = vDbs.begin(); it != vDbs.end(); it++) {
        wprintf(L"%s\n", (*it).c_str());
    }
}

void printDbTables(DbTableVector_t tables)
{
    wprintf(L"table numbers: %ld\n", tables.size());
    for (auto it = tables.begin(); it != tables.end(); it++) {
        wprintf(L"%s\n%s\n\n", (it->table).c_str(), (it->sql).c_str());
    }
}

void printDbResults(SqlRetVector_t vvResults)
{
    int rows = vvResults.size();
    printf("vvResults.size: %d\n", rows);
    rows = 0;
    for (auto vv = vvResults.begin(); vv != vvResults.end(); vv++) {
        printf("Row %d\n", rows++);
        for (auto v = vv->begin(); v != vv->end(); v++) {
            wprintf(L"%s[%s]: ", v->column.c_str(), sqlTypes[v->type].c_str());
            switch (v->type) {
                case 1: {
                    printf("%d\n", stoi(v->content.c_str()));
                    break;
                }
                case 2: {
                    printf("%f\n", stof(v->content.c_str()));
                    break;
                }
                case 3: {
                    printf("%s\n", v->content.c_str());
                    break;
                }
                case 4: {
                    byte *p = (byte *)(v->content.c_str());
                    for (unsigned int i = 0; i < v->content.size(); i++) {
                        printf("%02X ", p[i]);
                    }
                    printf("\n");
                    break;
                }
                default: {
                    printf("\n");
                    break;
                }
            }
        }
    }
    printf("\n");
}

int onTextMsg(WxMessage_t msg)
{
    wprintf(L"%s msgType: %d, msgSource: %d, isSelf: %d\n", msg.id.c_str(), msg.type, msg.source, msg.self);
    wprintf(L"%s[%s] >> %s\n", msg.wxId.c_str(), msg.roomId.c_str(), msg.content.c_str());
    wprintf(L"msgSourceXml: %s\n", msg.xml.c_str());

    return 0;
}

int main()
{
    DWORD status     = 0;
    wstring wxid     = L"filehelper"; // 微信ID
    wstring at_wxid  = L"";
    wstring content  = L"这里填写消息内容";
    wstring img_path = L"test.jpg";

    setlocale(LC_ALL, "chs"); // 这是个大坑，不设置中文直接不见了。。。

    wprintf(L"WxInitSDK: ");
    status = WxInitSDK();
    wprintf(L"%d\n", status);
    if (status != 0) {
        return 0;
    }

    // 自己的 wxid
    wstring selfWxid = WxGetSelfWxid();
    wprintf(L"本号WXID：%s\n", selfWxid.c_str());

    // 获取消息类型
    wprintf(L"获取消息类型\n");
    const MsgTypesMap_t WxMsgTypes = WxGetMsgTypes();
    for (auto it = WxMsgTypes.begin(); it != WxMsgTypes.end(); ++it) {
        wprintf(L"%d: %s\n", it->first, it->second.c_str());
    }
    Sleep(1000); // 等待1秒

    wprintf(L"Message: 接收通知中......\n");
    WxEnableRecvMsg(onTextMsg);
    Sleep(1000); // 等待1秒

    // 测试发送消息
    wprintf(L"测试发送消息\n");
    WxSendTextMsg(wxid, content, at_wxid);
    Sleep(1000); // 等待1秒

    // 测试发送照片
    wprintf(L"测试发送照片\n");
    WxSendImageMsg(wxid, img_path);
    Sleep(1000); // 等待1秒

    // 测试获取联系人
    auto mContact = WxGetContacts();
    printContacts(mContact);
    Sleep(1000); // 等待1秒

    // 测试获取数据库名
    auto vDbNames = WxGetDbNames();
    printDbNames(vDbNames);
    Sleep(1000); // 等待1秒

    // 测试获取数据库中的表
    auto vDbTables = WxGetDbTables(L"MicroMsg.db");
    printDbTables(vDbTables);
    Sleep(1000); // 等待1秒

    // 测试执行 SQL
    auto vvResults = WxExecDbQuery(L"MicroMsg.db", L"SELECT * FROM Contact LIMIT 1;");
    printDbResults(vvResults);

    while (1) {
        Sleep(10000); // 休眠，释放CPU
    }
}
