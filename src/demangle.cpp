#include "demangle.h"

#include <llvm/Demangle/Demangle.h>
#include <llvm/Demangle/ItaniumDemangle.h>
#include <llvm/Demangle/Utility.h>

namespace {
// A copy of llvm::BumpPointerAllocator
class BumpPointerAllocator {
    struct BlockMeta {
        BlockMeta* Next;
        size_t Current;
    };

    static constexpr size_t AllocSize = 4096;
    static constexpr size_t UsableAllocSize = AllocSize - sizeof(BlockMeta);

    alignas(long double) char InitialBuffer[AllocSize];
    BlockMeta* BlockList = nullptr;

    void grow() {
        char* NewMeta = static_cast<char *>(std::malloc(AllocSize));
        if (NewMeta == nullptr)
            std::terminate();
        BlockList = new (NewMeta) BlockMeta{BlockList, 0};
    }

    void* allocateMassive(size_t NBytes) {
        NBytes += sizeof(BlockMeta);
        BlockMeta* NewMeta = reinterpret_cast<BlockMeta*>(std::malloc(NBytes));
        if (NewMeta == nullptr)
            std::terminate();
        BlockList->Next = new (NewMeta) BlockMeta{BlockList->Next, 0};
        return static_cast<void*>(NewMeta + 1);
    }

public:
  BumpPointerAllocator()
      : BlockList(new (InitialBuffer) BlockMeta{nullptr, 0}) {}

    void* allocate(size_t N) {
        N = (N + 15u) & ~15u;
        if (N + BlockList->Current >= UsableAllocSize) {
            if (N > UsableAllocSize)
                return allocateMassive(N);
            grow();
        }
        BlockList->Current += N;
        return static_cast<void*>(reinterpret_cast<char*>(BlockList + 1) + BlockList->Current - N);
    }

    void reset() {
        while (BlockList) {
            BlockMeta* Tmp = BlockList;
            BlockList = BlockList->Next;
            if (reinterpret_cast<char*>(Tmp) != InitialBuffer)
                std::free(Tmp);
        }
        BlockList = new (InitialBuffer) BlockMeta{nullptr, 0};
    }

    ~BumpPointerAllocator() { reset(); }
};

// A copy of llvm::DefaultAllocator
class DefaultAllocator {
    BumpPointerAllocator Alloc;

public:
    void reset() { Alloc.reset(); }

    template<typename T, typename ...Args> T *makeNode(Args &&...args) {
        return new (Alloc.allocate(sizeof(T)))
            T(std::forward<Args>(args)...);
    }

    void *allocateNodeArray(size_t sz) {
        return Alloc.allocate(sizeof(llvm::itanium_demangle::Node *) * sz);
    }
};
}  // unnamed namespace

// A copy of llvm::itaniumDemangle
static char *itanium_demangle(std::string_view mangled, char *Buf, size_t *N, int *Status) {

    using Demangler = llvm::itanium_demangle::ManglingParser<DefaultAllocator>;

    if (mangled.empty() || (Buf != nullptr && N == nullptr)) {
        if (Status)
            *Status = llvm::demangle_invalid_args;
        return nullptr;
    }

    int InternalStatus = llvm::demangle_success;
    Demangler Parser(mangled.data(), mangled.data() + mangled.size());
    llvm::itanium_demangle::OutputStream S;
    llvm::itanium_demangle::Node *AST = Parser.parse();

    if (AST == nullptr)
        InternalStatus = llvm::demangle_invalid_mangled_name;
    else if (!llvm::itanium_demangle::initializeOutputStream(Buf, N, S, 1024))
        InternalStatus = llvm::demangle_memory_alloc_failure;
    else {
        assert(Parser.ForwardTemplateRefs.empty());
        AST->print(S);
        S += '\0';
        if (N != nullptr)
            *N = S.getCurrentPosition();
        Buf = S.getBuffer();
    }

    if (Status)
        *Status = InternalStatus;
    return InternalStatus == llvm::demangle_success ? Buf : nullptr;
}

// A copy of llvm::isItaniumEncoding
static bool is_itanium_encoding(std::string_view mangled) {
    size_t pos = mangled.find_first_not_of('_');
    // A valid Itanium encoding requires 1-4 leading underscores, followed by 'Z'.
    return pos > 0 && pos <= 4 && mangled[pos] == 'Z';
}

// A copy of llvm::damangle
std::string itanium_demangle(std::string_view mangled) {
    // #TODO: Fix wasted allocation?
    char *demangled;
    if (is_itanium_encoding(mangled)) {
        demangled = itanium_demangle(mangled, nullptr, nullptr, nullptr);
    } else {
        demangled = nullptr;
    }
    if (!demangled) {
        return std::string{mangled};
    }
    std::string ret = demangled;
    std::free(demangled);
    return ret;
}
