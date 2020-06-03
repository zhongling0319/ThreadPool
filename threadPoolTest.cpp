#include "threadPool.h"
#include <iostream>
#include <unistd.h>
using namespace std;

class MyTask : public Task
{
public:
	MyTask() = default;
	int Run()
	{
		cout << (char*)m_ptrData << endl;
		sleep(rand() % 5 + 1);
		return 0;
	}
	~MyTask() {}
};

int main()
{
	MyTask taskObj;
	char data[] = "Hello World!";
	taskObj.setData((void*)data);

	ThreadPool threadpool(5); //线程池大小为5
	for (int i = 0; i < 10; i++)
	{
		threadpool.AddTask(&taskObj);
	}
	while (1)
	{
		cout << "There are still " << threadpool.getTaskSize() << " tasks need to handle." << endl;
		if (threadpool.getTaskSize() == 0)
		{
			if (threadpool.StopAll() == -1)
			{
				cout << "Thread pool clear, exit." << endl;
				exit(0);
			}
		}
		sleep(1);
		cout << "1 second later..." << endl;
	}
	return 0;
}