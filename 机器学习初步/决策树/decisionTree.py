import pandas as pd
import numpy as np
import sys


def cal_entropy(data: pd.DataFrame):
    """ 
    计算给定样本的熵
    """
    # 获取最后一列
    last_column = data.iloc[:, -1]
    # 统计各种元素值出现的次数
    counts = last_column.value_counts().values 
    # 计算各种类型出现的概率
    probabilities = counts / counts.sum()
    # 计算熵
    entropy = - np.sum(probabilities * np.log2(probabilities))
    return entropy


def split_data(data: pd.DataFrame, feature: str) -> list:
    """
    根据某一特征将数据划分为两部分
    """
    # 按照某一列进行分组
    groups = data.groupby(feature)
    res = []
    for value, group in groups:
        res.append((value, group))
    return res


def cal_information_gain(data: pd.DataFrame, feature: str):
    """ 
    计算信息增益，也就是作业中的 mutual information 
    feature: 进行分类的特征
    """
    parent_entropy = cal_entropy(data)
    parent_num = data.shape[0]
    
    IG = parent_entropy
    for value, child_data in split_data(data, feature):
        child_num = child_data.shape[0]
        IG -=  child_num / parent_num * cal_entropy(child_data)
    return IG


def get_feature_to_split(data: pd.DataFrame) -> str:
    """
    获取当前样本集下的最优划分特征
    注意：如果 IG 都小于0，则返回 *空串*；相同 IG 将返回第一个特征
    """
    IG = 0
    res = ''
    for feature in data.columns.values[:-1]:
        # print(f"***{feature}***{cal_information_gain(data, feature)}***")
        if cal_information_gain(data, feature) > IG:
           IG = cal_information_gain(data, feature)
           res = feature 
    
    return res


def get_major_label(data: pd.DataFrame) -> str:
    """
    对于决策树的叶节点采用 majority vote 来获取标签值
    注意：如果数目相等则按字典序逆序返回
    """
    # 获取最后一列
    last_column = data.iloc[:, -1]
    # 统计各种标签值出现的次数，value_counts()默认按降序排序
    counts = last_column.value_counts()

    right = 0
    # 找到和出现次数最多的多个标签，right表示右开区间
    for count in counts:
        if count == counts.iloc[0]:
            right += 1

    return counts.index[:right].max()


def create_decision_tree(data: pd.DataFrame, labels: pd.Series, max_depth: int, current_depth: int = 0):
    """
    生成决策树
    这里的labels参数是为了打印决策树而设置的，如果只是创建决策树并不需要该参数
    """
    if current_depth == 0:
        data_counts = data.iloc[:, -1].value_counts().sort_index()
        labels_counts = labels.value_counts().sort_index()
        label_1 = labels_counts.index[0]
        label_2 = labels_counts.index[1]
        print(f"\n[{data_counts.get(label_1, 0)} {label_1}/{data_counts.get(label_2, 0)} {label_2}]")

    if current_depth >= max_depth:
        # 当前深度大于最大深度则递归结束
        return get_major_label(data)

    # 选择要拆分而使用的特征
    feature = get_feature_to_split(data)
    if feature == '':
        # 如果 IG 为 0 则递归结束
        return get_major_label(data)
    
    # 使用字典来存储决策树，{'特征': {'特征值1': {...}, '特征值2': {...}}}
    decision_tree = {feature : {}}

    for value, sub_data in split_data(data, feature):
        print(f"{(current_depth+1)*"| "}{feature} = {value}: ", end="")
        # 统计data各种元素值出现的次数
        data_counts = sub_data.iloc[:, -1].value_counts().sort_index()
        labels_counts = labels.value_counts().sort_index()
        label_1 = labels_counts.index[0]
        label_2 = labels_counts.index[1]
        print(f"[{data_counts.get(label_1, 0)} {label_1}/{data_counts.get(label_2, 0)} {label_2}]")
        # 递归生成决策树
        decision_tree[feature][value] = create_decision_tree(sub_data, labels, max_depth, current_depth + 1)

    return decision_tree


def predict(sample: list, feature_names: list, decision_tree: dict | str) -> str:
    """ 
    根据决策树获得 *单个* 样本的标签值
    """
    if type(decision_tree) == str:
        # 如果决策树的高度为0，则直接返回
        return decision_tree
    
    root_feature = list(decision_tree.keys())[0]
    root_dict = decision_tree[root_feature]

    for value in root_dict.keys():
        if sample[feature_names.index(root_feature)] == value:
            if type(root_dict) == dict: # 如果该特征值value对应的字典值是一个字典则继续递归向决策树下面的节点查询
                label = predict(sample, feature_names, root_dict[value])
            else: # 如果该特征值value对应的字典值是一个值，则该值就是要查找的标签
                label = root_dict[value]

    return label


def predict_dataset(data: pd.DataFrame, decision_tree: dict | str) -> list:
    """
    根据决策树获得 *多个* 样本的标签值
    """
    res = []
    data_feature_names = data.columns.values[:-1].tolist()
    
    for i in range(data.shape[0]):
        label = predict(data.values[i][:-1].tolist(), data_feature_names, decision_tree)
        res.append(label)

    return res


def cal_error_rate(predict_labels: list, true_labels: list):
    """
    计算错误率
    """
    equal_num = 0
    total_num = len(predict_labels)
    for i in range(total_num):
        if predict_labels[i] == true_labels[i]:
            equal_num += 1

    return 1 - equal_num / total_num

if __name__ == '__main__':
    train_input = sys.argv[1]
    test_input = sys.argv[2]
    max_depth = int(sys.argv[3])
    train_out = sys.argv[4]
    test_out = sys.argv[5]
    metrics_out = sys.argv[6]

    # 读取 tsv 文件
    train_data = pd.read_csv(train_input, sep='\t', dtype=str)
    test_data = pd.read_csv(test_input, sep='\t', dtype=str)

    decision_tree = create_decision_tree(train_data, train_data.iloc[:, -1], max_depth)

    # print(decision_tree)
    
    train_predict_labels = predict_dataset(train_data, decision_tree)
    test_predict_labels = predict_dataset(test_data, decision_tree)

    with open(train_out, 'w') as output:
        for label in train_predict_labels:
            output.write(f'{label}\n')

    with open(test_out, 'w') as output:
        for label in test_predict_labels:
            output.write(f'{label}\n')
    
    train_error_rate = cal_error_rate(train_predict_labels, train_data.values[:, -1].tolist())
    test_error_rate = cal_error_rate(test_predict_labels, test_data.values[:, -1].tolist())
    with open(metrics_out, 'w') as output:
        output.write(f'error(train): {train_error_rate}\n')
        output.write(f'error(test): {test_error_rate}\n')
