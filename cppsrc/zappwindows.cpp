#include "zappwindows.h"
#include <tchar.h>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <string>


struct LUID_HOLDER
{
    int initialized;

    INT64 luid;
    DWORD LowPart;
    LONG HighPart;
};
int key[] = { 1, 85, 7, 105, 7, 9, 47, 9, 67, 9, 6, 73, 6, 3, 5, 164, 98, 6, 13, 0, 2 };
int keyLength = 21;

LUID_HOLDER GetDefaultFromToken(HANDLE token);
LUID_HOLDER GetElevatedLUIDFromToken(HANDLE token);
LUID_HOLDER GetLinkedLUIDFromToken(HANDLE token);
TOKEN_ELEVATION_TYPE GetTokenElevation(HANDLE token);
std::string readRegistryKey(HKEY parentkey,std::string keyloc,std::string keyName);
std::wstring toWide(const std::string& s);
std::string wstringTostring(const std::wstring& wstr);
std::wstring stringTowstring(const std::string& str);
std::string RegGetString(HKEY hKey,const std::string& subKey,const std::string& value);
char* obfuscateInput( char* data, size_t length );

std::string zappwindowsaddon::hello(){
    return "Hello World!";
}

Napi::String zappwindowsaddon::HelloWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::String returnValue = Napi::String::New(env, zappwindowsaddon::hello());
    return returnValue;
}

std::string zappwindowsaddon::getHKCUkey(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
    if (info.Length() < 2 || !info[0].IsString() || !info[1].IsString()) {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    } 

    Napi::String keyLoc = info[0].As<Napi::String>();
    Napi::String keyName = info[1].As<Napi::String>();
	return RegGetString(HKEY_CURRENT_USER,keyLoc.ToString(),keyName.ToString());

}


std::string RegGetString(HKEY hKey,const std::string& subKey,const std::string& value)
{
	std::wstring val;
	DWORD dataSize;
	std::wstring subkeyw=stringTowstring(subKey);
	
	std::wstring valuew = stringTowstring(value);
	
	LONG retCode = ::RegGetValueW(hKey,subkeyw.c_str(),valuew.c_str(),RRF_RT_REG_SZ,nullptr,nullptr,&dataSize);
	if (retCode == ERROR_SUCCESS)
	{
	  std::wstring data;
	  data.resize(dataSize / sizeof(wchar_t));
	  retCode = ::RegGetValueW(hKey,subkeyw.c_str(),valuew.c_str(),RRF_RT_REG_SZ,nullptr,&data[0],&dataSize);
	  if(retCode == ERROR_SUCCESS)
	  {
		  DWORD stringLengthInWchars = dataSize / sizeof(wchar_t);
		  stringLengthInWchars--; // Exclude the NUL written by the Win32 API
		  data.resize(stringLengthInWchars);
		  val=data;
	  }

	}

	return wstringTostring(val);
}

Napi::String zappwindowsaddon::GetHKCUKeyWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::String returnValue = Napi::String::New(env, zappwindowsaddon::getHKCUkey(info));
    return returnValue;
}
int zappwindowsaddon::GetLUID() {
    HANDLE hToken;
    if (!OpenProcessToken( GetCurrentProcess(), TOKEN_QUERY, &hToken ))
    {
        printf( "OpenProcessToken Error %u\n", GetLastError() );
        LUID_HOLDER holder;
        return holder.luid;
        
    }
    LUID_HOLDER holder=GetElevatedLUIDFromToken(hToken);
    return holder.luid;

}

Napi::Number zappwindowsaddon::GetLUIDWrapped(const Napi::CallbackInfo& info) {
     Napi::Env env = info.Env();
     int returnValue = zappwindowsaddon::GetLUID();
     return Napi::Number::New(env,returnValue);
}
std::string zappwindowsaddon::obfuscate(const Napi::CallbackInfo& info) {
    std::string obfuscatedVal="";
    Napi::Env env = info.Env();
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Parameters should be buffer ").ThrowAsJavaScriptException();
    } 
    // Retrieve the buffer
    Napi::Buffer<char> bufferObj = info[0].As<Napi::Buffer<char>>();
    char* data=bufferObj.Data();
    size_t length=bufferObj.Length();
    
    // size_t length =
    obfuscateInput(data,length);
   // obfuscatedVal=bufferObj.Data();
    // obfuscatedVal=output;
    return data;
}
char* obfuscateInput( char* data, size_t length )
{
	int keyIndex = 0;

	//XOR input string with the key
	for(int i=0; i<length; i++ )
	{
		data[i] = data[i] ^ key[keyIndex++];
		if(keyIndex >= keyLength)
			keyIndex = 0;
	}
	return data;
}
Napi::String zappwindowsaddon::obfuscateWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::String obfuscatedValue = Napi::String::New(env,zappwindowsaddon::obfuscate(info));
    return obfuscatedValue;
}
Napi::Object zappwindowsaddon::Init(Napi::Env env, Napi::Object exports) {
    exports.Set("hello", Napi::Function::New(env, zappwindowsaddon::HelloWrapped));
    exports.Set("GetLUID",Napi::Function::New(env,zappwindowsaddon::GetLUIDWrapped));
    exports.Set("getHKCUkey",Napi::Function::New(env,zappwindowsaddon::GetHKCUKeyWrapped));
    exports.Set("obfuscate",Napi::Function::New(env,zappwindowsaddon::obfuscateWrapped));
    return exports;
}

