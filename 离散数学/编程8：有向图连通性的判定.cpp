#include <iostream>
#include <cstring>
using namespace std;
int main()
{

    int a[100][100];
    int b[100][100];
    int c[100][100];
    int d[100][100];
    int e[100][100];
    int sum[100][100];
    int sum2[100][100];
    int count = 0, flag1 = 2, flag2 = 2, flag3 = 2;
    int n;

    cin >> count;
    n = count;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            cin >> a[i][j];
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            sum[i][j] = a[i][j];
        }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int z = 0; z < n; z++)
            {
                b[i][j] += a[i][z] * a[z][j];
            }

    count = count - 2;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            sum[i][j] = sum[i][j] + b[i][j];
        }

    // 处理成可达矩阵
    while (count != 0)
    {

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int z = 0; z < n; z++)
                {
                    c[i][j] += b[i][z] * a[z][j];
                }
        count = count - 1;

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                sum[i][j] = sum[i][j] + c[i][j];
            }

        memcpy(b, c, sizeof(c));
        memset(c, 0, sizeof(c));
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (sum[i][j] != 0)
                sum[i][j] = 1; // 此时矩阵为可达矩阵
        }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (sum[i][j] != 1)
                flag1 = 0; // 不是强连通
        }

    if (flag1 != 0)
        flag1 = 1;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (i != j)
            {
                if (sum[i][j] + sum[j][i] <= 0)
                    flag2 = 0; // 不是单向连通
            }
        }

    if (flag2 != 0)
        flag2 = 1;

    // 判断弱连通情况

    if (flag1 == 0 && flag2 == 0)
    {
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                if (i != j)
                {
                    if (a[i][j] == 1 || a[j][i] == 1)
                    {
                        a[i][j] = a[j][i] = 1; // 新的邻接矩阵
                    }
                }
            }

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                sum2[i][j] = a[i][j];
            }

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int z = 0; z < n; z++)
                {
                    d[i][j] += a[i][z] * a[z][j];
                }

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                sum2[i][j] = sum2[i][j] + d[i][j];
            }

        count = n;
        count = count - 2;

        // 处理成可达矩阵
        while (count != 0)
        {

            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    for (int z = 0; z < n; z++)
                    {
                        e[i][j] += d[i][z] * a[z][j];
                    }
            count = count - 1;

            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                {
                    sum2[i][j] = sum2[i][j] + e[i][j];
                }

            memcpy(d, e, sizeof(e));
            memset(e, 0, sizeof(e));
        }

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                if (sum2[i][j] != 0)
                    sum2[i][j] = 1; // 此时矩阵为可达矩阵
            }

        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
            {
                if (i != j)
                {
                    if (sum2[i][j] != 1)
                        flag3 = 0; // 不是弱连通
                }
            }

        if (flag3 != 0)
            flag3 = 1;
    }

    if (flag1 == 1)
        cout << "A" << endl;
    else if (flag1 == 0 && flag2 == 1)
        cout << "B" << endl;
    else if (flag1 == 0 && flag2 == 0 && flag3 == 1)
        cout << "C" << endl;
    // else if(flag1==0&&flag2==0&&flag3==0)
    // cout<<"该图是不连通的";
    return 0;
}