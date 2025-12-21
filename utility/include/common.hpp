//! @file common.hpp
//! @author ryftchen
//! @brief The declarations (common) in the utility module.
//! @version 0.1.0
//! @copyright Copyright (c) 2022-2025 ryftchen. All rights reserved.

#pragma once

#include <algorithm>
#include <condition_variable>
#include <cstring>
#include <format>
#include <shared_mutex>

//! @brief The utility module.
namespace utility // NOLINT(modernize-concat-nested-namespaces)
{
//! @brief Common-interface-related functions in the utility module.
namespace common
{
//! @brief Brief function description.
//! @return function description (module_function)
inline static const char* description() noexcept
{
    return "UTIL_COMMON";
}
extern const char* version() noexcept;

//! @brief Hash seed for BKDR hash function.
constexpr std::size_t bkdrHashSeed = 131;
//! @brief Hash mask for BKDR hash function.
constexpr std::size_t bkdrHashMask = 0x7FFFFFFF;
//! @brief The Brian-Kernighan Dennis-Ritchie hash function (recursive).
//! @param str - input data
//! @param hash - recursive hash value
//! @return hash value
constexpr std::size_t bkdrHashRecursive(const char* const str, const std::size_t hash = 0) noexcept
{
    return str ? ((*str != '\0') ? bkdrHashRecursive(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashMask) : hash) : 0;
}
//! @brief The operator ("") overloading with BKDR hash function.
//! @param str - input data
//! @return hash value
constexpr std::size_t operator""_bkdrHash(const char* const str, const std::size_t /*len*/) noexcept
{
    return str ? bkdrHashRecursive(str) : 0;
}
//! @brief The operator ("") overloading with BKDR hash function.
//! @param str - input data
//! @return hash value
constexpr std::size_t operator""_bkdrHash(const char* const str) noexcept
{
    return str ? bkdrHashRecursive(str) : 0;
}
extern std::size_t bkdrHash(const char* str) noexcept;

extern std::string base64Encode(const std::string_view data);
extern std::string base64Decode(const std::string_view data);

extern std::string printfString(const char* const fmt, ...);
//! @brief Format as a string (format style).
//! @tparam Args - type of arguments
//! @param fmt - null-terminated multibyte string specifying how to interpret the data
//! @param args - arguments
//! @return string after formatting
template <typename... Args>
inline std::string formatString(
    const std::string_view fmt, Args&&... args) // NOLINT(cppcoreguidelines-missing-std-forward)
{
    return std::vformat(fmt, std::make_format_args(args...));
}

//! @brief Compare whether multiple strings are equal.
//! @tparam Others - type of arguments of string
//! @param str1 - string 1
//! @param str2 - string 2
//! @param others - arguments of string
//! @return be equal or not
template <typename... Others>
requires (std::is_same_v<Others, const char*> && ...)
inline bool areStringsEqual(const char* str1, const char* str2, Others... others)
{
    return (str1 && str2 && (... && others))
        && ((std::strcmp(str1, str2) == 0) && ((std::strcmp(str1, others) == 0) && ...));
}

//! @brief Splice strings into constexpr type.
//! @tparam Strings - target strings to be spliced
template <const std::string_view&... Strings>
class ConcatString
{
private:
    //! @brief A sequence of characters.
    static constexpr std::array characters{
        []() constexpr noexcept
        {
            constexpr auto len = (Strings.length() + ... + 0);
            std::array<char, len + 1> str{};
            char* dst = str.data();
            ((std::copy_n(Strings.cbegin(), Strings.length(), dst), dst += Strings.length()), ...);
            str[len] = '\0';
            return str;
        }()};

public:
    //! @brief The splicing result. Converted from a sequence of characters.
    static constexpr std::string_view value{characters.data(), characters.size() - 1};
};
//! @brief Get the result of splicing strings.
//! @tparam Strings - target strings to be spliced
template <const std::string_view&... Strings>
inline constexpr auto concatString = ConcatString<Strings...>::value;

//! @brief Check whether the target class is stateless.
//! @tparam Cls - type of target class
//! @return be stateless or not
template <typename Cls>
consteval bool isStatelessClass()
{
    return std::is_empty_v<Cls> || ((sizeof(Cls) == sizeof(void*)) && std::is_polymorphic_v<Cls>);
}

//! @brief Check whether the target value is part of the enumeration.
//! @tparam Enum - type of enumeration
//! @tparam Values - arguments of enumeration
template <typename Enum, Enum... Values>
requires std::is_enum_v<Enum>
struct EnumCheck;
//! @brief Check whether the target value is part of the enumeration.
//! @tparam Enum - type of enumeration
template <typename Enum>
requires std::is_enum_v<Enum>
struct EnumCheck<Enum>
{
    //! @brief Check whether it contains the value as an enumeration.
    //! @return has or not
    static constexpr bool has(const std::integral auto /*val*/) { return false; }
};
//! @brief Check whether the target value is part of the enumeration.
//! @tparam Enum - type of enumeration
//! @tparam Curr - current enumeration value
//! @tparam Next - next enumeration value
template <typename Enum, Enum Curr, Enum... Next>
requires std::is_enum_v<Enum>
struct EnumCheck<Enum, Curr, Next...> : private EnumCheck<Enum, Next...>
{
    //! @brief Check whether it contains the value as an enumeration.
    //! @param val - target value
    //! @return has or not
    static constexpr bool has(const std::integral auto val)
    {
        return (static_cast<std::underlying_type_t<Enum>>(Curr) == val) || EnumCheck<Enum, Next...>::has(val);
    }
};

//! @brief Pattern matching for the variant.
//! @tparam Variant - type of target variant
//! @tparam Cases - types of cases to match
//! @param var - target variant
//! @param cases - cases to match
//! @return result of matching
template <typename Variant, typename... Cases>
constexpr decltype(auto) patternMatch(Variant&& var, Cases&&... cases)
{
    struct Overloaded : public Cases...
    {
        using Cases::operator()...;
    };
    return std::visit(Overloaded{std::forward<Cases>(cases)...}, std::forward<Variant>(var));
}

//! @brief Closure wrapper.
//! @tparam Func - type of callable function
//! @tparam Op - type of call operator
//! @tparam Wrap - flag to indicate that further wrapping is required
template <typename Func, typename Op = decltype(&Func::operator()), bool Wrap = (sizeof(Func) > (sizeof(void*) * 2U))>
struct WrapClosure
{
    //! @brief Wrap operation.
    //! @tparam Clos - type of closure
    //! @param closure - target closure
    //! @return original closure
    template <typename Clos>
    static constexpr auto&& wrap(Clos&& closure) noexcept
    {
        return std::forward<Clos>(closure);
    }
};
//! @brief Closure wrapper. For the non-const member function.
//! @tparam Func - type of callable function
//! @tparam Ret - type of return value
//! @tparam Obj - type of object to which the member belongs
//! @tparam Args - type of function arguments
template <typename Func, typename Ret, typename Obj, typename... Args>
struct WrapClosure<Func, Ret (Obj::*)(Args...), true>
{
    //! @brief Wrap operation.
    //! @tparam Clos - type of closure
    //! @param closure - target closure
    //! @return wrapped closure
    template <typename Clos>
    static auto wrap(Clos&& closure) // NOLINT(cppcoreguidelines-missing-std-forward)
    {
        return [sharedClosure = std::make_shared<Func>(std::forward<Clos>(closure))](Args&&... args) mutable
        { return (*sharedClosure)(std::forward<Args>(args)...); };
    }
};
//! @brief Closure wrapper. For the const member function.
//! @tparam Func - type of callable function
//! @tparam Ret - type of return value
//! @tparam Obj - type of object to which the member belongs
//! @tparam Args - type of function arguments
template <typename Func, typename Ret, typename Obj, typename... Args>
struct WrapClosure<Func, Ret (Obj::*)(Args...) const, true>
{
    //! @brief Wrap operation.
    //! @tparam Clos - type of closure
    //! @param closure - target closure
    //! @return wrapped closure
    template <typename Clos>
    static auto wrap(Clos&& closure) // NOLINT(cppcoreguidelines-missing-std-forward)
    {
        return [sharedClosure = std::make_shared<Func>(std::forward<Clos>(closure))](Args&&... args)
        { return (*sharedClosure)(std::forward<Args>(args)...); };
    }
};
//! @brief Wrap closure further.
//! @tparam Clos - type of closure
//! @param closure - target closure
//! @return wrapped closure
template <typename Clos>
inline auto wrapClosure(Clos&& closure)
{
    return WrapClosure<std::decay_t<Clos>>::wrap(std::forward<Clos>(closure));
}

//! @brief The instance manager that never calls the destructor. Use for the function-level static variables.
//! @tparam Inst - type of instance
template <typename Inst>
class NoDestructor final
{
public:
    //! @brief Construct a new NoDestructor object.
    //! @tparam Args - type of constructor arguments
    //! @param args - constructor arguments
    template <typename... Args>
    explicit NoDestructor(Args&&... args);
    //! @brief Destroy the NoDestructor object.
    ~NoDestructor() = default;
    //! @brief Construct a new NoDestructor object.
    NoDestructor(const NoDestructor&) = delete;
    //! @brief Construct a new NoDestructor object.
    NoDestructor(NoDestructor&&) noexcept = default;
    //! @brief The operator (=) overloading of NoDestructor class.
    //! @return reference of the NoDestructor object
    NoDestructor& operator=(const NoDestructor&) = delete;
    //! @brief The operator (=) overloading of NoDestructor class.
    //! @return reference of the NoDestructor object
    NoDestructor& operator=(NoDestructor&&) noexcept = default;

