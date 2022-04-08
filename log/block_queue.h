#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include <bits/types/struct_timeval.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include "../locker.h"

using namespace std;
//锁这边还有几个函数没有进行实现；
template<class T>
class block_queue
{
public:
    block_queue(int max_size = 1000)
    {
        if(max_size<=0) exit(-1);
        m_max_size=max_size;
        m_array=new T[max_size];
        m_size=0;
        m_front=-1;
        m_back=-1;
    }
    void clear()
    {
        m_mutex.lock();
        m_size = 0;
        m_front = -1;
        m_back = -1;
        m_mutex.unlock();
    }
    ~block_queue()
    {
        m_mutex.lock();
        if(m_array != nullptr)//c++11
        {
           delete [] m_array; 
        }
        m_mutex.unlock();
    }
    bool full()
    {
        m_mutex.lock();
        if(m_size>=m_max_size)
        {
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    bool empty()
    {
        m_mutex.lock();
        if(m_size<=0){
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }
    //返回队首元素
    bool front(T &value) 
    {
        m_mutex.lock();
        if (0 == m_size)
        {
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_front];
        m_mutex.unlock();
        return true;
    }
    //返回队尾元素
    bool back(T &value) 
    {
        m_mutex.lock();
        if (0 == m_size)
        {
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_back];
        m_mutex.unlock();
        return true;
    }

    int size() 
    {
        int tmp = 0;

        m_mutex.lock();
        tmp = m_size;

        m_mutex.unlock();
        return tmp;
    }

    int max_size()
    {
        int tmp = 0;

        m_mutex.lock();
        tmp = m_max_size;

        m_mutex.unlock();
        return tmp;
    }
    //往队列添加元素，需要将所有使用队列的线程先唤醒
    //当有元素push进队列,相当于生产者生产了一个元素
    //若当前没有线程等待条件变量,则唤醒无意义
    int push(const T&item)
    {
        m_mutex.lock();
        if(m_size>=m_max_size)
        {
            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }
        
        m_back=(m_back+1)%m_max_size;//m_back???
        m_array[m_back]=item;

        m_size++;

        m_cond.broadcast();//pthread_cond_broadcast(&m_cond)唤醒等待该条件的所有进程
        m_mutex.unlock();
        return true;    
    }
    
    bool pop(T& item)
    {
        m_mutex.lock();
        while(m_size<=0)//为什么不是if而是while
        {
            if(!m_cond.wait(m_mutex.get()))
            {
                m_mutex.unlock();
                return false;
            }
        }

        m_front = (m_front+1)%m_max_size;
        item=m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }
    
    bool pop(T& item,int ms_timeout)
    {
        struct timespec t={0,0};
        struct timeval now={0,0};
        gettimeofday(&now,NULL);
        m_mutex.lock();
        if (m_size <= 0)
        {
            t.tv_sec = now.tv_sec + ms_timeout / 1000;
            t.tv_nsec = (ms_timeout % 1000) * 1000;
            if (!m_cond.timewait(m_mutex.get(), t))//timewait(xxx,t)
            {
                m_mutex.unlock();
                return false;
            }
        }

        if (m_size <= 0)
        {
            m_mutex.unlock();
            return false;
        }
        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_size--;
        m_mutex.unlock();
        return true;
    }

private:
    
    locker m_mutex;
    cond m_cond;
    int m_max_size;
    T* m_array;
    int m_size;
    int m_front;
    int m_back;
};



#endif
