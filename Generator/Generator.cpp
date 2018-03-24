#include "StdAfx.h"
#include "Generator.h"
#include "Helpers.h"

#define MAX_VALUE_COUNT        100

Generator::Generator(void)
{
}


Generator::~Generator(void)
{
    Cleanup();
}

void Generator::Prepare(const string& inputFileName, const string& outputFileName)
{
    // Initialize reader
    m_reader.Initialize(inputFileName);
    // Open output file
    m_output.open(outputFileName, ios::out | ios::trunc);
    if (!m_output.is_open())
        throw exception(ERROR_GENERATOR_OPEN_FAILED);
    
    KeyValue pair;
    while (m_reader.GetNext(pair))
    {
        // Skip empty lines and separators
        if (pair.first.empty())
            continue;
        
        // Find existing keys
        bool found = false;
        for (KeyValues::iterator iter = m_values.begin(); iter != m_values.end(); ++iter)
        {
            if (iter->first.compare(pair.first) == 0)
            {
                // Check limit for values per key
                if (iter->second.size() < MAX_VALUE_COUNT)
                    iter->second.push_back(pair.second);
                found = true;
                break;
            }
        }
        if (!found)
            m_values.push_back(KeyValues::value_type(pair.first, vector<string>(1, pair.second)));
    }
}

void Generator::Process(size_t recordCount, void* context, PFN_STEP_CALLBACK pfn)
{
    srand((unsigned int)time(NULL));
    
    m_output << RECORD_SEPARATOR << endl;
    // Process records
    for (size_t i = 0; i < recordCount; ++i)
    {
        // Enumerate keys
        for (KeyValues::const_iterator iter = m_values.cbegin(); iter != m_values.cend(); ++iter)
        {
            // Random index for values
            size_t index = rand() % iter->second.size();
            m_output << iter->first.c_str() << KEYVALUE_DELIMETER << " ";
            m_output << iter->second[index].c_str() << endl;
        }
        m_output << RECORD_SEPARATOR << endl;
        pfn(context, (int)i, (int)recordCount);
    }
}

void Generator::Cleanup(void)
{
    m_reader.Uninitialize();
    m_output.close();
    m_values.clear();
}

void Generator::Run(const string& inputFileName, const string& outputFileName, size_t recordCount)
{
    clock_t begin = clock();
    Generator generator;
    // Prepare dictionary and output
    generator.Prepare(inputFileName, outputFileName);
    // Initialize progress bar
    Progress progress(MESSAGE_GENERATOR_PROCESSING);
    // Begin processing
    progress.Begin();
    // Process data
    generator.Process(recordCount, &progress, Progress::Callback);
    // Clean up
    generator.Cleanup();
    // Finish processing
    progress.End();
    double duration = (double)(clock() - begin) / CLOCKS_PER_SEC;
    cout.precision(2);
    cout << endl << recordCount << " record(s)" << " successfully generated in " << fixed << duration << " seconds" << endl;
}

int Generator::Main(int argc, char* argv[])
{
    if (argc != 4)
    {
        cerr << "Usage:" << endl;
        cerr << "  generator DICTIONARY_FILE OUTPUT_FILE RECORD_COUNT" << endl;
        cerr << "    DICTIONARY_FILE  dictionary file name containing key and value pairs" << endl;
        cerr << "    OUTPUT_FILE      file name to output" << endl;
        cerr << "    RECORD_COUNT     number of records" << endl;
        cerr << "  e.g.: generator dictionary.txt data.txt 4000000" << endl;
        return EXIT_FAILURE;
    }
    
    int exitCode = EXIT_SUCCESS;
    try
    {
        string fileName(argv[1]);
        string dbName(argv[2]);
        size_t recordCount = (size_t)atoi(argv[3]);
        
        Generator::Run(fileName, dbName, recordCount);
    }
    catch (exception& exception)
    {
        exitCode = EXIT_FAILURE;
        cerr << endl << exception.what() << endl;
    }
    return exitCode;
}


