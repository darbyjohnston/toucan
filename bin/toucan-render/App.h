// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include "CmdLine.h"

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
        void _writeRaw(const OIIO::ImageBuf&);
        
        void _printHelp();
        
        std::string _exe;
        
        struct Args
        {
            std::string input;
            std::string output;
            bool outputRaw = false;
            std::vector<std::shared_ptr<ICmdLineArg> > list;
        };
        Args _args;
        
        struct Options
        {
            bool printStart = false;
            bool printDuration = false;
            bool printRate = false;
            bool printSize = false;
            std::string raw = "rgba";
            bool filmstrip = false;
            bool graph = false;
            bool verbose = false;
            bool help = false;
            std::vector<std::shared_ptr<ICmdLineOption> > list;
        };
        Options _options;
    };
}

