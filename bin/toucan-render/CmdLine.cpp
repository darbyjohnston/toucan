// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 2024 Darby Johnston
// All rights reserved.

#include "CmdLine.h"

#include <cstdlib>

namespace toucan
{
    ICmdLineOption::ICmdLineOption(
        const std::vector<std::string>& names,
        const std::string& help) :
        _names(names),
        _help(help)
    {}

    ICmdLineOption::~ICmdLineOption()
    {}

    CmdLineFlagOption::CmdLineFlagOption(
        bool& value,
        const std::vector<std::string>& names,
        const std::string& help) :
        ICmdLineOption(names, help),
        _value(value)
    {}

    void CmdLineFlagOption::parse(std::vector<std::string>& args)
    {
        for (const auto& name : _names)
        {
            auto i = std::find(args.begin(), args.end(), name);
            if (i != args.end())
            {
                _matchedName = name;
                _value = true;
                i = args.erase(i);
            }
        }
    }

    std::vector<std::string> CmdLineFlagOption::getHelp() const
    {
        std::vector<std::string> out;
        out.push_back(join(_names, ", "));
        out.push_back(_help);
        return out;
    }

    bool cmdLineParse(std::vector<std::string>& args, std::vector<std::string>::iterator& it, std::string& value)
    {
        bool out = false;
        if (it != args.end())
        {
            value = *it;
            it = args.erase(it);
            out = true;
        }
        return out;
    }

    bool cmdLineParse(std::vector<std::string>& args, std::vector<std::string>::iterator& it, bool& value)
    {
        bool out = false;
        if (it != args.end())
        {
            value = std::atoi(it->c_str());
            it = args.erase(it);
            out = true;
        }
        return out;
    }

    bool cmdLineParse(std::vector<std::string>& args, std::vector<std::string>::iterator& it, int& value)
    {
        bool out = false;
        if (it != args.end())
        {
            value = std::atoi(it->c_str());
            it = args.erase(it);
            out = true;
        }
        return out;
    }

    bool cmdLineParse(std::vector<std::string>& args, std::vector<std::string>::iterator& it, float& value)
    {
        bool out = false;
        if (it != args.end())
        {
            value = std::atof(it->c_str());
            it = args.erase(it);
            out = true;
        }
        return out;
    }
}
