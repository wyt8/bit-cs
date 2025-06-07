# 文档的倒排索引

使用MapReduce算法构建文档的倒排索引，将倒排索引存储在HBase中。

🔗[其他资源](https://github.com/wyt8/bit-cs)

## 📅分割文本文件脚本

```python
# 把sentences.txt文本文件放在当前目录下
# 当前文件夹下创建一个sentences_splited文件夹
f = open("sentences.txt","r")
i = 0
read_over = 1
while read_over:
    i = i + 1
    sub_f = open("sentences_splited/sentences_"+"%03d"%i,"w")
    for index in range(0,1000000):
        read_over = sub_f.write(f.readline())
    sub_f.close()
f.close()
```

## 🎮代码解释

我们采用的方案是一个`mapper`和一个`reducer`，在`map`阶段统计每行不同单词的个数，然后输入到`reduce`阶段，在`reduce`阶段将倒排索引保存到`HBase`中。

### Map阶段

在`Map`阶段需要解析输入的句子文件，输入`Mapper`的是一个键值对，`key`是一行的起始字节数，`value`是每行的内容；`Mapper`将每个单词映射为一个键值对，其中`key`是单词，`value`的格式为“行号:每行的该单词数”。

在解析每行的数据时，利用`StringTokenizer words = new StringTokenizer(line.toString())`来将这一行用空格划分开来并获得迭代器，第一个单词为行号，剩余部分利用`Map`容器来统计不同单词在这一行出现的次数。在统计结束后，遍历容器来输出数据到`Reducer`。

### Reduce阶段

在`Reduce`阶段，利用`HBase`提供的`TableReducer`来简化向`HBase`中写入数据的流程。输入`Reducer`是一个键值对，`key`是单词，`value`是从`Mapper`接受到的键为该`key`的所有值的容器。

在`Reducer`中，为每一行创建一个`Put`对象，调用它的`addColumn()`方法来向`HBase`表中的每行添加数据。

## ✒️部分错误记录与解决方案

1. hadoop jar XXX运行报错不能够找到类，日志如下：`java.lang.NoClassDefFoundError: org/apache/hadoop/hbase/filter/Filter`
   * **解决方法**：在`hadoop/etc/Hadoop/Hadoop-env.sh`中添加：`export HADOOP_CLASSPATH=<hbase路径>/hbase/lib/*`

1. `HBase`表中的数据出现了许多不期望的后缀

   * **原因**：代码中使用了`getBytes()`方法，将数据转换为字节数组，而该方法如果没有给参数，默认是按照平台的默认字符编码来转化，这就导致不同平台、不同文件编码有不同的差异。使用`HBase`中`Bytes.toBytes()`方法其实是调用了`getBytes()`方法，并且将字符编码设为UTF-8的格式。

   * **解决方法**：推荐使用`Bytes.toBytes()`方法，这样可以有效的避免因为个人文件字符编码不同而导致的问题。
