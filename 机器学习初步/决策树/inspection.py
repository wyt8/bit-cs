import pandas as pd
import numpy as np
import sys

def inspect(input_file_path, output_file_path):
    # 读取 tsv 文件
    df = pd.read_csv(input_file_path, sep='\t')
    # 获取最后一列
    last_column = df.iloc[:, -1]
    # 统计各种元素值出现的次数
    counts = last_column.value_counts().values 
    # 计算各种类型出现的概率
    probabilities = counts / counts.sum()
    # 采用 majority vote 获得的错误率
    error_rate = 1 - probabilities.max()
    # 计算熵
    entropy = - np.sum(probabilities * np.log2(probabilities))

    with open(output_file_path, 'w') as output:
        output.write(f'entropy: {entropy}\n')
        output.write(f'error: {error_rate}\n')

if __name__ == '__main__':
    inspect(sys.argv[1], sys.argv[2])
