数据成员：

+ 表示false positive probility
+ 位向量的长度
+ 已经插入的项数

基本流程：

+ 初始化Bloomfilter

  + 指定错误率
  + 指定K
  + 指定长度m

  ......

+ insert：

  + K个Hash函数，产生K个position
  + 将bit vector的K个position置为1

+ query：

  + K个hash函数，产生K个position
  + 查询bit vector的K的position 结果如果全为1 那么认为存在 否则不存在



首先实现bit vector

+ 直接使用vector？——固定大小的vector





https://blog.csdn.net/zhaoyunxiang721/article/details/41123007

https://en.wikipedia.org/wiki/Bloom_filter