// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucan/CmdLine.h>
#include <toucan/ImageEffectHost.h>
#include <toucan/ImageGraph.h>
#include <toucan/TimelineWrapper.h>

#include <OpenImageIO/imagebuf.h>

extern "C"
{
#include <libswscale/swscale.h>

} // extern "C"

namespace toucan
{
    class App : public std::enable_shared_from_this<App>
    {
    public:
        App(std::vector<std::string>&);
        
        ~App();
        
        int run();
    
    private:
        void _writeRawFrame(const OIIO::ImageBuf&);
        void _writeY4mHeader();
        void _writeY4mFrame(const OIIO::ImageBuf&);
        
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
            std::string videoCodec = "MJPEG";
            bool printStart = false;
            bool printDuration = false;
            bool printRate = false;
            bool printSize = false;
            std::string raw;
            std::string y4m;
            bool verbose = false;
            bool help = false;
            std::vector<std::shared_ptr<ICmdLineOption> > list;
        };
        Options _options;

        std::shared_ptr<TimelineWrapper> _timelineWrapper;
        std::shared_ptr<ImageGraph> _graph;
        std::shared_ptr<ImageEffectHost> _host;

        AVFrame* _avFrame = nullptr;
        AVFrame* _avFrame2 = nullptr;
        AVPixelFormat _avInputPixelFormat = AV_PIX_FMT_NONE;
        AVPixelFormat _avOutputPixelFormat = AV_PIX_FMT_NONE;
        SwsContext* _swsContext = nullptr;
    };
}

