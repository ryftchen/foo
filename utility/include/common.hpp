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
extern const char* version() noexcept;

//! @brief Hash seed for BKDR hash function.
constexpr std::size_t bkdrHashSeed = 131;
//! @brief Hash size for BKDR hash function.
constexpr std::size_t bkdrHashSize = 0x7FFFFFFF;

extern std::size_t bkdrHash(const char* str);
//! @brief The Brian-Kernighan Dennis-Ritchie hash function at compile time.
//! @param str - input data
//! @param hash - previous hash value
//! @return hash value
constexpr std::size_t bkdrHashInCompiling(const char* const str, const std::size_t hash = 0) noexcept
{
    return *str ? bkdrHashInCompiling(str + 1, (hash * bkdrHashSeed + *str) & bkdrHashSize) : hash;
}
//! @brief The operator ("") overloading with BKDR hash function.
//! @param str - input data
//! @return hash value
constexpr std::size_t operator""_bkdrHash(const char* str, const std::size_t /*len*/) noexcept
{
    return bkdrHashInCompiling(str);
}
//! @brief The operator ("") overloading with BKDR hash function.
//! @param str - input data
//! @return hash value
constexpr std::size_t operator""_bkdrHash(const char* str) noexcept
{
    return bkdrHashInCompiling(str);
}

extern std::string base64Encode(const std::string_view data);
extern std::string base64Decode(const std::string_view data);

extern std::string printfString(const char* const fmt, ...);
//! @brief Format as a string (format style).
//! @tparam Args - type of arguments
//! @param fmt - null-terminated multibyte string specifying how to interpret the data
//! @param args - arguments
//! @return string after formatting
template <typename... Args>
inline std::string formatString(const std::string_view fmt, Args&&... args)
{
    return std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
}

//! @brief Compare whether two strings are equal.
//! @param str1 - string 1
//! @param str2 - string 2
//! @return be equal or not
inline bool areStringsEqual(const char* const str1, const char* const str2)
{
    return std::strcmp(str1, str2) == 0;
}
//! @brief Compare whether multiple strings are equal.
//! @tparam Others - type of arguments of string
//! @param str1 - string 1
//! @param str2 - string 2
//! @param others - arguments of string
//! @return be equal or not
template <typename... Others>
inline bool areStringsEqual(const char* const str1, const char* const str2, Others&&... others)
{
    return areStringsEqual(str1, str2) && areStringsEqual(str2, std::forward<Others>(others)...);
}

//! @brief Splice strings into constexpr type.
//! @tparam Strings - target strings to be spliced
template <const std::string_view&... Strings>
struct ConcatString
{
private:
    //! @brief A sequence of characters.
    static constexpr std::array characters{
        []() constexpr noexcept
        {
            constexpr auto length = (Strings.length() + ... + 0);
            std::array<char, length + 1> result{};
            char* dst = result.data();
            ((std::copy_n(Strings.cbegin(), Strings.length(), dst), dst += Strings.length()), ...);
            result.at(length) = '\0';
            return result;
        }()};

public:
    //! @brief The splicing result. Converted from a sequence of characters.
    static constexpr std::string_view value{characters.data(), characters.size() - 1};
};
//! @brief Get the result of splicing strings.
//! @tparam Strings - target strings to be spliced
template <const std::string_view&... Strings>
static constexpr auto concatString = ConcatString<Strings...>::value;

//! @brief Check whether the target class is stateless.
//! @tparam T - type of target class
//! @return be stateless or not
template <typename T>
consteval bool isStatelessClass()
{
    return std::is_empty_v<T> || ((sizeof(T) == sizeof(void*) && std::is_polymorphic_v<T>));
}

//! @brief Check whether the target value is part of the enumeration.
//! @tparam EnumType - type of enumeration
//! @tparam Values - arguments of enumeration
template <typename EnumType, EnumType... Values>
requires std::is_enum_v<EnumType>
class EnumCheck;
//! @brief Check whether the target value is part of the enumeration.
//! @tparam EnumType - type of enumeration
template <typename EnumType>
requires std::is_enum_v<EnumType>
class EnumCheck<EnumType>
{
public:
    //! @brief Check whether it contains the value as an enumeration.
    //! @tparam IntType - type of integral
    //! @return has or not
    template <typename IntType>
    static constexpr bool has(const IntType /*val*/)
    {
        return false;
    }
};
//! @brief Check whether the target value is part of the enumeration.
//! @tparam EnumType - type of enumeration
//! @tparam Value - current value
//! @tparam Next - next enumeration value
template <typename EnumType, EnumType Value, EnumType... Next>
requires std::is_enum_v<EnumType>
class EnumCheck<EnumType, Value, Next...> : private EnumCheck<EnumType, Next...>
{
public:
    //! @brief Check whether it contains the value as an enumeration.
    //! @tparam IntType - type of integral
    //! @param val - target value
    //! @return has or not
    template <typename IntType>
    static constexpr bool has(const IntType val)
    {
        return (static_cast<IntType>(Value) == val) || EnumCheck<EnumType, Next...>::has(val);
    }
};

//! @brief Invoke a template callable.
//! @tparam Ts - type of instantiations with template callable
//! @tparam Func - type of callable function
//! @tparam Args - type of function arguments
//! @param func - callable function
//! @param args - function arguments
//! @return function execution
template <typename... Ts, typename Func, typename... Args>
constexpr decltype(auto) invokeCallableWith(Func&& func, Args&&... args)
{
    return std::forward<Func>(func).template operator()<Ts...>(std::forward<Args>(args)...);
}

//! @brief Closure wrapper.
//! @tparam Func - type of callable function
//! @tparam Op - type of call operator
//! @tparam toWrap - flag to indicate that further wrapping is required
template <typename Func, typename Op = decltype(&Func::operator()), bool toWrap = (sizeof(Func) > (sizeof(void*) * 2U))>
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
    static constexpr auto wrap(Clos&& closure) // NOLINT(cppcoreguidelines-missing-std-forward)
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
    static constexpr auto wrap(Clos&& closure) // NOLINT(cppcoreguidelines-missing-std-forward)
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
constexpr auto wrapClosure(Clos&& closure)
{
    return WrapClosure<std::decay_t<Clos>>::wrap(std::forward<Clos>(closure));
}

//! @brief Simple spin lock.
class SpinLock
{
public:
    //! @brief Construct a new SpinLock object.
    SpinLock() = default;
    //! @brief Destroy the SpinLock object.
    virtual ~SpinLock() = default;

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
    //! @brief Construct a new ReadWriteLock object.
    ReadWriteLock() = default;
    //! @brief Destroy the ReadWriteLock object.
    virtual ~ReadWriteLock() = default;

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
    std::atomic_uint_fast16_t reader{0};
    //! @brief Counter of writers that have acquired the exclusive lock.
    std::atomic_uint_fast16_t writer{0};
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

private:
    //! @brief Object managed by the guard.
    ReadWriteLock& lock;
    //! @brief Lock mode.
    const LockMode mode{LockMode::read};
};
} // namespace common
} // namespace utility
