#include<iostream>
#include<ctime>
#include<cstdlib>
#include<vector>
#include<string>
#include<algorithm>
#include<limits>
#include <iomanip>  // 引入输入输出格式控制库
#include <chrono> // 时间库
#include <thread> // 跨平台兼容的多线程库

using namespace std;

// 定义全局常量
#define n 10        // 已分配区表最大条目数
#define m 10        // 空闲区表最大条目数
#define L 1048576   // 总内存大小1MB（1024*1024）

// 作业类
class Works {
public:
    // 构造函数
    Works() {
        srand(time(0));  // 初始化随机种子，使用当前时间
        cout << endl << n << "个作业内存的大小随机初始化请输入R，输入其他为自定义作业信息：";
        string choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(),  '\n');//while (getchar() != '\n'); C语言风格 清空输入缓冲区
	    if(choice[0] == 'r' || choice[0] == 'R') initialize_jobs();  // 初始化作业队列
        else customize_jobs();  // 自定义作业队列
        print_jobs();  // 打印作业队列
    }
    // 作业结构
    struct Job{
        string name;    // 作业名称
        float size;     // 需求内存大小
        int flag;       // 状态标记：0-未分配 1-已分配 2-已回收
        int id;         // 作业编号
        Job() = default; // 显式定义默认构造函数 
		Job(string N, float S, int F, int i) : name(move(N)), size(S), flag(F), id(i) {}// // 移动构造函数
    };
    // 修改作业信息
	void modify_jobs(int i, const int& F = 404, const float& resize = -1, const string& rename = ""){ // 用引用传入避免不必要的拷贝 void modify_jobs(int i, string rename, float resize, int F)
		if (!rename.empty()) jobs.at(i).name = rename;
		if (resize >= 0) jobs.at(i).size = resize;
		if (F != 404) jobs.at(i).flag = F;
	}
	//寻找处于一定状态下的作业返回其下标
	vector<int> find_jobs(int targetFlag){
		state.clear();
		vector<Job>::iterator it = jobs.begin(); // 可以用自动识别类型的auto it代替iterator it(需显式声明迭代器类型（如 vector<int>::iterator），明确指定容器元素的访问方式)
    	while (true) {
        	// 使用 Lambda 表达式定义匹配条件
        	it = find_if(it, jobs.end(), 
							[targetFlag](const Job& j) {
            					return j.flag == targetFlag;
        					}
						);
        	if (it == jobs.end()) break; // 未找到跳出循环
        	// 计算当前元素的下标
        	state.push_back(distance(jobs.begin(), it));
        	++it; // 跳过已找到的元素，继续向后查找
    	}
    	return state;
	}
	// 返回作业队列或单个作业的指针可供外部修改
    vector<Job>* get_jobs() {
        return &jobs;
    }                                                                                                
    Job* get_jobs(int i) {
        return &jobs.at(i); 
    }
    // 返回作业队列或单个作业
    vector<Job> getjobs() {
        return jobs;
    }                                                                                                
    Job getjobs(int i) {
        return jobs.at(i);//return jobs[i]; 
    }
    // 打印作业队列
    void print_jobs() {
        // 遍历作业队列的每个作业
        cout << "\n作业队列：";
        if(jobs.empty()) cout << "无" << endl;
		else {
			cout << "\n编号\t作业名称\t内存需求\t状态" << endl;
        	for (int i = 0; i < jobs.size(); i++) {
        // 输出作业名称、内存需求和状态
            	cout << " " << jobs.at(i).id << "\t" << jobs.at(i).name << "\t\t" << jobs.at(i).size << "\t\t" << jobs.at(i).flag;
				switch (jobs.at(i).flag){
					case 0: cout << "（未分配）"; break;
					case 1: cout << "（已分配）"; break;
					case 2: cout << "（已回收）"; break;
					default: cout << "（未定义状态）";
				}
				cout << endl;  
        	} 
		}
    }
    // 析构函数
    ~Works() {
        jobs.clear();  // 清空作业队列
        vector<Job>().swap(jobs);  // 彻底释放内存（通过与空容器交换数据实现）
        state.clear();
        vector<int>().swap(state);
		cout << "\n作业类已销毁\n" << endl;  // 输出销毁信息
    }
