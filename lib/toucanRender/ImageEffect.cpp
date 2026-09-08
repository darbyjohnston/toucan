// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "ImageEffect.h"

#include <toucanRender/Util.h>

namespace toucan
{
    ImageEffectNode::ImageEffectNode(
        ImageEffectPlugin& plugin,
        const OTIO_NS::AnyDictionary& metaData,
        const std::string& name,
        const std::vector<std::shared_ptr<IImageNode> >& inputs) :
        IImageNode(name, inputs),
        _plugin(plugin),
        _instance(new ImageEffectInstance),
        _handle{ &plugin, _instance.get() },
        _metaData(metaData)
    {
        // Set default values. They are stored the way OTIO metadata arrives,
        // a scalar or an AnyVector, so both take the same path in the host.
        for (const auto& param : _plugin.paramDefs)
        {
            auto& out = _instance->params[param.first];
            const auto type = _plugin.paramTypes.find(param.first);
            if (type != _plugin.paramTypes.end())
            {
                out.type = type->second;
            }
            int count = 0;
            param.second.getDimension(kOfxParamPropDefault, &count);
            auto props = param.second.getStringProperties();
            if (std::find(props.begin(), props.end(), kOfxParamPropDefault) != props.end())
            {
                char* s = nullptr;
                param.second.getString(kOfxParamPropDefault, 0, &s);
                if (s)
                {
                    out.value = std::string(s);
                }
                continue;
            }
            props = param.second.getDoubleProperties();
            if (std::find(props.begin(), props.end(), kOfxParamPropDefault) != props.end())
            {
                std::vector<double> d(count, 0.0);
                param.second.getDoubleN(kOfxParamPropDefault, count, d.data());
                if (1 == count)
                {
                    out.value = d[0];
                }
                else
                {
                    OTIO_NS::AnyVector v;
                    for (double i : d)
                    {
                        v.push_back(i);
                    }
                    out.value = v;
                }
                continue;
            }
            props = param.second.getIntProperties();
            if (std::find(props.begin(), props.end(), kOfxParamPropDefault) != props.end())
            {
                std::vector<int> n(count, 0);
                param.second.getIntN(kOfxParamPropDefault, count, n.data());
                if (1 == count)
                {
                    out.value = static_cast<int64_t>(n[0]);
                }
                else
                {
                    OTIO_NS::AnyVector v;
                    for (int i : n)
                    {
                        v.push_back(static_cast<int64_t>(i));
                    }
                    out.value = v;
                }
            }
        }

        // Set values.
        for (const auto& i : metaData)
        {
            auto& out = _instance->params[i.first];
            const auto type = _plugin.paramTypes.find(i.first);
            if (type != _plugin.paramTypes.end())
            {
                out.type = type->second;
            }
            out.value = i.second;
        }

        // Create the plugin instance.
        OfxStatus ofxStatus = _plugin.ofxPlugin->mainEntry(
            kOfxActionCreateInstance,
            &_handle,
            nullptr,
            nullptr);
    }

    ImageEffectNode::~ImageEffectNode()
    {
        // Destroy the plugin instance.
        OfxStatus ofxStatus = _plugin.ofxPlugin->mainEntry(
            kOfxActionDestroyInstance,
            &_handle,
            nullptr,
            nullptr);
    }

    OIIO::ImageBuf ImageEffectNode::exec()
    {
        OIIO::ImageBuf out;

        // Initialize the images.
        std::vector<OIIO::ImageBuf> inputs;
        IMATH_NAMESPACE::V2i size = IMATH_NAMESPACE::V2i(0, 0);
        auto i = _metaData.find("size");
        if (i != _metaData.end() && i->second.has_value())
        {
            anyToVec(std::any_cast<OTIO_NS::AnyVector>(i->second), size);
        }
        const std::string context = !_plugin.contexts.empty() ? _plugin.contexts.front() : std::string();
        if (context == kOfxImageEffectContextGenerator)
        {
            out = OIIO::ImageBuf(OIIO::ImageSpec(size.x, size.y, 4));
            _instance->images["Output"] = bufToPropSet(out);
        }
        else if (
            context == kOfxImageEffectContextFilter &&
            !_inputs.empty() &&
            _inputs[0])
        {
            inputs.push_back(_inputs[0]->exec());
            auto spec = inputs[0].spec();
            if (size.x > 0 && size.y > 0)
            {
                spec.width = size.x;
                spec.height = size.y;
            }
            out = OIIO::ImageBuf(spec);
            _instance->images["Source"] = bufToPropSet(inputs[0]);
            _instance->images["Output"] = bufToPropSet(out);
        }
        else if (
            context == kOfxImageEffectContextTransition &&
            _inputs.size() > 1 &&
            _inputs[0] &&
            _inputs[1])
        {
            inputs.push_back(_inputs[0]->exec());
            inputs.push_back(_inputs[1]->exec());
            auto spec = inputs[0].spec();
            if (size.x > 0 && size.y > 0)
            {
                spec.width = size.x;
                spec.height = size.y;
            }
            out = OIIO::ImageBuf(spec);
            _instance->images["SourceFrom"] = bufToPropSet(inputs[0]);
            _instance->images["SourceTo"] = bufToPropSet(inputs[1]);
            _instance->images["Output"] = bufToPropSet(out);
        }

        // Render.
        const auto& spec = out.spec();
        if (spec.width > 0 && spec.height > 0)
        {
            PropertySet args;
            args.setDouble(kOfxPropTime, 0, _time.value());
            OfxRectI bounds;
            bounds.x1 = 0;
            bounds.x2 = spec.width;
            bounds.y1 = 0;
            bounds.y2 = spec.height;
            args.setIntN(kOfxImageEffectPropRenderWindow, 4, &bounds.x1);

            _plugin.ofxPlugin->mainEntry(
                kOfxImageEffectActionRender,
                &_handle,
                (OfxPropertySetHandle)&args,
                nullptr);
        }

        return out;
    }
}
