#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <system_error>
#include <unistd.h>
#include <unordered_set>

namespace bashpp::wrappers {
    namespace details {
        inline int open(const char *path, int oflag, mode_t mode) {
            if (oflag & O_CREAT) {
                return ::open(path, oflag, mode);
            } else {
                return ::open(path, oflag);
            }
        }

        inline int fcntlSetFL(int fd, int arg) {
            return ::fcntl(fd, F_SETFL, arg);
        }
        inline int fcntlGetFL(int fd) {
            return ::fcntl(fd, F_GETFL);
        }

        inline int fcntlSetFD(int fd, int arg) {
            return ::fcntl(fd, F_SETFD, arg);
        }
        inline int fcntlGetFD(int fd) {
            return ::fcntl(fd, F_GETFD);
        }

        struct NoThrow {};

        struct Allow {
            std::unordered_set<int> errors;
        };

        template<typename RetType, typename... ArgTypes>
        class Wrapper {
            using funptr_t = RetType (*)(ArgTypes...);

            funptr_t fun_;

        public:
            constexpr explicit Wrapper(funptr_t fun) : fun_{fun} {}

            RetType operator()(ArgTypes... args) const {
                int ret = fun_(args...);
                if ((ret != -1) || (errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                    return ret;
                }
                throw std::system_error{errno, std::system_category()};
            }

            RetType operator()(NoThrow, ArgTypes... args) const {
                return fun_(args...);
            }

            RetType operator()(const Allow &allowed, ArgTypes... args) const {
                int ret = fun_(args...);
                if ((ret != -1) || (allowed.errors.contains(errno))) {
                    return ret;
                }
                throw std::system_error{errno, std::system_category()};
            }
        };
    }// namespace details

    using NoThrow = details::NoThrow;
    using Allow = details::Allow;

    constexpr details::Wrapper open{details::open};
    constexpr details::Wrapper unlink{::unlink};
    constexpr details::Wrapper close{::close};
    constexpr details::Wrapper read{::read};
    constexpr details::Wrapper write{::write};
    constexpr details::Wrapper dup{::dup};
    constexpr details::Wrapper dup2{::dup2};
    constexpr details::Wrapper pipe{::pipe};
    constexpr details::Wrapper fork{::fork};
    constexpr details::Wrapper execve{::execve};
    constexpr details::Wrapper waitpid{::waitpid};
    constexpr details::Wrapper lseek{::lseek};
    constexpr details::Wrapper fcntlSetFL{details::fcntlSetFL};
    constexpr details::Wrapper fcntlGetFL{details::fcntlGetFL};
    constexpr details::Wrapper fcntlSetFD{details::fcntlSetFD};
    constexpr details::Wrapper fcntlGetFD{details::fcntlGetFD};
}// namespace bashpp::wrappers