private:
    vector<Job> jobs{n};  // 作业表（初始化为10个作业）提前分配至少 10 个元素的空间，避免多次扩容
    vector<int> state;
    void initialize_jobs() {//随机内存大小
        // cout << "\n将随机分配" << n << "个作业的内存需求（默认范围是1到L/2，L=1MB，即1024*1024）" << endl;
        cout << "\n将随机分配" << n << "个作业的内存需求（为实现动态内存管理的回收功能，默认范围是L/8到L*5/8 - 1，L=1MB，即1024*1024）" << endl;
        // 遍历作业队列的每个作业
        for (int i = 0; i < n; i++) {
        // 生成作业名称，格式为"JOB1"到"JOB10"
            jobs[i].name = "JOB" + to_string(i + 1);
        // 生成随机内存需求，范围是1到L/2
            jobs[i].size = (rand() % (L/2)) + L/8; // jobs[i].size = (rand() % (L/2)) + 1;  
        // 初始状态为未分配
            jobs[i].flag = 0;
        // 生成作业编号，范围是1到10
            jobs[i].id = i + 1;
        }
    }
    void customize_jobs() {//自定义作业信息
        cout << "\n将自定义若干个作业的初始信息，作业名为空字符串表示退出自定义作业" << endl << endl;
        // 遍历作业队列的每个作业
        for(int i = 0; ;i++) {
			string N;
            float size;
            int F;
        // 输入作业名称，输入空字符串表示退出自定义作业
            cout << "\n请输入作业" << i + 1 << "的名称（输入空字符串，即直接按回车表示退出）：";
            getline(cin, N); // 读取整行输入，包括空格。getline函数通常用于读取字符串，直到遇到换行符，然后丢弃换行符。所以一般情况下，使用getline之后，缓冲区里不会有残留的换行符，这可能意味着不需要再清除缓存了
            if (N.empty()) break;  // 输入空字符串退出
            if(i >= 10) jobs.emplace_back(" ", 0.0f, 0, 0);// 空间不足时直接构造，避免拷贝 string
            jobs[i].name = N;
            //cin.ignore(numeric_limits<streamsize>::max(),  '\n'); // while (getchar() != '\n'); 循环清空所有字符（每次getchar仅清除一个字符，C风格，效率低）

        // 输入内存需求
            cout << "请输入作业" << jobs[i].name << "的内存需求：";
            while(true) {
                cin >> size;
                if (cin.fail() || size < 0)  {  // 检测输入是否失败（如输入字母，负数）
                    cin.clear();    // 重置错误状态 
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清空缓冲区直至换行符 // cin.ignore();仅清空1个字符（可能残留数据）
                    cerr << "所占内存不能为字符串或负数，请重新输入：";
                } else {
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清除多余字符（如回车）
                    break;  // 输入正确，跳出循环
                }
            }
            jobs[i].size = size;  
        // 输入初始状态
            cout << "请输入作业" << jobs[i].name << "的初始状态（0-未分配，1-已分配，2-已回收，其他-未定义）：";
            while(1) {
            	cin >> F;
            	if (cin.fail())  {  // 检测输入是否失败（如输入字母）
                    cin.clear();    // 重置错误状态 
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清空缓冲区直至换行符 // cin.ignore();仅清空1个字符（可能残留数据）
                    cerr << "请重新输入一个整数：";
                } else {
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清除多余字符（如回车）
                    break;  // 输入正确，跳出循环
                }
			}
            jobs[i].flag = F;
            jobs[i].id = i + 1;
            if(i < 10) vector<Job>(jobs).swap(jobs); // 通过拷贝构造生成一个临时 vector，其容量精确等于当前元素数量（无冗余内存），再与原容器交换内容。临时容器销毁时释放原容器的冗余内存
						// jobs.shrink_to_fit(); // 释放容器多余容量（初始化为10），该代码仅表示请求，编译器不一定会释放
        }
        system("cls"); // 清屏
    }
    // 测试代码样例
    /*
int main() {
    Works w;  // 创建作业对象
    
    cout<<(*w.get_jobs())[0].name<<endl;
    cout<<w.get_jobs(0)->size<<endl;  // 输出第一个作业的名称
    
	cout<<w.getjobs()[1].name<<endl;
	cout<<w.getjobs().at(2).name<<endl;
    cout<<w.getjobs(1).size<<endl;
    
	w.get_jobs()->at(0).name = "fuck";
	(*w.get_jobs(0)).flag = 100;
	
	w.modify_jobs(1,0,200,"shit");
	w.modify_jobs(2,404,-2,"shit");
	w.modify_jobs(3,0,500);
	w.modify_jobs(4,67);
	//w.modify_jobs(4,0);
	//vector<works::Job> jobs = w.getjobs();  // 获取作业队列
    //cout<<jobs[0].name<<endl;  // 输出第一个作业的名称
	//works::Job job = w.getjobs(0);  // 获取第一个作业
    //job.flag = 1;  // 将第一个作业的状态标记为已分配
    //jobs[0].name = "asd";
    //cout<<jobs[0].name<<endl;
    
    for (const auto& elem : w.find_jobs(0)) { // 只读遍历（避免拷贝开销）
    	cout << elem << " ";
	}
    
    w.print_jobs();  // 打印作业队列
    return 0;  // 程序结束 
}
    */ 
   
};
// 已分配区类
class Used {
public:
    // 构造函数
    Used() {
    	//initialize_used_table();
    	cout << endl << "将默认设置" << n << "个已分配区" << endl;
    	print_used_table();
    }
    // 已分配区表条目结构
    struct UsedEntry{
        string name;    // 作业名称
        float address;  // 起始地址
        float length;   // 长度（字节） 
        int flag;       // 状态标记：0-空 1-已分配 其他-未定义
        int id;         // 记录作业编号
        UsedEntry() = default; // 显式定义默认构造函数	
        UsedEntry(string N, float A, float l, int F, int i) : name(move(N)), address(A), length(l), flag(F), id(i) {} // 移动构造函数
    };
    // 获取已分配区表（只读）
    vector<UsedEntry> get_used_table() const {
        return used_table;
    }
    // 修改已分配区信息
    void modify_used_table(int& i, const string& name = "", const float& a = -1, const float& l = -1, const int& I = 0, const int& F = 1){ // 用引用传入避免不必要的拷贝
        /* 左值（T&）引用：允许直接访问和修改左值，通常用于函数参数传递和返回左值
        // 右值（T&&）引用：允许直接访问和修改右值，通常用于函数参数传递和返回右值
        // 左值引用通常用于函数参数传递和返回左值，右值引用通常用于函数参数传递和返回右值
        // 左值引用可以绑定到左值（如变量、表达式等），右值引用可以绑定到右值（如临时对象、字面量等） 
        // 通过const T& 可以实现只读访问，通过T&& 可以实现移动语义，避免不必要的拷贝
1. 左值/右值的核心区别
定义：
左值：有持久内存地址的表达式（变量、数组元素）
右值：临时计算结果或字面常量（5、a+b的结果）
关键事实：
C++标准要求非常量引用必须绑定到左值（防止意外修改临时对象）
最新趋势：C++17引入"保证拷贝消除"优化临时对象性能
现实案例：
cpp
复制
void modify(int& x);  
modify(5); // 错误：5是右值  
争议：部分开发者认为编译器应自动创建临时变量（Python/Java风格），但C++坚持显式控制
2. 常量引用的特殊豁免
定义：const T&可绑定右值，延长临时对象生命周期至引用作用域结束
关键机制：
编译器隐式创建const临时副本（C++标准条款§9.4.4）
最新发展：C++20禁止常量引用绑定到volatile右值（强化类型安全）
工程意义：
cpp
复制
const std::string& s = "hello"; // 合法：创建临时string对象  
性能数据：临时对象拷贝在x86架构消耗约12-15个时钟周期（Agner Fog优化手册）
3. 现代C++的解决方案（C++11+）
右值引用：
语法T&&，专门处理临时对象
移动语义：避免深拷贝，提升性能40-60%（STL容器实测）
转发引用：
模板中的auto&&可接受任意值类别
关键应用：完美转发（std::forward）
行业应用：
Google Abseil库中absl::string_view广泛使用常量引用处理字符串
Unreal Engine禁止非常量引用参数（强制使用指针/const引用）
        */
        /*if(i >= used_table.size()) {
        	cout << "该已分配区不存在" << endl;
        	return;
        }*/
        if (i > n - 1) { // 检查索引是否超出范围
            size_t j = i + 1 - used_table.size(); // 计算超出部分的索引  size_t无符号整数类型，用于表示内存/对象大小（sizeof运算符的返回类型）
            for (int I = 0; I < j; I++) used_table.emplace_back("", 0.0f, 0.0f, 0, 0); // 空间不足时直接构造，避免拷贝 string
        }
        if (!name.empty()) used_table.at(i).name = name; // 作业名称
        if (a >= 0) used_table.at(i).address = a; // 起始地址
        if (l >= 0) used_table.at(i).length = l; // 长度
        if (I != 0) used_table.at(i).id = I; // 已分配区编号
        used_table.at(i).flag = F; // 默认状态已分配
        i++;
    }
    // 返回待回收的已分配区
    vector<UsedEntry> decide_recycle(Works& w){
        vector<UsedEntry> recycle;
        vector<int> d = decide();
        recycle.resize(d.size()); // 存储待回收的已分配区，预分配初始化
        // 遍历已分配区表的每个条目
        for (size_t i = 0; i < d.size(); i++) {
            recycle.at(i) = used_table.at(d.at(i)); // 复制已分配区信息到回收表
            w.modify_jobs(recycle.at(i).id - 1, 2); // 修改对应作业的状态为已回收
        }
        print_used_table(); // 打印已分配区表
        return recycle; // 返回待回收的已分配区
    }
    // 打印已分配区表
    void print_used_table() {
        // 遍历已分配区表的每个条目
        cout << "\n已分配区表：";
        if(used_table.empty()) cout << "无" << endl; // 输出空信息
        else {
        	cout << "\n序号\t作业名称\t起始地址\t长度\t\t状态" << endl;	
        	for (size_t i = 0; i < used_table.size(); i++) {
                cout << " " << i+1 << "\t" << used_table.at(i).name << "\t\t" << used_table.at(i).address << "\t\t" << used_table.at(i).length << "\t\t" << used_table.at(i).flag;
                switch (used_table.at(i).flag){
                	case 0: cout << "（空）"; break;
                	case 1: cout << "（已分配）"; break;
                	default: cout << "（未定义）";	
                }
                cout << endl;
        	}
        }
    }
    // 析构函数
    ~Used() {
        used_table.clear();  // 清空已分配区表
        vector<UsedEntry>().swap(used_table);  // 彻底释放内存（通过与空容器交换数据实现）
        cout << "\n已分配区类已销毁" << endl;  // 输出销毁信息
    }
private:
    vector<UsedEntry> used_table{n};  // 已分配区表
	// 初始化已分配区表
    void initialize_used_table() {
        cout << endl << "将默认设置" << n << "个已分配区" << endl;
        // 遍历已分配区表的每个条目
        for (int i = 0; i < n; i++) {
        // 初始化作业名称为空字符串
            used_table[i].name = "" ;
        // 初始地址为0 初始长度为0
            used_table[i].address = used_table[i].length = 0;
        // 初始状态为未分配
            used_table[i].flag = 0;
        // 初始编号为0
            used_table[i].id = 0; 	
        }
    }
    // 寻找处于一定状态（默认为已分配状态）下的已分配区返回其下标
    vector<int> find_used_table(int&& targetFlag = 1){
    	vector<int> state;
    	auto it = used_table.begin(); // 可以用自动识别类型的auto it代替iterator it(需显式声明迭代器类型（如 vector<int>::iterator），明确指定容器元素的访问方式)
    	while (1) {
        	// 使用 Lambda 表达式定义匹配条件
        	it = find_if(it, used_table.end(),
							[targetFlag](const UsedEntry& u) {
            					return u.flag == targetFlag;
        					}
						); // 返回第一个满足条件的元素的迭代器，若没有找到则返回end()迭代器
        	if (it == used_table.end()) break; // 未找到跳出循环
        	// 计算当前元素的下标
        	state.push_back(distance(used_table.begin(), it));
        	++it; // 跳过已找到的元素，继续向后查找
    	}
    	return state;
    }
    // 随机决定是否回收已分配区
    vector<int> decide() {
        // 可以通过两种算法实现
		/*
		vector<int> randomly_decide = find_used_table(); // 存储已分配状态的已分配区下标
        // 遍历已分配区表的每个条目
        auto it = randomly_decide.begin(); // 可以用自动识别类型的auto it代替iterator it(需显式声明迭代器类型（如 vector<int>::iterator it），明确指定容器元素的访问方式)
        while (it != randomly_decide.end()) {
            // 随机决定是否回收
            if (rand() % 2 == 0) it = randomly_decide.erase(it); // 随机数为0则不回收，删除该元素  
            //vector::erase() 返回被删除元素的下一个有效迭代器。删除元素后，后续元素前移，被删位置之后的迭代器失效。关键模式：循环中必须用返回值更新迭代器
            else ++it; // 随机数为1则回收，继续向后查找
        }
        if(randomly_decide.empty()) cout << "\n本轮随机决定无要回收的已分配区" << endl; // 输出空信息
        else {
            cout << "\n本轮随机决定回收的已分配区的序号：";
            for (auto i = randomly_decide.begin(); i != randomly_decide.end(); i++) { // 迭代器遍历
                cout << *i + 1 ; // 输出要回收的已分配区序号
                int a = *i;// 因为modify_used_table函数会自动对a自增
                modify_used_table(a, "", -1.0f, -1.0f, 0, 0); // 修改已分配区状态为空
                if (i != randomly_decide.end() - 1) cout << " "; // 输出空格
                else cout << endl;
            }
        }
        return randomly_decide; // 返回要随机回收的已分配区下标
        */
        
        vector<int> randomly_decide = find_used_table();
        for(int val: randomly_decide) { // 范围 for 循环
        	modify_used_table(val, "", -1, -1, 0, rand() % 2); // 随机修改已分配状态的已分配区状态
            val--; // 因为modify_used_table函数会自动对val自增
        }
        vector<int> decide = find_used_table(0); // 查找空状态的已分配区
        // 查找两个容器的交集部分
        // 1.先对两个容器进行排序，确保元素顺序一致
        sort(randomly_decide.begin(), randomly_decide.end());
        sort(decide.begin(), decide.end());
        // 2. 创建目标容器（预分配空间）
        vector<int> intersection; 
        intersection.resize(min(randomly_decide.size(), decide.size())); // intersection.reserve(min(randomly_decide.size(), decide.size()));
        //resize(n)：调整容器中元素数量（size），可能修改容量（capacity）。若n > size，新增元素默认初始化或指定值初始化；若n < size，尾部元素被销毁。
        //reserve(n)：仅调整容量（capacity），不改变元素数量（size）。预分配内存以避免频繁重分配。若n > capacity，容器容量增加；若n < capacity，容器容量不变。   
        // 注意：resize() 会改变容器中元素的值，而 reserve() 不会。
        //resize可能触发隐式内存分配（如新size > capacity时），耗时复杂度为𝑂(𝑛)。
        //reserve通过一次性预分配内存，将多次内存分配优化为单次（例如插入1000个元素时，避免多次指数级扩容）。
        //标准行为：reserve(n)若n < capacity，多数实现不会缩减容量（需手动调用shrink_to_fit）。 
        // 3. 使用 set_intersection 算法查找交集部分
        auto it = set_intersection(randomly_decide.begin(), randomly_decide.end(), decide.begin(), decide.end(), intersection.begin());
        //set_intersection(randomly_decide.begin(), randomly_decide.end(), decide.begin(), decide.end(), back_inserter(intersection));
        //std::set_intersection 的返回值是一个迭代器，指向目标容器中交集元素范围的末尾（即最后一个有效交集元素的下一个位置） 这意味着可以通过该迭代器确定实际写入的交集元素数量，避免处理无效数据
        // 4. 删除多余空间
        intersection.resize(it  - intersection.begin());// intersection.erase(it, intersection.end());  保留交集部分，删除多余部分
        
        if(intersection.empty()) cout << "\n没有要回收的已分配区" << endl; // 输出空信息
        else {
            cout << "\n随机决定回收的已分配区的序号：";
            for (int val: intersection) { // 范围 for 循环
                cout << val + 1 ; // 输出要回收的已分配区序号
                if (val!= intersection.back()) cout << " "; // 输出空格
                else cout << endl;
            }
        }
        return intersection; // 返回要随机回收的已分配区下标
		
    }
};
// 空闲区类
class Free {
public:
    // 构造函数
    Free() {
        cout << endl << m << "个空闲区内存块默认初始化请输入D，输入其他为自定义空闲区内存块信息：";
        string choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(),  '\n');//while (getchar()!= '\n'); C语言风格 清空输入缓冲区
	    if(choice[0] == 'd' || choice[0] == 'D') initialize_free_table();  // 初始化空闲区表
        else customize_free_table();  // 自定义空闲区表
        print_free_table();
    }	
    // 空闲区表条目结构
    struct FreeEntry{
        float address;  // 空闲区起始地址
        float length;   // 空闲区长度（字节）
        int flag;       // 状态标记：0-空 1-未分配 其他-未定义
        FreeEntry() = default; // 显式定义默认构造函数 
		FreeEntry(float a, float l, int f) : address(a), length(l), flag(f) {} // 构造函数
    };
    // 打印空闲区表
    void print_free_table() {
        // 遍历空闲区表的每个条目
        cout << "\n空闲区表：";
        if(free_table.empty()) cout << "无" << endl;
        else {
        	cout << "\n序号\t起始地址\t长度\t\t状态" << endl;
        	for (size_t i = 0; i < free_table.size(); i++) {
        // 输出空闲区的起始地址、长度和状态
            	cout << " " << i+1 << "\t" 
			//<< fixed 
				<< setprecision(7) << free_table.at(i).address << "\t\t" << free_table.at(i).length << "\t\t" << free_table.at(i).flag;
            //fixed 强制以定点十进制格式输出浮点数，避免默认的科学计数法模式
            //setprecision(0) 设置小数点后的位数为 0，隐藏小数部分（若存在），直接输出整数部分
			//setprecision(n) 设置总有效位数（非定点模式下）
            //float 类型的有效位数约为 6-7位。若数值超过此范围（如 12345678），强制输出完整整数可能导致精度丢失（如显示 12345680）
				switch (free_table.at(i).flag){
                	case 0: cout << "（空）"; break;
                	case 1: cout << "（未分配）"; break;
                	default: cout << "（未定义）";
            	}
				cout << endl;	
        	} 	
		}
    }
    bool start(Works& w, Used& u) { // 开始分配
    	float T = check_total_memory(u);
        cout << "\n初始设置的内存总量为：" << T << "字节" << endl;
        int t = 0, I = 0, start = 0; // 记录轮数
        cout << "\n请选择要演示的动态内存分配算法：" << endl
        << "1. 首次适应分配算法" << endl << "2. 循环首次适应分配算法" << endl << "3. 最优适应分配算法" << endl << "4. 最坏适应分配算法" << endl
        << "输入序号代表你的选择，输入其他字符将退出程序：";
        string choice;
        do {
            getline(cin, choice); // 读取整行输入，包括空格。getline函数通常用于读取字符串，直到遇到换行符，然后丢弃换行符。所以一般情况下，使用getline之后，缓冲区里不会有残留的换行符，这可能意味着不需要再清除缓存了
        } while(choice.empty()); // 循环直到输入非空字符串
        switch (choice[0]) { // 选择要演示的动态内存分配算法
            case '1': // 首次适应分配算法
                while(alloate_first_fit(w, u, I)) { // 首次适应分配
                    recycle_free(w, u); // 回收空闲区
                    cout << "\n第" << ++t << "轮分配完成，当前内存总量为：" << check_total_memory(u) << "字节，";
                    if(T == check_total_memory(u)) cout << "内存总量未变，程序执行效果达到预期！" << endl;// 内存总量不变
                    else cout << "内存总量发生改变，程序出错！" << endl; // 内存总量减少
                } 
                cout << "\n\n首次适应分配算法演示完毕，程序结束！\n输入任意字符退出程序或双击回车键清屏继续......";
                break;
            case '2': // 循环首次适应分配算法
                while(alloate_loop_first_fit(w, u, I, start)) { // 循环首次适应分配
                    recycle_free(w, u); // 回收空闲区
                    cout << "\n第" << ++t << "轮分配完成，当前内存总量为：" << check_total_memory(u) << "字节，";
                    if(T == check_total_memory(u)) cout << "内存总量未变，程序执行效果达到预期！" << endl;// 内存总量不变
                    else cout << "内存总量发生改变，程序出错！" << endl; // 内存总量减少
                }
                cout << "\n\n循环首次适应分配算法演示完毕，程序结束！\n输入任意字符退出程序或双击回车键清屏继续......";
                break;
            case '3': // 最优适应分配算法
                while(alloate_best_fit(w, u, I)) { // 最优适应分配
                    recycle_free(w, u); // 回收空闲区
                    cout << "\n第" << ++t << "轮分配完成，当前内存总量为：" << check_total_memory(u) << "字节，";
                    if(T == check_total_memory(u)) cout << "内存总量未变，程序执行效果达到预期！" << endl;// 内存总量不变
                    else cout << "内存总量发生改变，程序出错！" << endl; // 内存总量减少
                }
                cout << "\n\n最优适应分配算法演示完毕，程序结束！\n输入任意字符退出程序或双击回车键清屏继续......";
                break;
            case '4': // 最坏适应分配算法
                while(alloate_worst_fit(w, u, I)) { // 最坏适应分配
                    recycle_free(w, u); // 回收空闲区
                    cout << "\n第" << ++t << "轮分配完成，当前内存总量为：" << check_total_memory(u) << "字节，";
                    if(T == check_total_memory(u)) cout << "内存总量未变，程序执行效果达到预期！" << endl;// 内存总量不变
                    else cout << "内存总量发生改变，程序出错！" << endl; // 内存总量减少
                }
                cout << "\n\n最坏适应分配算法演示完毕，程序结束！\n输入任意字符退出程序或双击回车键清屏继续......";
                break;
            default: // 退出程序
                cout << "\n\n分配算法程序已退出，程序结束！" << endl;
                return false;
        }
        char c;
        cin.get(c);  // 等待用户输入
        cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清空输入缓冲区
        /*
输入流程分析：​​
​​cin.get()的作用​​
cin.get()会从输入缓冲区读取一个字符（包括回车符\n）。如果此时缓冲区为空，程序会阻塞并等待用户输入。用户输入回车键后，cin.get()会读取这个回车符，​​但不会清除缓冲区中的换行符​​（换行符仍保留在缓冲区中）。
​​cin.ignore()的作用​​
cin.ignore(numeric_limits<streamsize>::max(), '\n')会从缓冲区中忽略所有字符，直到遇到换行符\n或达到最大字符数。如果缓冲区中已有换行符（例如用户第一次按回车键时输入的），ignore()会立即清除该换行符；如果缓冲区为空，它会等待用户输入新的字符，直到用户再次输入回车键。
​​具体执行步骤​​：
​​第一次回车​​
用户按下第一次回车键后：
cin.get()读取换行符\n，程序继续执行。
但​​缓冲区中仍保留换行符​​（因为cin.get()仅读取一个字符，但未清除后续字符）。
​​第二次回车​​
用户按下第二次回车键后：
cin.ignore()检测到缓冲区中的第一个换行符（来自第一次输入）并清除它。
如果此时缓冲区仍为空（例如用户仅按了一次回车），ignore()会继续等待输入，直到用户再次按下回车键以产生新的换行符。
​​总结​​
​​最少需要按两次回车键​​：第一次触发cin.get()，第二次触发cin.ignore()清除缓冲区中的换行符。
如果用户仅按一次回车键，cin.ignore()会因缓冲区中无换行符而继续等待输入，程序无法继续。
特性	      cin.get()	            cin.getline()
​​终止符处理​​	 保留在缓冲区	            从缓冲区移除
​​返回值类型​​	 istream&（支持链式调用）	istream&
​​缓冲区溢出保护​​	自动添加 '\0'	       自动添加 '\0'
​​适用场景​​   需保留终止符或精确控制读取范围	需完全清除终止符（如整行输入）
        */
        if(c == '\n') return true;
        else return false; // 退出程序
    }
    // 析构函数
    ~Free() {
        free_table.clear();  // 清空空闲区表
        vector<FreeEntry>().swap(free_table);  // 彻底释放内存（通过与空容器交换数据实现）
        cout << "\n空闲区类已销毁" << endl;  // 输出销毁信息
    }
