#include "pch.h"
#include "AnimationClip.h"
#include "AsepriteParser.h"
#include "D2DRender.h"
#include "AssetManager.h"

AnimationClips& AssetManager::GetClips(std::wstring filename)
{
    //for (const auto& [name, clip] : m_clipsMap)
    //{
    //    for (auto iter = clip.begin(); iter != clip.end(); iter++) {
    //        if (iter->first.c_str() == filename.c_str())
    //        {

    //        }
    //    }
    //
    //    for (const auto& [name2, clip2] : clip)
    //    {

    //    }
    //}
    //for (auto& clip : m_clipsMap)
    //{
    //    
    //    SpriteAnimator ap;

    //    ap.SetClip(&clip);

    //    m_curSprites.push_back(ap);
    //}
    return m_clipsMap[filename];
}

bool AssetManager::GetIsJson(std::wstring filename)
{
    return m_isJson[filename];
}

void AssetManager::LoadTexture(sample::D2DRenderer* renderer, std::wstring& keywide, std::filesystem::path& path)
{
    //Microsoft::WRL::ComPtr<ID2D1Bitmap1> animationBitmap;
    //renderer->CreateBitmapFromFile(path.c_str(), *(animationBitmap.GetAddressOf()));

    ////D2D1_SIZE_U size = animationBitmap.Get()->GetPixelSize();

    ////Frame frame;

    //AnimationClips temp;
    //std::pair<std::string, AnimationClip> temp2;
    //temp2.first = std::string();
    //temp2.second.SetBitmap(animationBitmap);
    //temp.push_back(temp2);


    //m_clipsMap.insert(std::pair<std::wstring, AnimationClips>(keywide, temp));
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> animationBitmap;
    renderer->CreateBitmapFromFile(path.c_str(), *(animationBitmap.ReleaseAndGetAddressOf()));
    std::pair<std::string, AnimationClip> temp;
    Frame frame;
    frame.srcRect.left = 0;
    frame.srcRect.right = animationBitmap.Get()->GetPixelSize().width;
    frame.srcRect.top = 0;
    frame.srcRect.bottom = animationBitmap.Get()->GetPixelSize().height;
    frame.duration == 100;
    temp.second.SetBitmap(animationBitmap);
    temp.second.AddFrame(frame);
    std::string string;
    string.assign(keywide.begin(), keywide.end());
    temp.first = std::string(string);
    AnimationClips temp2;
    temp2.push_back(temp);
    m_clipsMap.insert(std::pair<std::wstring, AnimationClips>(keywide, temp2));
}

void AssetManager::LoadAseprite(sample::D2DRenderer* renderer, std::wstring& keywide, std::filesystem::path& path)
{
    std::filesystem::path pngPath = path;
    pngPath.replace_extension(".png"); // 확장자 제거하고 키로 쓸꺼에요
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> animationBitmap;
    renderer->CreateBitmapFromFile(pngPath.c_str(), *(animationBitmap.ReleaseAndGetAddressOf()));

    AnimationClips temp = AsepriteParser::Load(path);
    for (auto& [name, clip] : temp)
    {
        clip.SetBitmap(animationBitmap);
    }
    m_clipsMap.erase(keywide);
    m_clipsMap.insert(std::pair<std::wstring, AnimationClips>(keywide, temp));
    m_isJson.insert(std::pair<std::wstring, bool>(keywide, true));
}
