// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#pragma once

#include <toucanRender/AudioGraph.h>
#include <toucanRender/ImageEffectHost.h>
#include <toucanRender/ImageGraph.h>
#include <toucanRender/TimelineWrapper.h>

#include <ftk/Core/CmdLine.h>
#include <ftk/Core/IApp.h>

#include <OpenImageIO/imagebuf.h>

extern "C"
{
#include <libswscale/swscale.h>

} // extern "C"

namespace toucan
{
    class App : public ftk::IApp
    {
    protected:
        void _init(
            const std::shared_ptr<ftk::Context>&,
            std::vector<std::string>&);

        App();

    public:
        ~App();

        static std::shared_ptr<App> create(
            const std::shared_ptr<ftk::Context>&,
            std::vector<std::string>&);

        void run();

    private:
        void _writeRawFrame(const OIIO::ImageBuf&);
        void _writeY4mHeader();
        void _writeY4mFrame(const OIIO::ImageBuf&);

        struct CmdLine
        {
            std::shared_ptr<ftk::CmdLineArg<std::string> > input;
            std::shared_ptr<ftk::CmdLineArg<std::string> > output;
            bool outputRaw = false;

            std::shared_ptr<ftk::CmdLineOption<std::string> > videoCodec;
            std::shared_ptr<ftk::CmdLineFlag> printStart;
            std::shared_ptr<ftk::CmdLineFlag> printDuration;
            std::shared_ptr<ftk::CmdLineFlag> printRate;
            std::shared_ptr<ftk::CmdLineFlag> printSize;
            std::shared_ptr<ftk::CmdLineOption<std::string> > raw;
            std::shared_ptr<ftk::CmdLineOption<std::string> > y4m;
            std::shared_ptr<ftk::CmdLineFlag> verbose;

            std::shared_ptr<ftk::CmdLineOption<std::string> > audioCodec;
            std::shared_ptr<ftk::CmdLineOption<int> > audioSampleRate;
            std::shared_ptr<ftk::CmdLineOption<int> > audioChannelCount;
            std::shared_ptr<ftk::CmdLineOption<std::string> > audioFile;
            std::shared_ptr<ftk::CmdLineFlag> noAudio;
        };
        CmdLine _cmdLine;

        std::shared_ptr<TimelineWrapper> _timelineWrapper;
        std::shared_ptr<ImageGraph> _graph;
        std::shared_ptr<AudioGraph> _audioGraph;
        std::shared_ptr<ImageEffectHost> _host;

        AVFrame* _avFrame = nullptr;
        AVFrame* _avFrame2 = nullptr;
        AVPixelFormat _avInputPixelFormat = AV_PIX_FMT_NONE;
        AVPixelFormat _avOutputPixelFormat = AV_PIX_FMT_NONE;
        SwsContext* _swsContext = nullptr;
    };
}