private:
    vector<FreeEntry> free_table;  // 空闲区表
    // 初始化空闲区表
    void initialize_free_table() {
        cout << "\n将默认折半分配1M的总内存成" << m << "个内存块，起始地址默认为0" << endl;
    	free_table.resize(m);  // 调整空闲区表大小为m个条目
        float sizes[] = {L/512, L/256, L/128, L/64, L/32, L/16, L/8, L/4, L/2, L/512}; // 定义不同大小的内存块数组
        float current_address = 0; // 当前内存地址初始化为0
        // 遍历空闲区表的每个条目
        for (int i = 0; i < m; i++) {
        // 设置空闲区的起始地址
            free_table[i].address = current_address;
        // 设置空闲区的长度
            free_table[i].length = sizes[i];
        // 标记为未分配
            free_table[i].flag = 1;  
        // 更新当前内存地址
            current_address += sizes[i];  
        }
    }
    // 自定义空闲区表
    void customize_free_table() {
        cout << "\n将自定义若干个空闲区的初始信息，起始地址默认为0，空闲区长度为字符（串）表示退出自定义空闲区" << endl << endl; 
        float a = 0; // 当前内存地址初始化为0
        // 遍历空闲区表的每个条目
        int i = 0;
        while(true) {
            float l;
            int F;
            cout << "\n请输入第" << i+1 <<"个空闲区的长度：";
            while(1) {
                cin >> l;
                if (cin.fail()) {  // 检测输入是否失败（如输入字母）
                    cin.clear();    // 重置错误状态 
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清空缓冲区直至换行符 // cin.ignore();仅清空1个字符（可能残留数据）
                    i = -1;
                    goto T;
                } else if (l < 0) {
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清除多余字符（如回车）
                    cerr << "空闲区长度不能为负数，请重新输入：";	
                } else {
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清除多余字符（如回车）
                    break;  // 输入正确，跳出循环
                }
            }
            T:
            if(i == -1) break;
            cout << "请输入该空闲区的状态（0-空，1-未分配，其他-未定义）：";
            while(1) {
            	cin >> F;
            	if (cin.fail())  {  // 检测输入是否失败（如输入字母）
                    cin.clear();    // 重置错误状态
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清空缓冲区直至换行符 // cin.ignore();仅清空1个字符（可能残留数据）
                    cerr << "请重新输入一个整数：";
                } else {
                    cin.ignore(numeric_limits<streamsize>::max(),  '\n');  // 清除多余字符（如回车）
                    break;  // 输入正确，跳出循环
                }
            }
            free_table.emplace_back(a, l, F); // 直接构造，避免拷贝
            a += l; // 更新当前内存地址
            i++;
        }
    }
    // 检查总内存是否完整
    float check_total_memory(Used& u) {
        float total_memory = 0; // 初始化总内存
        // 遍历空闲区表的每个条目
        for (size_t i = 0; i < free_table.size(); i++) {
        // 累加空闲区的长度
            if(free_table.at(i).flag != 0) total_memory += free_table.at(i).length;
        }
        // 遍历已分配区表的每个条目
        for (size_t i = 0; i < u.get_used_table().size(); i++) {
        // 累加已分配区的长度
            if(u.get_used_table().at(i).flag == 1) total_memory += u.get_used_table().at(i).length;	
        }
        return total_memory; // 返回总内存
    }
    // 寻找最大空闲区
    int find_max_free() {
        int max_index = -1; // 初始化最大空闲区索引为-1
        float max_length = -1; // 初始化最大空闲区长度为-1，万一最后可分配的空闲区长度都为0，也可以正确找到，确保不会返回-1
        // 遍历空闲区表的每个条目
        for (size_t i = 0; i < free_table.size(); i++) {
        // 检查空闲区是否未分配且长度大于当前最大长度
            if (free_table.at(i).flag == 1 && free_table.at(i).length > max_length) {
            // 更新最大空闲区索引和长度
                max_index = i;
                max_length = free_table.at(i).length;	
            } 	
        }
        return max_index; // 返回第一个（如果有同时为最大值的情况）最大空闲区索引
    }
    // 寻找最小空闲区
    int find_min_free(Used& u) {
        int min_index = -1; // 初始化最小空闲区索引为-1
        float min_length = check_total_memory(u) + 1; // 初始化最小空闲区长度为总内存量加一（最大值），万一最后仅剩下一块内存为总内存量的空闲区，也会被分配，确保不会返回-1
        // 遍历空闲区表的每个条目
        for (size_t i = 0; i < free_table.size(); i++) {
        // 检查空闲区是否未分配且长度小于当前最小长度
            if (free_table.at(i).flag == 1 && free_table.at(i).length < min_length) {
            // 更新最小空闲区索引和长度
                min_index = i;
                min_length = free_table.at(i).length;	
            } 	
        }
        return min_index; // 返回第一个（如果有同时为最小值的情况）最小空闲区索引
    }
    // 回收空闲区
    void recycle_free(Works& w, Used& u) { // 回收空闲区
        vector<Used::UsedEntry> r = u.decide_recycle(w); // 查找已分配状态的已分配区
        if (r.empty())  return; // 无待回收的已分配区
        // 合并相邻空闲区
        cout << "\n开始回收随机决定好的已分配区并合并相邻的空闲区：" << endl;
        for (size_t i = 0; i < r.size(); i++) { // 遍历待回收的已分配区
            // 记录合并后的起始地址
            float new_address = r.at(i).address;
            // 记录合并后的长度
            float new_length = r.at(i).length;
            // 合并上邻空闲区
            for (size_t j = 0; j < free_table.size(); j++) { // 遍历待回收的已分配区
                // 找到上邻空闲区
                if (free_table[j].flag != 0 && 
                    free_table[j].address + free_table[j].length == new_address) {
                    // 更新合并后的起始地址
                    new_address = free_table[j].address;
                    // 更新合并后的长度
                    new_length += free_table[j].length;
                    // 标记原空闲区为空
                    free_table[j].flag = 0;
                    free_table[j].address = 0;
                    free_table[j].length = 0;
                    break;
                }
            }
            // 合并下邻空闲区
            for (size_t j = 0; j < free_table.size(); j++) { // 遍历待回收的已分配区
                // 找到下邻空闲区
                if (free_table[j].flag != 0 &&
                    free_table[j].address == new_address + new_length) {
                    	// 更新合并后的长度
                        new_length += free_table[j].length;
                        // 标记原空闲区为空
                        free_table[j].flag = 0;
                        free_table[j].address = 0;
                        free_table[j].length = 0;
                        break;
                }
            }
            // 将合并后的空闲区插入到空闲区表中
            for (size_t j = 0; j < free_table.size(); j++)
            {
                if (free_table[j].flag == 0) { // 找到第一个空的空闲区
                    // 设置合并后的空闲区的起始地址
                    free_table[j].address = new_address;
                    // 设置合并后的空闲区的长度
                    free_table[j].length = new_length;
                    // 标记为未分配
                    free_table[j].flag = 1;  
                    // 加入成功，跳出循环
                    cout << "编号" << r.at(i).id << "作业" << r.at(i).name << "成功回收" << r.at(i).length << "字节内存到空闲区表中" << endl;
                    break;
                }
                else if (j == free_table.size() - 1 && free_table[j].flag != 0) { // 遍历完所有空闲区都未找到空的空闲区
                    cout << "未找到满足相邻位置合并条件的空闲区，现将编号" << r.at(i).id << "作业" << r.at(i).name << "的" << r.at(i).length << "字节内存插入到空闲区表末尾来回收" << endl;
                    free_table.emplace_back(new_address, new_length, 1); // 直接构造，避免拷贝
                    break;
                }
            }
        }
        w.print_jobs(); // 打印作业队列
        print_free_table(); // 打印空闲区表
        cout << "\n随机回收已分配的内存执行完毕！" << endl;
    }
    // 打印标识
    void print_mark(Works& w, Used& u, int j, int&& i = 1) { // 打印标识
        if (i == 0)
        {
            switch(j) {
                case 1: { // 首次适应分配
                    cout << "\n开始执行首次适应分配算法：" << endl;	
                    break;
                }
                case 2: { // 循环首次适应分配
                    cout << "\n开始执行循环首次适应分配算法：" << endl;
                    break;
                }
                case 3: { // 最优适应分配
                    cout << "\n开始执行最优适应分配算法：" << endl;
                    break;
                }
                case 4: { // 最坏适应分配
                    cout << "\n开始执行最坏适应分配算法：" << endl;
                    break;
                }
                default: { 
                    cout << "\n未定义适应分配算法！" << endl;
                    break;
                }
            } 
        } else { 
            w.print_jobs(); // 打印作业队列
            print_free_table(); // 打印空闲区表
            u.print_used_table(); // 打印已分配区表
            switch(j) {
                case 1: { // 首次适应分配
                    cout << "\n首次适应分配算法执行完毕！" << endl;
                    break;
                }	
                case 2: { // 循环首次适应分配
                    cout << "\n循环首次适应分配算法执行完毕！" << endl;
                    break;
                }
                case 3: { // 最优适应分配
                    cout << "\n最优适应分配算法执行完毕！" << endl;
                    break;
                }
                case 4: { // 最坏适应分配
                    cout << "\n最坏适应分配算法执行完毕！" << endl;
                    break;
                }
                default: {
                    cout << "\n未定义适应分配算法！" << endl;
                    break;	
                }
            }
        }
    }
    // 结束分配算法条件
    bool end_allocate(Works& w, Used& u, vector<int>& f, int i) { // 判断该轮的作业是否已经满足分配
        print_mark(w, u, i, 0);
        if(f.empty() || f.size() == 0) { // 如果没有未分配状态的作业
            cout << "所有作业都已分配完成，无未分配状态的作业！" << endl;
            print_mark(w, u, i);
            return false;
        } else {
            int E = 0; // 如果空闲区的状态都为空，说明所有作业都无法分配
            for (size_t i = 0; i < free_table.size(); i++) {
                if (free_table.at(i).flag == 1) {
                    E = 1;
                    break;
                }
            }
            if(E == 0) { // 说明空闲区的状态都为空，所有作业都无法分配
                cout << "没有处于未分配状态的空闲区，无法完成分配" << endl;
                print_mark(w, u, i);
                return false;
            }
			int M, N = 0; // 标识是否有作业内存需求大于总内存
        	for(int find: f) {
        		if(!w.get_jobs(find) -> name.empty()) { 
        		if(w.get_jobs(find) -> size > check_total_memory(u) || w.get_jobs(find) -> size > free_table.at(find_max_free()).length) ++M; // 检测是否有作业内存需求大于总内存或大于最大空闲区长度
				} else N++; // 记录未定义的作业
			}
            if(M == f.size() - N && M != 0) { // 跳过全是未定义的作业的情况
                cout << "剩下未分配作业的内存需求都大于当前最大空闲区" << free_table.at(find_max_free()).length << "字节的长度，无法完成分配" << endl;
                print_mark(w, u, i);
                return false;
            } else if(f.size() == N) { // 说明全是未定义的作业的情况
                cout << "所有作业都已分配完成，无未分配状态的作业！" << endl;
                print_mark(w, u, i);
                return false;
            }
        }
        return true; // 作业未分配完成
    }
    // 首次适应分配算法
    bool alloate_first_fit(Works& w, Used& u, int& I, int&& J = 1) { // 用右值引用&&传入避免不必要的拷贝 int I = 0，记录已分配区表的索引（默认初始从索引0开始记录已分配区表） alloate_first_fit(Works& w, Used& u, int&& I = 0)
        vector<int> f = w.find_jobs(0);
        if (!end_allocate(w, u, f, J)) return false; // 判断该轮的作业是否已经满足分配
        /*右值引用（T&&）绑定到右值，左值引用（T&）绑定到左值
          右值引用可以绑定到临时对象、字面量、表达式结果等右值，而左值引用只能绑定到左值
          右值引用通常用于移动语义和完美转发，以提高性能
          右值引用的主要目的是允许对右值进行修改和操作，而不是复制它们
          右值引用可以减少不必要的复制操作，提高程序的效率
          右值引用可以与移动语义结合使用，实现资源的高效转移
        */
        for (int j = 0; j < f.size(); j++) { // 遍历未分配的作业
            if(!w.get_jobs(f[j]) -> name.empty()){ // 跳过未定义的作业
            	float size = w.get_jobs(f[j]) -> size; // 获取未分配作业的内存需求
            	for (size_t i = 0; i < free_table.size(); i++) { // 遍历空闲区表
        // 找到第一个足够大的未分配块
                	if (free_table[i].flag == 1 && free_table[i].length >= size) {
            // 记录分配的起始地址
                    	float alloc_address = free_table[i].address;
            // 如果空闲区长度等于需求大小
                    	if (free_table[i].length == size) {
                // 标记该空闲区为空
                        	free_table[i].flag = 0;  
                            free_table[i].address = 0;
                            free_table[i].length = 0;
                    	} else {
                // 分割空闲区：剩余部分地址后移，长度减少
                        	free_table[i].address += size;
                        	free_table[i].length -= size;
                    	}
            // 标记作业为已分配
                    	w.modify_jobs(f.at(j), 1); // 直接修改作业状态
                        u.modify_used_table(I, w.get_jobs(f.at(j)) -> name, alloc_address, size, w.get_jobs(f.at(j)) -> id); // 更新已分配区表
                    	cout << "编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "已分配到地址" << alloc_address << "，大小为" << size << "字节" << endl;
                    	break;  // 分配完成，跳出循环
                	}
            	}
            	if(w.get_jobs(f.at(j)) -> flag == 0) cout << "未找到足够大的空闲块来满足编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "的" << size << "字节内存的需求" << endl;
            }	
        }
        print_mark(w, u, J);
        return true;
    }
    // 循环首次适应分配算法
    bool alloate_loop_first_fit(Works& w, Used& u, int& I, int& start, int&& J = 2) { 
        vector<int> f = w.find_jobs(0);
        if (!end_allocate(w, u, f, J)) return false; // 判断该轮的作业是否已经满足分配
        for (int j = 0; j < f.size(); j++) { // 遍历未分配的作业
            if(!w.get_jobs(f[j]) -> name.empty()){ // 跳过未定义的作业
            	float size = w.get_jobs(f[j]) -> size; // 获取未分配作业的内存需求
            	int s = start; // 记录起始位置，并判断是否循环查找一轮无果
                while (1) { // 遍历空闲区表
                    int i = start % free_table.size(); // 循环遍历
        // 找到下一个足够大的未分配块
                	if (free_table[i].flag == 1 && free_table[i].length >= size) {
            // 记录分配的起始地址
                    	float alloc_address = free_table[i].address;
            // 如果空闲区长度等于需求大小
                    	if (free_table[i].length == size) {
                // 标记该空闲区为空
                        	free_table[i].flag = 0;  
                            free_table[i].address = 0;
                            free_table[i].length = 0;
                    	} else {
                // 分割空闲区：剩余部分地址后移，长度减少
                        	free_table[i].address += size;
                        	free_table[i].length -= size;
                    	}
            // 标记作业为已分配
                    	w.modify_jobs(f.at(j), 1); // 直接修改作业状态
                        u.modify_used_table(I, w.get_jobs(f.at(j)) -> name, alloc_address, size, w.get_jobs(f.at(j)) -> id); // 更新已分配区表
                    	cout << "编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "已分配到地址" << alloc_address << "，大小为" << size << "字节" << endl;
                    	start++; // 指向下一个空闲区
                        break;  // 分配完成，跳出循环
                	}
                	++start; // 继续查找下一个空闲区
                    if(start - s == free_table.size()) break; // 循环查找一轮无果
            	}
            	if(w.get_jobs(f.at(j)) -> flag == 0) cout << "未找到足够大的空闲块来满足编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "的" << size << "字节内存的需求" << endl;
            }	
        }
        print_mark(w, u, J);
        return true;
    }
    // 最优适应分配算法
    bool alloate_best_fit(Works& w, Used& u, int& I, int&& J = 3) { // 用右值引用&&传入避免不必要的拷贝 int I = 0，记录已分配区表的索引（默认初始从索引0开始记录已分配区表） alloate_first_fit(Works& w, Used& u, int&& I = 0)
        vector<int> f = w.find_jobs(0);
        if (!end_allocate(w, u, f, J)) return false; // 判断该轮的作业是否已经满足分配
        for (int j = 0; j < f.size(); j++) { // 遍历未分配的作业
            if(!w.get_jobs(f[j]) -> name.empty()){ // 跳过未定义的作业
                float size = w.get_jobs(f[j]) -> size; // 获取未分配作业的内存需求
                int min_index = -1; // 初始化最小空闲区索引为-1
                float min_length = check_total_memory(u) + 1; // 初始化最小空闲区长度为总内存量加一（最大值）
                // 遍历空闲区表的每个条目
                for (size_t i = 0; i < free_table.size(); i++) { // 遍历空闲区表
                // 检查空闲区是否未分配且长度小于当前最小长度
                    if (free_table[i].flag == 1 && free_table[i].length >= size && free_table[i].length < min_length) {
                    // 更新最小空闲区索引和长度
                        min_index = i; // 记录并返回第一个满足要求的最小空闲区索引
                        min_length = free_table[i].length; 	
                    }
                }
                if (min_index != -1) { // 找到最小空闲区
                    // 记录分配的起始地址
                    float alloc_address = free_table[min_index].address;
                    // 如果空闲区长度等于需求大小
                    if (free_table[min_index].length == size) {
                        // 标记该空闲区为空
                        free_table[min_index].flag = 0;
                        free_table[min_index].address = 0;
                        free_table[min_index].length = 0; 	
                    } else { // 分割空闲区：剩余部分地址后移，长度减少
                        free_table[min_index].address += size;
                        free_table[min_index].length -= size;
                    }
                // 标记作业为已分配
                w.modify_jobs(f.at(j), 1); // 直接修改作业状态
                u.modify_used_table(I, w.get_jobs(f.at(j)) -> name, alloc_address, size, w.get_jobs(f.at(j)) -> id); // 更新已分配区表
                cout << "编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "已分配到地址" << alloc_address << "，大小为" << size << "字节" << endl;
                } else { // 未找到足够大的空闲块
                    cout << "未找到足够大的空闲块来满足编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "的" << size << "字节内存的需求" << endl;
                }
            }
        }
        print_mark(w, u, J);
        return true;
    }
    // 最坏适应分配算法
    bool alloate_worst_fit(Works& w, Used& u, int& I, int&& J = 4) { // 用右值引用&&传入避免不必要的拷贝 int I = 0，记录已分配区表的索引（默认初始从索引0开始记录已分配区表） alloate_first_fit(Works& w, Used& u, int&& I = 0)
        vector<int> f = w.find_jobs(0);
        if (!end_allocate(w, u, f, J)) return false; // 判断该轮的作业是否已经满足分配
        for (int j = 0; j < f.size(); j++) { // 遍历未分配的作业
            if(!w.get_jobs(f[j]) -> name.empty()){ // 跳过未定义的作业
                float size = w.get_jobs(f[j]) -> size; // 获取未分配作业的内存需求
                float alloc_address = free_table[find_max_free()].address; // 记录最大空闲区分配的起始地址
                float max_length = free_table.at(find_max_free()).length; // 记录最大空闲区的长度
                if (max_length >= size) { // 找到最大空闲区
                    // 如果空闲区长度等于需求大小
                    if (max_length == size) {
                        // 标记该空闲区为空
                        free_table[find_max_free()].flag = 0;
                        free_table[find_max_free()].address = 0;
                        free_table[find_max_free()].length = 0;	
                    } else { // 分割空闲区：剩余部分地址后移，长度减少
                        free_table[find_max_free()].address += size;
                        free_table[find_max_free()].length -= size;
                    }
                // 标记作业为已分配
                    w.modify_jobs(f.at(j), 1); // 直接修改作业状态
                    u.modify_used_table(I, w.get_jobs(f.at(j)) -> name, alloc_address, size, w.get_jobs(f.at(j)) -> id); // 更新已分配区表
                    cout << "编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "已分配到地址" << alloc_address << "，大小为" << size << "字节" << endl;
                } else { // 未找到足够大的空闲块
                    cout << "未找到足够大的空闲块来满足编号" << w.get_jobs(f.at(j)) -> id << "作业" << w.get_jobs(f.at(j)) -> name << "的" << size << "字节内存的需求" << endl;
                }
            }
        }
        print_mark(w, u, J);
        return true;
    }
};
// 主函数
int main() {
    while(1){
        this_thread::sleep_for(std::chrono::milliseconds(250));  // 暂停 0.25 秒 
        system("cls");  // 清屏
    	cout << "\n\t---------------分区管理---------------" << endl;
		Works w;  // 创建作业对象
    	Free f;   // 创建空闲区对象
        Used u;  // 创建已分配区对象
        if(!f.start(w, u)) break; // 开始分配算法
	}
    system("pause");  // 暂停程序
    return 0;  // 程序结束 
}