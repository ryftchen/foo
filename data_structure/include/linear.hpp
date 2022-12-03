#pragma once

namespace date_structure::linear
{
class LinearStructure
{
public:
    LinearStructure();
    virtual ~LinearStructure() = default;

    void linkedListInstance() const;
    void stackInstance() const;
    void queueInstance() const;

private:
    typedef struct TagMeta
    {
        int id;
        char name[4];
    } Meta;
};

namespace doubly_linked_list
{
typedef struct TagNode
{
    struct TagNode* prev;
    struct TagNode* next;
    void* p;
} Node, *DLL;

static int createDll(DLL* pHead);
static int destroyDll(DLL* pHead);
static int dllSize(DLL pHead);
static int dllIsEmpty(DLL pHead);
static void* dllGet(DLL pHead, const int index);
static void* dllGetFirst(DLL pHead);
static void* dllGetLast(DLL pHead);
static int dllInsert(DLL pHead, const int index, void* const pVal);
static int dllInsertFirst(DLL pHead, void* const pVal);
static int dllInsertLast(DLL pHead, void* const pVal);
static int dllDelete(DLL pHead, const int index);
static int dllDeleteFirst(DLL pHead);
static int dllDeleteLast(DLL pHead);
} // namespace doubly_linked_list
} // namespace date_structure::linear
