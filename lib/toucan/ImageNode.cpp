// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "ImageNode.h"

namespace toucan
{
    IImageNode::IImageNode(
        const std::string& name,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        _name(name),
        _inputs(inputs)
    {}

    IImageNode::~IImageNode()
    {}

    const std::string& IImageNode::getName() const
    {
        return _name;
    }

    std::string IImageNode::getLabel() const
    {
        return _name;
    }

    const std::vector<std::shared_ptr<IImageNode> >& IImageNode::getInputs() const
    {
        return _inputs;
    }

    void IImageNode::setInputs(const std::vector<std::shared_ptr<IImageNode> >& value)
    {
        _inputs = value;
    }

    const OTIO_NS::RationalTime& IImageNode::getTimeOffset() const
    {
        return _timeOffset;
    }

    void IImageNode::setTimeOffset(const OTIO_NS::RationalTime& timeOffset)
    {
        _timeOffset = timeOffset;
    }

    std::vector<std::string> IImageNode::graph(
        const OTIO_NS::RationalTime& time,
        const std::string& name)
    {
        std::vector<std::string> out;
        std::stringstream ss;
        ss << "digraph " << name << " {";
        out.push_back(ss.str());
        out.push_back("    node [shape=box, fontsize=12, margin=0.05, width=0, height=0];");
        _graph(time, shared_from_this(), out);
        out.push_back("}");
        return out;
    }

    void IImageNode::_graph(
        OTIO_NS::RationalTime time,
        const std::shared_ptr<IImageNode>& node,
        std::vector<std::string>& out)
    {
        if (!node->_timeOffset.is_invalid_time())
        {
            time -= node->_timeOffset;
        }
        std::stringstream ss;
        const std::string graphName = node->_getGraphName();
        ss << "    " << graphName << " [label=\"" << node->getLabel() << "\"]";
        out.push_back(ss.str());
        for (const auto& input : node->_inputs)
        {
            if (input)
            {
                std::stringstream ss;
                ss << "    " << input->_getGraphName() << " -> " << graphName;
                out.push_back(ss.str());
                _graph(time, input, out);
            }
        }
    }

    std::string IImageNode::_getGraphName() const
    {
        std::stringstream ss;
        ss << getName() << "_" << int64_t(shared_from_this().get());
        return ss.str();
    }
}
