#include "threadPool.h"

void Task::setData(void* data)	//设置任务数据
{
	m_ptrData = data;
}

ThreadPool::ThreadPool(int threadNum)	//构造函数，创建线程
{
	cout << threadNum << " threads create." << endl;
	for (int i = 0; i < threadNum; i++)
	{
		//调用emplace_back()把线程插入队列，线程函数用lambda
		m_threads.emplace_back([this]{
			while (1)
			{
				Task* task;
				//上锁，锁对象生命周期结束自动解锁
				{
					unique_lock<mutex> myLock(m_threadMutex);
					//如果任务队列为空，则等待
					while (!m_shutdown && m_tasks.empty())
					{
						m_threadCon.wait(myLock);
					}

					//关闭线程
					if (m_shutdown)
					{
						return;
					}

					//从队列中取出一个任务
					task = m_tasks.front();
					m_tasks.pop();
				}

				//执行任务
				cout << "tid: " << this_thread::get_id() << endl;
				task->Run();
			}
		});
	}
}

int ThreadPool::AddTask(Task* task)	//把任务添加到队列中
{
	{
		unique_lock<mutex> myLock(m_threadMutex);
		m_tasks.push(task);
	}
	m_threadCon.notify_one();	//通知一个线程
	
	return 0;
}

int ThreadPool::StopAll()				//使线程池中所有线程退出
{
	if (m_shutdown)	//避免重复调用
	{
		return -1;
	}
	cout << "All threads will be ended." << endl;
	{
		unique_lock<mutex> myLock(m_threadMutex);
		m_shutdown = true;
	}
	m_threadCon.notify_all();	//唤醒所有线程，线程池要销毁了
	for (thread &thr : m_threads)	//清理僵尸线程
	{
		thr.join();
	}
	return 0;
}

int ThreadPool::getTaskSize()			//获取当前任务数
{
	unique_lock<mutex> myLock(m_threadMutex);
	return m_tasks.size();
}

ThreadPool::~ThreadPool()				//析构函数，退出所有线程
{
	StopAll();
}