#pragma once

namespace ds_tree
{
class TreeStructure
{
public:
    TreeStructure();
    virtual ~TreeStructure() = default;

    void bsInstance() const;
    void avlInstance() const;
    void splayInstance() const;
};
} // namespace ds_tree
