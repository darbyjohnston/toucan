// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "Generator.h"

#include "Util.h"

#include <OpenImageIO/imagebufalgo.h>

namespace toucan
{
    CheckersNode::CheckersNode(
        const CheckersData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Checkers", inputs),
        _data(data)
    {}

    CheckersNode::~CheckersNode()
    {}

    const CheckersData& CheckersNode::getData() const
    {
        return _data;
    }

    void CheckersNode::setData(const CheckersData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf CheckersNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf(OIIO::ImageSpec(_data.size.x, _data.size.y, 4));
        PropertySet propSet;
        propSet.setIntN("checkerSize", 2, &_data.checkerSize.x);
        const double color1[] =
        {
            _data.color1.x,
            _data.color1.y,
            _data.color1.z,
            _data.color1.w
        };
        propSet.setDoubleN("color1", 4, color1);
        const double color2[] =
        {
            _data.color2.x,
            _data.color2.y,
            _data.color2.z,
            _data.color2.w
        };
        propSet.setDoubleN("color2", 4, color2);
        host->generator("Toucan:Checkers", buf, propSet);
        return buf;
    }

    FillNode::FillNode(
        const FillData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Fill", inputs),
        _data(data)
    {}

    FillNode::~FillNode()
    {}

    const FillData& FillNode::getData() const
    {
        return _data;
    }

    void FillNode::setData(const FillData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf FillNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf(OIIO::ImageSpec(_data.size.x, _data.size.y, 4));
        PropertySet propSet;
        const double color[] =
        {
            _data.color.x,
            _data.color.y,
            _data.color.z,
            _data.color.w
        };
        propSet.setDoubleN("color", 4, color);
        host->generator("Toucan:Fill", buf, propSet);
        return buf;
    }
    
    NoiseNode::NoiseNode(
        const NoiseData& data,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode("Noise", inputs),
        _data(data)
    {}

    NoiseNode::~NoiseNode()
    {}

    const NoiseData& NoiseNode::getData() const
    {
        return _data;
    }

    void NoiseNode::setData(const NoiseData& value)
    {
        _data = value;
    }

    OIIO::ImageBuf NoiseNode::exec(
        const OTIO_NS::RationalTime& time,
        const std::shared_ptr<ImageEffectHost>& host)
    {
        OIIO::ImageBuf buf(OIIO::ImageSpec(_data.size.x, _data.size.y, 4));
        PropertySet propSet;
        propSet.setString("type", 0, _data.type.c_str());
        propSet.setDouble("a", 0, _data.a);
        propSet.setDouble("b", 0, _data.b);
        propSet.setInt("mono", 0, _data.mono);
        propSet.setInt("seed", 0, _data.seed);
        host->generator("Toucan:Noise", buf, propSet);
        return buf;
    }
}
