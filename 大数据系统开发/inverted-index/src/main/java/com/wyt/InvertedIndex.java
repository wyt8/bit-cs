package com.wyt;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.client.Mutation;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.io.ImmutableBytesWritable;
import org.apache.hadoop.hbase.mapreduce.TableMapReduceUtil;
import org.apache.hadoop.hbase.mapreduce.TableReducer;
import org.apache.hadoop.hbase.util.Bytes;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;

import java.io.IOException;
import java.util.HashMap;
import java.util.StringTokenizer;

public class InvertedIndex {

    // 要写入的HBase表名
    private static final String tableName = "invertedindex";
    // 要写入的HBase的列族名
    private static final String colFamilyName = "res";

    public static void main(String[] args) throws Exception {
        Configuration conf = HBaseConfiguration.create();

        Job job = Job.getInstance(conf, "inverted index");
        // 设置执行作业的类，就是main函数所在的类
        job.setJarByClass(InvertedIndex.class);
        // 设置Mapper类
        job.setMapperClass(Map.class);
        // 设置Reducer类，TableMapReduceUtil是hbase提供的一个工具类，可以方便地向hbase中写入数据
        TableMapReduceUtil.initTableReducerJob(
                tableName,
                Reduce.class,
                job
        );
        // 设置Mapper的输出的键值对的类型
        job.setMapOutputKeyClass(Text.class);
        job.setMapOutputValueClass(Text.class);
//        // 设置reduce的个数，默认为1，太大会暴内存
//        job.setNumReduceTasks(3);

        // 运行jar程序的第一个参数即为要进行倒排索引的文件目录名
        FileInputFormat.setInputPaths(job, new Path(args[0]));
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }

    // 基类Mapper的前两个参数为输入的键、值，后两个参数为输出的键、值
    public static class Map extends Mapper<LongWritable, Text, Text, Text> {
        // 提高作用域，防止反复创建对象造成运行速度减慢
        private final Text wordKey = new Text(); //
        private final Text wordValue = new Text();
        private final HashMap<String, Integer> wordNumMap = new HashMap<>();

        @Override
        // 重写基类的map方法：第一个参数key表示一行的起始字节数，第二个参数表示一行字符串的内容，context是map函数的输出，即一系列的键值对
        protected void map(LongWritable keyIn, Text line, Mapper<LongWritable, Text, Text, Text>.Context context)
                throws IOException, InterruptedException {
            // StringTokenizer将一行文本用空格划
            StringTokenizer words = new StringTokenizer(line.toString());
            String lineIndex = words.nextToken(); // 行号

            // 清空上一次统计的单词数目信息
            wordNumMap.clear();
            String word;
            // 利用Map容器来统计一行中各个单词的数目
            while(words.hasMoreElements()) {
                word = words.nextToken();
                if(wordNumMap.containsKey(word)) {
                    wordNumMap.put(word, wordNumMap.get(word) + 1);
                }
                else {
                    wordNumMap.put(word, 1);
                }
            }

            // 将单词数目信息输出到Reducer
            for (java.util.Map.Entry<String, Integer> entry : wordNumMap.entrySet()) {
                String key = entry.getKey();
                Integer value = entry.getValue();
                wordKey.set(key);
                wordValue.set(lineIndex + ":" + value);
                context.write(wordKey, wordValue);
            }
        }
    }

    // TableReducer是HBase提供的类，方便向HBase中写入数据
    // 基类TableReducer的泛型参数：第一个表示输入key的类型，第二个表示输入value的类型，第三个表示hbase中每行的rowKey类型
    public static class Reduce extends TableReducer<Text, Text, ImmutableBytesWritable> {
        private final ImmutableBytesWritable result = new ImmutableBytesWritable();

        @Override
        protected void reduce(Text key, Iterable<Text> values, Reducer<Text, Text, ImmutableBytesWritable, Mutation>.Context context) throws IOException, InterruptedException {
            byte[] keyByte = Bytes.toBytes(key.toString());
            Put put = new Put(keyByte);
            int i = 1;
            for(Text val: values) {
                // 参数分别为：列族名，列名，要写入cell的数据
                put.addColumn(Bytes.toBytes(colFamilyName), Bytes.toBytes(String.valueOf(i)), Bytes.toBytes(val.toString()));
                i++;
            }
            result.set(keyByte);
            // 向HBase中写入数据
            context.write(result, put);
        }
    }
}
