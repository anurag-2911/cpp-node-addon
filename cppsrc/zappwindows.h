#include <napi.h>

namespace zappwindowsaddon
{
    std::string hello();
    Napi::String HelloWrapped(const Napi::CallbackInfo& info);

    int GetLUID();
    Napi::Number GetLUIDWrapped(const Napi::CallbackInfo& info);

    std::string getHKCUkey(const Napi::CallbackInfo& info);
    Napi::String GetHKCUKeyWrapped(const Napi::CallbackInfo& info);
    
    std::string obfuscate(const Napi::CallbackInfo& info);
    Napi::String obfuscateWrapped(const Napi::CallbackInfo& info);

    Napi::Object Init(Napi::Env env, Napi::Object exports);  
} 