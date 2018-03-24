#include "StdAfx.h"
#include "Reader.h"
#include "defines.h"

Reader::Reader(void)
: m_size(0)
{
}

Reader::~Reader(void)
{
}

void Reader::Initialize(const string& fileName)
{
    // Open input stream
    m_input.open(fileName, ios::in);
    if (!m_input.is_open())
        throw exception(ERROR_READER_OPEN_FAILED);
    
    // Recognize file size
    m_input.seekg(0, ios::end);
    m_size = m_input.tellg();
    m_input.seekg(0, ios::beg);
}

void Reader::Uninitialize(void)
{
    m_size = 0;
    m_input.close();
}

bool Reader::GetNext(KeyValue& pair)
{
    // Read a line from the input stream
    char buffer[512];
    m_input.getline(buffer, sizeof(buffer) / sizeof(buffer[0]));
    if ((m_input.gcount() == 0) && m_input.eof())
        return false;
    
    // Check for separator
    if ((buffer[0] == '\n') || (buffer[0] == '\0') || (strcmp(buffer, RECORD_SEPARATOR) == 0))
    {
        pair.first.clear();
        pair.second = buffer;
        return true;
    }
    // Split the input line to a pair of key & value
    return Tokenize(buffer, pair);
}

bool Reader::Tokenize(const char* line, KeyValue& pair)
{
    bool valid = false;
    bool delim = false;
    int i = 0;
    char key[64] = {0};
    const char* s = line;
    // Determine key
    for (; *s != '\0'; ++s)
    {
        // Validate key
        if (isalpha(*s) || isdigit(*s) || (*s == '_'))
        {
            if (delim || ((i == 0) && isdigit(*s)))
                throw exception(ERROR_READER_INVALID_KEY);
            key[i++] = *s;
        }
        else if (iswspace(*s))
            delim = true;
        else if (*s == KEYVALUE_DELIMETER)
        {
            key[i++] = '\0';
            valid = true;
            ++s;
            break;
        }
    }
    if (!valid)
        throw exception(ERROR_READER_INVALID_LINE);
    // Skip a white space before the value
    if (iswspace(*s))
        ++s;
    pair.first = key;
    pair.second = s;
    return true;
}

size_t Reader::GetSize(void)
{
    return (size_t)m_size;
}

size_t Reader::GetPosition(void)
{
    return (size_t)m_input.tellg();
}


