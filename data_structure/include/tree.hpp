#pragma once

namespace date_structure::tree
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
} // namespace date_structure::tree