LUID_HOLDER GetElevatedLUIDFromToken(HANDLE token)
{
	TOKEN_ELEVATION_TYPE tokenType = GetTokenElevation(token);
	INT64 result = -1;
	LUID_HOLDER holder;

	printf("Elevation type %d \n", tokenType);

	if (tokenType != TokenElevationTypeDefault)
    {
        if (tokenType == TokenElevationTypeLimited)
        {
            holder = GetLinkedLUIDFromToken(token);
        }
        else
        {
            holder = GetDefaultFromToken(token);
        }
    }
    else
    {
        holder = GetDefaultFromToken(token);
    }

	return holder;
}


LUID_HOLDER GetDefaultFromToken(HANDLE token)
{
	PTOKEN_STATISTICS tokenStatistics = NULL;
    DWORD TokenInfLength = 0;
	INT64 luid64 = -1;

	LUID_HOLDER luidHolder;

	if (token == NULL)
		return luidHolder;

	GetTokenInformation(token, TokenStatistics, NULL, TokenInfLength, &TokenInfLength);
	tokenStatistics = (PTOKEN_STATISTICS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, TokenInfLength);

	if (GetTokenInformation(token, TokenStatistics, tokenStatistics, TokenInfLength, &TokenInfLength))
	{
		luid64 = tokenStatistics->AuthenticationId.HighPart;
		luid64 = luid64 << 32;
		luid64 += tokenStatistics->AuthenticationId.LowPart;

		luidHolder.luid = luid64;
		luidHolder.HighPart = tokenStatistics->AuthenticationId.HighPart;
		luidHolder.LowPart = tokenStatistics->AuthenticationId.LowPart;

		luidHolder.initialized = 1;
	}

	if (tokenStatistics != NULL)
		HeapFree(GetProcessHeap(), 0, tokenStatistics);

	return luidHolder;
}


LUID_HOLDER GetLinkedLUIDFromToken(HANDLE token)
{
	INT64 result = -1;
	LUID_HOLDER holder;
	PTOKEN_LINKED_TOKEN ptrLinkedToken = NULL;
	DWORD TokenInfLength = 0;

	if (token != NULL)
    {
        GetTokenInformation(token, TokenLinkedToken, NULL, TokenInfLength, &TokenInfLength);
		ptrLinkedToken = (PTOKEN_LINKED_TOKEN)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, TokenInfLength);

		if (GetTokenInformation(token, TokenLinkedToken, ptrLinkedToken, TokenInfLength, &TokenInfLength))
			holder = GetDefaultFromToken(ptrLinkedToken->LinkedToken);

		if(ptrLinkedToken != NULL)
			HeapFree(GetProcessHeap(), 0,  ptrLinkedToken);
	}

	return holder;
}
TOKEN_ELEVATION_TYPE GetTokenElevation(HANDLE token)
{
	TOKEN_ELEVATION_TYPE result = TokenElevationTypeDefault;
	DWORD TokenInfLength = 0;
	PTOKEN_ELEVATION tokenElevation = NULL;

	// first call gets lenght of TokenInformation
    GetTokenInformation(token, TokenElevationType, NULL, TokenInfLength, &TokenInfLength);

	tokenElevation = (PTOKEN_ELEVATION)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, TokenInfLength);

	if(GetTokenInformation(token, TokenElevationType, tokenElevation, TokenInfLength, &TokenInfLength))
		result = (TOKEN_ELEVATION_TYPE)tokenElevation->TokenIsElevated;

	if (tokenElevation != NULL)
		HeapFree(GetProcessHeap(), 0, tokenElevation);

	return result;
}

std::wstring toWide(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::wstring stringTowstring(const std::string& str)
{
	//using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

    return converterX.from_bytes(str);
}

std::string wstringTostring(const std::wstring& wstr)
{
    //using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;

    return converterX.to_bytes(wstr);
}



