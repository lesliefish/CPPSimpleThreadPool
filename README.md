# CPPSimpleThreadPool

使用方法参考：
#include "ThreadPool.h"
#include <iostream>

using namespace std;
int main()
{
	ThreadPool pool;
	pool.start(4);
	for (size_t i = 0; i < 1024*1024; i++)
	{
		pool.addTask([=]()
		{
			cout << "i value is : " << i << endl;
		});
	}

    return 0;
}