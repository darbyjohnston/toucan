// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ThumbnailGenerator.h"

#include <toucan/ImageGraph.h>

#include <OpenImageIO/imagebufalgo.h>

#include <sstream>

namespace toucan
{
    ThumbnailGenerator::ThumbnailGenerator(
        const std::filesystem::path& path,
        const OTIO_NS::SerializableObject::Retainer<OTIO_NS::Timeline>& timeline,
        const std::shared_ptr<ImageEffectHost>& host) :
        _path(path),
        _timeline(timeline),
        _host(host)
    {
        _graph = std::make_shared<ImageGraph>(_path, _timeline);
        const IMATH_NAMESPACE::V2i& imageSize = _graph->getImageSize();
        _aspect = imageSize.y > 0 ?
            (imageSize.x / static_cast<float>(imageSize.y)) :
            0.F;
    }

    ThumbnailGenerator::~ThumbnailGenerator()
    {}

    float ThumbnailGenerator::getAspect() const
    {
        return _aspect;
    }

    std::future<Thumbnail> ThumbnailGenerator::getThumbnail(
        const OTIO_NS::RationalTime& time,
        int height)
    {
        auto graph = _graph->exec(_host, time);
        return std::async(
            std::launch::async,
            [this, graph, time, height]
            {
                const auto sourceBuf = graph->exec(time);
                const auto& sourceSpec = sourceBuf.spec();

                const dtk::Size2I thumbnailSize(height * _aspect, height);
                dtk::ImageInfo info;
                info.size = thumbnailSize;
                switch (sourceSpec.nchannels)
                {
                case 1:
                    switch (sourceSpec.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8: info.type = dtk::ImageType::L_U8; break;
                    case OIIO::TypeDesc::UINT16: info.type = dtk::ImageType::L_U16; break;
                    case OIIO::TypeDesc::HALF: info.type = dtk::ImageType::L_F16; break;
                    case OIIO::TypeDesc::FLOAT: info.type = dtk::ImageType::L_F32; break;
                    }
                    break;
                case 2:
                    switch (sourceSpec.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8: info.type = dtk::ImageType::LA_U8; break;
                    case OIIO::TypeDesc::UINT16: info.type = dtk::ImageType::LA_U16; break;
                    case OIIO::TypeDesc::HALF: info.type = dtk::ImageType::LA_F16; break;
                    case OIIO::TypeDesc::FLOAT: info.type = dtk::ImageType::LA_F32; break;
                    }
                    break;
                case 3:
                    switch (sourceSpec.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8: info.type = dtk::ImageType::RGB_U8; break;
                    case OIIO::TypeDesc::UINT16: info.type = dtk::ImageType::RGB_U16; break;
                    case OIIO::TypeDesc::HALF: info.type = dtk::ImageType::RGB_F16; break;
                    case OIIO::TypeDesc::FLOAT: info.type = dtk::ImageType::RGB_F32; break;
                    }
                    break;
                default:
                    switch (sourceSpec.format.basetype)
                    {
                    case OIIO::TypeDesc::UINT8: info.type = dtk::ImageType::RGBA_U8; break;
                    case OIIO::TypeDesc::UINT16: info.type = dtk::ImageType::RGBA_U16; break;
                    case OIIO::TypeDesc::HALF: info.type = dtk::ImageType::RGBA_F16; break;
                    case OIIO::TypeDesc::FLOAT: info.type = dtk::ImageType::RGBA_F32; break;
                    }
                    break;
                }
                info.layout.mirror.y = true;

                std::shared_ptr<dtk::Image> thumbnail;
                if (info.isValid())
                {
                    thumbnail = dtk::Image::create(info);
                    auto resizedBuf = OIIO::ImageBufAlgo::resize(
                        sourceBuf,
                        "",
                        0.F,
                        OIIO::ROI(
                            0, info.size.w,
                            0, info.size.h,
                            0, 1,
                            0, std::min(4, sourceSpec.nchannels)));
                    memcpy(
                        thumbnail->getData(),
                        resizedBuf.localpixels(),
                        thumbnail->getByteCount());
                }
                return Thumbnail{ time, thumbnail };
            });
    }
}
