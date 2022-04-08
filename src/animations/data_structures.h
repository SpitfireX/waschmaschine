#pragma once

enum DataType: char {
    U8,
    U16,
    BOOL,
};

struct SettingValue {
    const char* name;
    DataType data_type;
    unsigned char stepsize;
    unsigned short value;
};