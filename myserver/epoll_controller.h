#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <vector>
#include <assert.h>
#include <fcntl.h>

class EpollController
{
public:
    EpollController(int max_ = 10000) : events(max_), epollFd(epoll_create(500))
    {
        assert(events.size() > 0 && epollFd >= 0);
    }
    // 添加文件描述符
    bool addFd(int fd, uint32_t event_)
    {
        if (fd <= 0)
            return false;

        epoll_event event;
        event.data.fd = fd;
        event.events = event_;
        return 0 == epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    }
    // 删除文件描述符
    bool delFd(int fd)
    {
        if (fd <= 0)
            return false;
        return 0 == epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
    }

    bool modFd(int fd, uint32_t event_)
    {
        if (fd <= 0)
            return false;

        epoll_event event;
        event.data.fd = fd;
        event.events = event_;
        return epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event);
    }

    int Wait(int timeout)
    {
        return epoll_wait(epollFd, &events[0], static_cast<int>(events.size()), timeout);
    }

    uint32_t getEvent(int i) const
    {
        assert(i < events.size() && i >= 0);
        return events[i].events;
    }

    int getFd(int i) const
    {
        assert(i < events.size() && i >= 0);
        return events[i].data.fd;
    }

private:
    std::vector<struct epoll_event> events;
    int epollFd;
};