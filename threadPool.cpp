#include "threadPool.h"

void Task::setData(void* data)	//������������
{
	m_ptrData = data;
}

ThreadPool::ThreadPool(int threadNum)	//���캯���������߳�
{
	cout << threadNum << " threads create." << endl;
	for (int i = 0; i < threadNum; i++)
	{
		//����emplace_back()���̲߳�����У��̺߳�����lambda
		m_threads.emplace_back([this]{
			while (1)
			{
				Task* task;
				//�������������������ڽ����Զ�����
				{
					unique_lock<mutex> myLock(m_threadMutex);
					//����������Ϊ�գ���ȴ�
					while (!m_shutdown && m_tasks.empty())
					{
						m_threadCon.wait(myLock);
					}

					//�ر��߳�
					if (m_shutdown)
					{
						return;
					}

					//�Ӷ�����ȡ��һ������
					task = m_tasks.front();
					m_tasks.pop();
				}

				//ִ������
				cout << "tid: " << this_thread::get_id() << endl;
				task->Run();
			}
		});
	}
}

int ThreadPool::AddTask(Task* task)	//��������ӵ�������
{
	{
		unique_lock<mutex> myLock(m_threadMutex);
		m_tasks.push(task);
	}
	m_threadCon.notify_one();	//֪ͨһ���߳�
	
	return 0;
}

int ThreadPool::StopAll()				//ʹ�̳߳��������߳��˳�
{
	if (m_shutdown)	//�����ظ�����
	{
		return -1;
	}
	cout << "All threads will be ended." << endl;
	{
		unique_lock<mutex> myLock(m_threadMutex);
		m_shutdown = true;
	}
	m_threadCon.notify_all();	//���������̣߳��̳߳�Ҫ������
	for (thread &thr : m_threads)	//����ʬ�߳�
	{
		thr.join();
	}
	return 0;
}

int ThreadPool::getTaskSize()			//��ȡ��ǰ������
{
	unique_lock<mutex> myLock(m_threadMutex);
	return m_tasks.size();
}

ThreadPool::~ThreadPool()				//�����������˳������߳�
{
	StopAll();
}