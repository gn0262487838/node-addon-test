#include <napi.h>
#include <iostream>
#include <windows.h>
#include <comdef.h>
#import "C:/NHI/LIB/csHisX.dll" raw_interfaces_only

using namespace Napi;

CSHISXLib::InhicshisxPtr pCSHISXLibObj; // Global COM object pointer

// Function to initialize the COM object
Value InitializeCOM(const CallbackInfo &info)
{
    Env env = info.Env();

    HRESULT hr;
    CoInitialize(NULL); // Initialize COM library
    hr = pCSHISXLibObj.CreateInstance(__uuidof(CSHISXLib::nhicshisx));

    if (FAILED(hr))
    {
        CoUninitialize(); // Clean up COM library
        return Boolean::New(env, false);
    }

    std::cout << "COM object created successfully." << std::endl;
    return Boolean::New(env, true);
}

// Function to call VPNGetRandomX from the COM DLL
String VPNGetRandomX(const CallbackInfo &info)
{
    Env env = info.Env();

    if (!pCSHISXLibObj)
    {
        return String::New(env, "COM object not initialized.");
    }

    BSTR bstrResult;
    HRESULT hr = pCSHISXLibObj->VPNGetRandomX(&bstrResult);

    if (FAILED(hr))
    {
        return String::New(env, "Failed to call VPNGetRandomX. HRESULT: " + std::to_string(hr));
    }

    // Wrap BSTR in _bstr_t to automatically handle memory
    _bstr_t bstrWrap(bstrResult, false);
    std::string result = (const char *)bstrWrap;
    return String::New(env, result);
}

// Function to call VPNH_SignX from the COM DLL
String VPNH_SignX(const CallbackInfo &info)
{
    Env env = info.Env();

    // Check that exactly three string arguments are provided
    if (info.Length() < 3 || !info[0].IsString() || !info[1].IsString() || !info[2].IsString())
    {
        TypeError::New(env, "Expected three string arguments").ThrowAsJavaScriptException();
        return String::New(env, "Invalid arguments");
    }

    // Extract the input strings
    std::string input1 = info[0].As<String>().Utf8Value();
    std::string input2 = info[1].As<String>().Utf8Value();
    std::string input3 = info[2].As<String>().Utf8Value();

    HRESULT hr;
    CoInitialize(NULL); // Initialize COM library

    CSHISXLib::InhicshisxPtr pCSHISXLibObj;
    hr = pCSHISXLibObj.CreateInstance(__uuidof(CSHISXLib::nhicshisx));
    if (FAILED(hr))
    {
        return String::New(env, "Failed to create COM object. HRESULT: " + std::to_string(hr));
    }

    // Convert std::string to BSTR
    BSTR inputBstr1 = _com_util::ConvertStringToBSTR(input1.c_str());
    BSTR inputBstr2 = _com_util::ConvertStringToBSTR(input2.c_str());
    BSTR inputBstr3 = _com_util::ConvertStringToBSTR(input3.c_str());

    BSTR bstrResult;
    hr = pCSHISXLibObj->VPNH_SignX(inputBstr1, inputBstr2, inputBstr3, &bstrResult); // Call the method with three arguments

    // Free the BSTRs after use
    SysFreeString(inputBstr1);
    SysFreeString(inputBstr2);
    SysFreeString(inputBstr3);

    if (FAILED(hr))
    {
        return String::New(env, "Failed to call VPNH_SignX. HRESULT: " + std::to_string(hr));
    }

    _bstr_t bstrWrap(bstrResult, false);
    std::string result = (const char *)bstrWrap;

    // Do not uninitialize COM here
    return String::New(env, result);
}

// Function to clean up COM library
void CleanupCOM(const CallbackInfo &info)
{
    if (pCSHISXLibObj)
    {
        pCSHISXLibObj.Release(); // Release COM object
        pCSHISXLibObj = nullptr;
    }
    CoUninitialize(); // Clean up COM library
    std::cout << "COM library cleaned up successfully." << std::endl;
}

// Initialize the addon
Object Init(Env env, Object exports)
{
    exports.Set("initializeCOM", Function::New(env, InitializeCOM));
    exports.Set("VPNGetRandomX", Function::New(env, VPNGetRandomX));
    exports.Set("VPNH_SignX", Function::New(env, VPNH_SignX));
    exports.Set("cleanupCOM", Function::New(env, CleanupCOM));
    return exports;
}

NODE_API_MODULE(addon, Init)
