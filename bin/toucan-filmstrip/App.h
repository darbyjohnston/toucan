// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once


#include <toucanRender/ImageEffectHost.h>
#include <toucanRender/ImageGraph.h>
#include <toucanRender/TimelineWrapper.h>
#include <toucanUtil/CmdLine.h>

#include <OpenImageIO/imagebuf.h>

namespace toucan
{
    class App : public std::enable_shared_from_this<App>
    {
    public:
        App(std::vector<std::string>&);
        
        ~App();
        
        int run();
    
    private:
        void _printHelp();
        
        std::string _exe;
        
        struct Args
        {
            std::string input;
            std::string output;
            std::vector<std::shared_ptr<ICmdLineArg> > list;
        };
        Args _args;
        
        struct Options
        {
            bool verbose = false;
            bool help = false;
            std::vector<std::shared_ptr<ICmdLineOption> > list;
        };
        Options _options;

        std::shared_ptr<TimelineWrapper> _timelineWrapper;
        std::shared_ptr<ImageGraph> _graph;
        std::shared_ptr<ImageEffectHost> _host;
    };
}