    //! @brief Get the pointer of the instance.
    //! @return pointer of the instance
    Inst* get() noexcept;
    //! @brief Get the const pointer of the instance.
    //! @return const pointer of the instance
    const Inst* get() const noexcept;

private:
    //! @brief Storage for the instance.
    alignas(Inst) std::array<std::byte, sizeof(Inst)> storage{};

    static_assert(
        (sizeof(storage) >= sizeof(Inst)) // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
        && (alignof(decltype(*reinterpret_cast<Inst*>(storage.data()))) >= alignof(Inst)));
};
// NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
template <typename Inst>
template <typename... Args>
NoDestructor<Inst>::NoDestructor(Args&&... args)
{
    std::construct_at(reinterpret_cast<Inst*>(storage.data()), std::forward<Args>(args)...);
}
template <typename Inst>
Inst* NoDestructor<Inst>::get() noexcept
{
    return reinterpret_cast<Inst*>(storage.data());
}
template <typename Inst>
const Inst* NoDestructor<Inst>::get() const noexcept
{
    return reinterpret_cast<const Inst*>(storage.data());
}
// NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)

//! @brief Simple spin lock.
class SpinLock
{
public:
    //! @brief Acquire the lock.
    void lock();
    //! @brief Release the lock.
    void unlock();
    //! @brief Try to acquire the lock without blocking.
    //! @return acquire or not
    bool tryLock();

private:
    //! @brief The atomic flag used to implement the spin lock.
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

//! @brief Lock to control reading and writing.
class ReadWriteLock
{
public:
    //! @brief Acquire a read lock.
    void readLock();
    //! @brief Release a read lock.
    void readUnlock();
    //! @brief Acquire a write lock.
    void writeLock();
    //! @brief Release a write lock.
    void writeUnlock();
    //! @brief Enumerate specific lock modes.
    enum class LockMode : std::uint8_t
    {
        //! @brief Read.
        read,
        //! @brief Write.
        write
    };

private:
    //! @brief Handling of shared and exclusive locks.
    std::shared_mutex rwLock;
    //! @brief Counter of readers that have acquired the shared lock.
    std::atomic_uint_fast64_t reader{0};
    //! @brief Counter of writers that have acquired the exclusive lock.
    std::atomic_uint_fast64_t writer{0};
    //! @brief Mutex for controlling counters.
    mutable std::mutex mtx;
    //! @brief The synchronization condition for counters. Use with mtx.
    std::condition_variable cond;
};

//! @brief Manage the lifetime of a lock under control.
class LockGuard
{
public:
    //! @brief Alias for the lock mode.
    using LockMode = ReadWriteLock::LockMode;
    //! @brief Construct a new LockGuard object.
    //! @param lock - object managed by the guard
    //! @param mode - lock mode
    LockGuard(ReadWriteLock& lock, const LockMode mode);
    //! @brief Destroy the LockGuard object.
    virtual ~LockGuard();
    //! @brief Construct a new LockGuard object.
    LockGuard(const LockGuard&) = delete;
    //! @brief Construct a new LockGuard object.
    LockGuard(LockGuard&&) = delete;
    //! @brief The operator (=) overloading of LockGuard class.
    //! @return reference of the LockGuard object
    LockGuard& operator=(const LockGuard&) = delete;
    //! @brief The operator (=) overloading of LockGuard class.
    //! @return reference of the LockGuard object
    LockGuard& operator=(LockGuard&&) = delete;

private:
    //! @brief Object managed by the guard.
    ReadWriteLock& lock;
    //! @brief Lock mode.
    const LockMode mode{LockMode::read};
};
} // namespace common
} // namespace utility
