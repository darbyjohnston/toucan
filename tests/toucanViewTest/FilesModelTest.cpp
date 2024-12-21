// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

#include "FilesModelTest.h"

#include <toucanView/FilesModel.h>

#include <cassert>
#include <iostream>

namespace toucan
{
    namespace
    {
        class Test
        {
        public:
            Test(
                const std::shared_ptr<dtk::Context>& context,
                const std::shared_ptr<ImageEffectHost>& host)
            {
                model = std::make_shared<FilesModel>(context, host);

                filesObserver = dtk::ListObserver<std::shared_ptr<File> >::create(
                    model->observeFiles(),
                    [this](const std::vector<std::shared_ptr<File> >& value)
                    {
                        files = value;
                    });

                addObserver = dtk::ValueObserver<int>::create(
                    model->observeAdd(),
                    [this](int value)
                    {
                        add = value;
                    });

                removeObserver = dtk::ValueObserver<int>::create(
                    model->observeRemove(),
                    [this](int value)
                    {
                        remove = value;
                    });

                currentObserver = dtk::ValueObserver<std::shared_ptr<File>>::create(
                    model->observeCurrent(),
                    [this](const std::shared_ptr<File>& value)
                    {
                        current = value;
                    });

                currentIndexObserver = dtk::ValueObserver<int>::create(
                    model->observeCurrentIndex(),
                    [this](int value)
                    {
                        currentIndex = value;
                    });
            }

            std::shared_ptr<FilesModel> model;
            std::vector<std::shared_ptr<File> > files;
            int add = -1;
            int remove = -1;
            std::shared_ptr<File> current;
            int currentIndex = -1;

            std::shared_ptr<dtk::ListObserver<std::shared_ptr<File> > > filesObserver;
            std::shared_ptr<dtk::ValueObserver<int> > addObserver;
            std::shared_ptr<dtk::ValueObserver<int> > removeObserver;
            std::shared_ptr<dtk::ValueObserver<std::shared_ptr<File> > > currentObserver;
            std::shared_ptr<dtk::ValueObserver<int> > currentIndexObserver;
        };
    }

    void filesModelTest(
        const std::shared_ptr<dtk::Context>& context,
        const std::shared_ptr<ImageEffectHost>& host,
        const std::filesystem::path& path)
    {
        std::cout << "filesModelTest" << std::endl;
        const std::filesystem::path timelinePath = path / "CompositeTracks.otio";
        const std::filesystem::path timelinePath2 = path / "Gap.otio";
        {
            Test test(context, host);
            test.model->open(timelinePath);
            assert(1 == test.files.size());
            assert(0 == test.add);
            assert(0 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[0]);

            test.model->close();
            assert(test.files.empty());
            assert(0 == test.remove);
            assert(-1 == test.currentIndex);
            assert(!test.current);
        }
        {
            Test test(context, host);
            test.model->open(timelinePath);
            test.model->open(timelinePath2);
            assert(2 == test.files.size());
            assert(1 == test.add);
            assert(1 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[1]);

            test.model->close();
            assert(1 == test.files.size());
            assert(1 == test.remove);
            assert(0 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[0]);

            test.model->closeAll();
            assert(test.files.empty());
            assert(0 == test.remove);
            assert(-1 == test.currentIndex);
            assert(!test.current);

            test.model->open(timelinePath);
            test.model->open(timelinePath2);
            test.model->close(0);
            assert(1 == test.files.size());
            assert(0 == test.remove);
            assert(0 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[0]);

            test.model->close(0);
            assert(test.files.empty());
            assert(0 == test.remove);
            assert(-1 == test.currentIndex);
            assert(!test.current);
        }
        {
            Test test(context, host);
            test.model->open(timelinePath);
            test.model->open(timelinePath2);

            test.model->setCurrentIndex(0);
            test.model->setCurrentIndex(0);
            assert(0 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[0]);
            test.model->setCurrentIndex(1);
            assert(1 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[1]);

            test.model->next();
            assert(0 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[0]);

            test.model->next();
            assert(1 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[1]);

            test.model->prev();
            assert(0 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[0]);

            test.model->prev();
            assert(1 == test.currentIndex);
            assert(test.current);
            assert(test.current == test.files[1]);
        }
    }
}
