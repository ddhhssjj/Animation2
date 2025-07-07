#include "pch.h"
#include <fstream>
#include "json.hpp"
#include "AsepriteParser.h"

AnimationClips AsepriteParser::Load(const std::filesystem::path& jsonPath)
{
    // 1) JSON 로드하세요.

    std::ifstream ifs(jsonPath);

    if (!ifs.is_open())
    {
        std::cerr << "파일을 열 수 없습니다.\n";
    }

    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    auto parsed = nlohmann::json::parse(content, nullptr, false);  // 마지막 인자 'false'는 예외 비활성화

    if (parsed.is_discarded())
    {
        std::cerr << "JSON 파싱 실패: 유효하지 않은 문서입니다.\n";
    }

    // 2) 태그별로  AnimationClips 생성합니다.
    AnimationClips clips;

    const auto& root = parsed;
    auto& framesNode = root["frames"];

    if (root["meta"].contains("frameTags"))
    {
        for (const auto& t : root["meta"]["frameTags"]) {
            std::pair<std::string, AnimationClip> clipPair;
            clipPair.first = t["name"];

            int sI = t["from"];
            int eI = t["to"];

            if (framesNode.is_object() || framesNode.is_array())
            {
                for (int i = sI; i <= eI; i++)
                {
                    const auto& f = framesNode[i];// it.value();
                    Frame fd;
                    //fd.m_filename = it.key();
                    fd.srcRect.left = f["frame"]["x"];
                    fd.srcRect.right = fd.srcRect.left + f["frame"]["w"];
                    fd.srcRect.top = f["frame"]["y"];
                    fd.srcRect.bottom = fd.srcRect.top + f["frame"]["h"];
                    //fd.m_rotated = f["rotated"];
                    //fd.m_trimmed = f["trimmed"];
                    //fd.m_spriteSourceSize.m_x = f["spriteSourceSize"]["x"];
                    //fd.m_spriteSourceSize.m_y = f["spriteSourceSize"]["y"];
                    //fd.m_spriteSourceSize.m_w = f["spriteSourceSize"]["w"];
                    //fd.m_spriteSourceSize.m_h = f["spriteSourceSize"]["h"];
                    //fd.m_sourceSize.m_w = f["sourceSize"]["w"];
                    //fd.m_sourceSize.m_h = f["sourceSize"]["h"];
                    fd.duration = f["duration"];
                    fd.duration /= 1000;
                    clipPair.second.AddFrame(fd);
                }
            }
            else
            {
                std::cerr << "Unsupported 'frames' format" << '\n';
            }
            clips.push_back(clipPair);
        }
    }

    //// 3) slices (옵션)
    //if (root["meta"].contains("slices"))
    //{
    //    for (const auto& s : root["meta"]["slices"]) {
    //        Slice slice;
    //        slice.m_name = s["name"];
    //        for (const auto& key : s["keys"]) {
    //            SliceKey sk;
    //            sk.m_frame = key["frame"];
    //            sk.m_bounds.m_x = key["bounds"]["x"];
    //            sk.m_bounds.m_y = key["bounds"]["y"];
    //            sk.m_bounds.m_w = key["bounds"]["w"];
    //            sk.m_bounds.m_h = key["bounds"]["h"];
    //            sk.m_pivot.m_x = key["pivot"]["x"];
    //            sk.m_pivot.m_y = key["pivot"]["y"];
    //            slice.m_keys.push_back(sk);
    //        }

    //        outData.m_slices.push_back(slice);
    //    }
    //}

    return clips;
}
