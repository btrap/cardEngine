#pragma once

struct CharArray
{
    umax char_Length;
    umax allocated_Bytes;
    char *chars;
};

bool RemoveFront(CharArray *char_Array)
{
    if(char_Array->char_Length > 0)
    {
        --char_Array->char_Length;
        return true;
    }
    return false;
}

bool IsWhiteSpace(CharArray *char_Array)
{
    char test = char_Array->chars[0];
    bool result = (test == ' ') || (test == '\r') || (test == '\n') || (test == '\t') || (test == '\f') || (test == '\v');
    return result;
}

bool AddChar(CharArray *char_Array, char token)
{
    if(char_Array->allocated_Bytes > char_Array->char_Length)
    {
        char_Array->chars[char_Array->char_Length] = token;
        ++char_Array->char_Length;
        return true;
    }

    return false;
}
