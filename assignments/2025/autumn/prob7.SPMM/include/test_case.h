
//测试稀疏与稠密矩阵之间的互相转换
void test_converter();
//测试生成随机矩阵
void test_generator();

void test_spmm_cpu(const int m, const int n, const int k,const int test_time,const double sparsity);