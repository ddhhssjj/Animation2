#pragma once

#include <unordered_map>
#include <filesystem>
#include <wrl/client.h>
#include "AnimationClip.h"
#include "D2DRender.h"

// 예시입니다.
class AssetManager
{
public:
    using AnimationClips = std::vector<std::pair<std::string, AnimationClip>>;
   
    AssetManager() = default;
    ~AssetManager() = default;

    AnimationClips& GetClips(std::wstring filename);
    bool GetIsJson(std::wstring filename);

    void LoadTexture(sample::D2DRenderer* renderer, std::wstring& keywide, std::filesystem::path& path);

    void LoadAseprite(sample::D2DRenderer* renderer, std::wstring& keywide, std::filesystem::path& path);

private:

    std::unordered_map<std::wstring, Microsoft::WRL::ComPtr<ID2D1Bitmap1>> m_textures;
    std::unordered_map<std::wstring, AnimationClips> m_clipsMap;
    std::unordered_map<std::wstring, bool> m_isJson;
};

