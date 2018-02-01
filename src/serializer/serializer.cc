//
// Copyright 2016 Horizon Robotics.
//

#define MODULE_TAG "_serializer"
#include <algorithm>
#include <string>
#include <vector>

#include "dms_frame/DMSFrame.pb.h"
#include "serializer/serializer.h"
#include "logging/DMSLog.h"
namespace HobotDMS
{
// de-serialize
MetaDeserializer::MetaDeserializer()
{
    m_fproto_deserialize = new DMSFrameProtocol::DMSFrame();
}
MetaDeserializer::~MetaDeserializer()
{
    Fini();
}
bool MetaDeserializer::Init()
{
    return true;
}
void MetaDeserializer::Fini()
{
    if(m_fproto_deserialize)
    {
        delete m_fproto_deserialize;
        m_fproto_deserialize = NULL;
    }
}

// TODO
bool MetaDeserializer::Deserialize(std::ifstream &ifs, size_t pos, size_t meta_len)
{
    return true;
};
bool MetaDeserializer::Deserialize(uint8_t *buf, size_t buf_size)
{
    LOGI_T(MODULE_TAG) << "MetaDeserializer::Deserialize get one frame before";
    m_fproto_deserialize->ParseFromArray(buf, buf_size);
    LOGI_T(MODULE_TAG) << "MetaDeserializer::Deserialize get one frame after";
    return true;
};

bool MetaDeserializer::getTime(int64_t &frame_id_, int64_t &timestamp_)
{
    if(m_fproto_deserialize == nullptr )
    {
        LOGI_T(MODULE_TAG) << "m_fproto_deserizlize == nullptr || frame == nullptr";
        return false;
    }
    if(m_fproto_deserialize->has_frame_id())
    {
        frame_id_ = m_fproto_deserialize->frame_id();
        LOGI_T(MODULE_TAG) << "get frame_id_: " << frame_id_;
    }
    // write img info
    LOGI_T(MODULE_TAG) << "Write img infor";
    timestamp_ = m_fproto_deserialize->image().time();

    return 0;
}
}  // namespace HobotDMS
