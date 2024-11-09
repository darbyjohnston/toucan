// SPDX-License-Identifier: Apache-2.0
// Copyright Contributors to the toucan project.

namespace toucan
{
    template<typename T>
    inline std::shared_ptr<T> IContainer::find(Find find) const
    {
        std::shared_ptr<IItem> out;
        for (const auto& child : _children)
        {
            if ((out = std::dynamic_pointer_cast<T>(child)))
            {
                break;
            }
            if (Find::Recurse == find)
            {
                if (auto container = std::dynamic_pointer_cast<IContainer>(child))
                {
                    out = container->find<T>();
                }
            }
        }
        return out;
    }

    template<typename T>
    inline std::vector<std::shared_ptr<T> > IContainer::findAll(Find find) const
    {
        std::vector<std::shared_ptr<T> > out;
        _findAll(find, out);
        return out;
    }

    template<typename T>
    inline void IContainer::_findAll(Find find, std::vector<std::shared_ptr<T> >& out) const
    {
        for (const auto& child : _children)
        {
            if (auto t = std::dynamic_pointer_cast<T>(child))
            {
                out.push_back(t);
            }
            if (Find::Recurse == find)
            {
                if (auto container = std::dynamic_pointer_cast<IContainer>(child))
                {
                    container->_findAll<T>(find, out);
                }
            }
        }
    }
}
