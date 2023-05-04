#include <pthread.h>
#include <iostream>
#include<time.h>
using namespace std;
int n = 7;
float pA[7][7];
float A[7][7];
int beishu = 0;
pthread_mutex_t mutex;
void* threadFunc(void* param);
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
typedef struct {
	int k;          // 消去的轮次
	int t_id;       // 线程id
} threadParam_t;
//结构体定义
int main() {
	// 读入A和n
	// ...
	srand(time(NULL));
	for (int i = 0; i < n; i++) {
		pA[i][i] = 1.0;//对角线为1.0
		for (int j = 0; j < n; j++) {
			if (j > i)pA[i][j] = rand() % 10;
			else if (j < i)pA[i][j] = 0;
		}
	}


	for (int k = 0; k < n; k++) {
		for (int i = k + 1; i < n; i++) {
			for (int j = 0; j < n; j++) {
				pA[i][j] += pA[k][j];
			}
		}
	}

	//上三角矩阵
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			A[i][j] = pA[i][j];
		}
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << pA[i][j] << " ";
			if (j == n - 1) {
				cout << endl;
			}
		}
	}
	//return 0;

		// 创建工作线程，进行消去操作
	for (int k = 0; k < n; ++k) {
		// 主线程做除法操作
		for (int j = k + 1; j < n; j++) {
			pA[k][j] = pA[k][j] / pA[k][k];
		}
		pA[k][k] = 1.0;

		// 创建工作线程，进行消去操作
		int worker_count = n - 1 - k;
		//beishu = n / worker_count;// 工作线程数量
		pthread_t* handles = (pthread_t*)malloc(worker_count * sizeof(pthread_t));        // 创建对应的 Handle
		threadParam_t* param = (threadParam_t*)malloc(worker_count * sizeof(threadParam_t));  // 创建对应的线程数据结构
		//后续有n+1-----k
		// 分配任务
		for (int t_id = 0; t_id < worker_count; t_id++) {
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}//4个线程

		// 创建线程
		for (int t_id = 0; t_id < worker_count; t_id++) {
			pthread_create(&handles[t_id], NULL, threadFunc, &param[t_id]);
		}

		// 主线程挂起等待所有的工作线程完成此轮消去工作
		for (int t_id = 0; t_id < worker_count; t_id++) {
			pthread_join(handles[t_id], NULL);
		}
		free(handles);
		free(param);
	}


	for (int k = 0; k < n; k++) {
		for (int j = k + 1; j < n; j++) {
			A[k][j] = A[k][j] / A[k][k];
		}
		A[k][k] = 1.0;
		for (int i = k + 1; i < n; i++) {
			for (int j = k + 1; j < n; j++) {
				A[i][j] = A[i][j] - A[k][j] * A[i][k];
			}
			A[i][k] = 0;
		}
	}


	cout << "pthread算法运行效果" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << pA[i][j] << " ";
			if (j == n - 1) {
				cout << endl;
			}
		}
	}

	cout << "串行算法运行效果" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << A[i][j] << " ";
			if (j == n - 1) {
				cout << endl;
			}
		}
	}
	return 0;
}
void* threadFunc(void* param) {
	threadParam_t* p = (threadParam_t*)param;
	int k = p->k;           // 消去的轮次
	int t_id = p->t_id;     // 线程编号
	int i = k + t_id + 1;   // 获取自己的计算任务

	for (int j = k + 1; j < n; ++j) {
		pthread_mutex_lock(&mutex);
		pA[i][j] = pA[i][j] - pA[i][k] * pA[k][j];
		pthread_mutex_unlock(&mutex);
	}
	pA[i][k] = 0;
	pthread_exit(NULL);
}
