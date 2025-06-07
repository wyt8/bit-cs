#include<iostream>
#include<queue>
using namespace std;
const int N = 100;
class Graph
{
private:
	int n;
	int matrix[N][N];
public:
	Graph()
	{
		cin >> n;
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				cin >> matrix[i][j];
			}
		}
	}
	void bfs()
	{
		int color[N] = { 0 };
		int visit[N] = { 0 };
		color[0] = 1;
		visit[0] = 1;
		queue<int>q;
		q.push(0);
		while (!q.empty())
		{
			int t = q.front();
			q.pop();
			for (int i = 0; i < n; i++)
			{
				if (matrix[t][i] > 0)
				{
					if (color[t] == color[i] || t == i)
					{
						cout << "no" << endl;
						return;
					}
					else if (color[i] == 0)
					{
						color[i] = -color[t];
						q.push(i);
						visit[i] = 1;
					}
				}
			}
			if (q.empty())
			{
				for (int i = 1; i < n; i++)
				{
					if (visit[i] == 0)
					{
						q.push(i);
						color[i] = 1;
						visit[i] = 1;
						break;
					}
				}
			}
		}
		cout << "yes" << endl;
	}
};
int main()
{
	Graph G;
	G.bfs();
	return 0;
}