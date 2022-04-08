#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <iostream>
#include "./block_queue.h"
class Log
{
public:
    static Log *get_instance()
    {
        static Log instance;
        return &instance;
    }
    static void *flush_log_thread(void *arg)
    {
        Log::get_instance()->async_write_log();
    }
    //可选择的参数有日志文件、日志缓冲区大小、最大行数以及最长日志条队列
    bool init(const char* file_name,int log_buf_size = 8192,int split_lines = 5000000,int max_queue_size = 0);
    
    void write_log(int level,const char *format,...);

    void flush(void);
    
    
private:
    Log();

    //条款07：为多态基类声明virtual析构函数；
    virtual ~Log();

    void *async_write_log()
    {
        string single_log;
        while(m_log_queue->pop(single_log))
        {
            m_mutex.lock();
            fputs(single_log.c_str(),m_fp);
            m_mutex.unlock();
        }
    }
private:

    char log_name[128];//log文件名；
    char dir_name[128];//路径名
    int m_log_buf_size;
    int m_split_lines;
    int m_today;   //因为按天分类，记录当前时间是哪一天
    block_queue<string> *m_log_queue;
    locker m_mutex;
    FILE *m_fp;//打开log的文件指针；
    char *m_buf;
    
    int m_count;
    bool m_is_asyns;
};

#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) Log::get_instance()->write_log(1, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) Log::get_instance()->write_log(2, format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) Log::get_instance()->write_log(3, format, ##__VA_ARGS__)

#endif
