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
	int k;          // ��ȥ���ִ�
	int t_id;       // �߳�id
} threadParam_t;
//�ṹ�嶨��
int main() {
	// ����A��n
	// ...
	srand(time(NULL));
	for (int i = 0; i < n; i++) {
		pA[i][i] = 1.0;//�Խ���Ϊ1.0
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

	//�����Ǿ���
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

		// ���������̣߳�������ȥ����
	for (int k = 0; k < n; ++k) {
		// ���߳�����������
		for (int j = k + 1; j < n; j++) {
			pA[k][j] = pA[k][j] / pA[k][k];
		}
		pA[k][k] = 1.0;

		// ���������̣߳�������ȥ����
		int worker_count = n - 1 - k;
		//beishu = n / worker_count;// �����߳�����
		pthread_t* handles = (pthread_t*)malloc(worker_count * sizeof(pthread_t));        // ������Ӧ�� Handle
		threadParam_t* param = (threadParam_t*)malloc(worker_count * sizeof(threadParam_t));  // ������Ӧ���߳����ݽṹ
		//������n+1-----k
		// ��������
		for (int t_id = 0; t_id < worker_count; t_id++) {
			param[t_id].k = k;
			param[t_id].t_id = t_id;
		}//4���߳�

		// �����߳�
		for (int t_id = 0; t_id < worker_count; t_id++) {
			pthread_create(&handles[t_id], NULL, threadFunc, &param[t_id]);
		}

		// ���̹߳���ȴ����еĹ����߳���ɴ�����ȥ����
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


	cout << "pthread�㷨����Ч��" << endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			cout << pA[i][j] << " ";
			if (j == n - 1) {
				cout << endl;
			}
		}
	}

	cout << "�����㷨����Ч��" << endl;
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
	int k = p->k;           // ��ȥ���ִ�
	int t_id = p->t_id;     // �̱߳��
	int i = k + t_id + 1;   // ��ȡ�Լ��ļ�������

	for (int j = k + 1; j < n; ++j) {
		pthread_mutex_lock(&mutex);
		pA[i][j] = pA[i][j] - pA[i][k] * pA[k][j];
		pthread_mutex_unlock(&mutex);
	}
	pA[i][k] = 0;
	pthread_exit(NULL);
}
