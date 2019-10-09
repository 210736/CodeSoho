#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <iterator>
#include <vector>
using namespace std;

int main()
{
//----------------------泛型算法----------------------------------------
//-----------------------INT---------------------------------------------
	int ia[] = { 27,210,12,47,109,83 };
	int ia2[] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	int val = 83;

	//查询是否ia中含有val
	auto *result = find(begin(ia), end(ia), val);
	cout << "The value " << val << (result == end(ia) ? "is not present" : "is present") << endl;
	cout << "The value address is " << result << endl;

	//将ia中所有字符连接
	int sum = accumulate(begin(ia), end(ia), 0);
	cout << "sum = " << sum << endl;

	// //将ia2和ia相对比
	// auto a=equal(begin(ia), end(ia), begin(ia2));
	// cout << a << endl;

	//将number中所有元素置为0
	int number[] = { 4,1,5,54,54,3,135,1,3 };
	fill(begin(number), end(number), 0);
	// //将ia中从前往后6个元素置为0
	// fill_n(begin(ia), 6, 0);

	//将一些字符插入容器
	vector<int> vec;
	auto it = back_inserter(vec);//back_inserter接收一个指向容器的引用，返回一个与该容器绑定的的插入迭代器
	*it = 42;//vec中含有一个42了
	fill_n(back_inserter(vec), 10, 0); // 在末尾插入10个0

	//将ia复制到ia3
	int ia3[sizeof(ia) / sizeof(*ia)];//ia3和ia一样大小
	auto ret = copy(begin(ia), end(ia), ia3);//ret为ia3末端地址
	cout << "ret= "<<ret << endl;

	// //替换算法
	// int REP = { 1,2,3,4,5,6,7,8,9 };
	// replace(begin(), end(REP),2,3);//将2替换成3
	// int ivec = {};
	// replace_copy(cbegin(REP), cend(REP), back_inserter((ivec), 0, 42));//将0替换成42保存到ivec中

	//删除重读元素
	string words;
	void elimDups(vector<string> & words);
	{
		//按字典排序word，以便查找重复单词
		sort(words.begin(), words.end());
		//unique重排输入范围，使得每个单词只出现一次
		//排列在范围的前部，返回指向不重复区域之后一个位置的迭代器
		auto end_unique = unique(words.begin(), words.end());
		//使用向量操作erase删除重复单词
		words.erase(end_unique, words.end());
	}




//--------------------------STRING----------------------------------------------------
	string name[] = {"sadasfasfdsa", "dsadas", "sadasd"};
	string name2[] = { "sd","sdas","sad","sada","sadas","asdas","sad" };
	string Name = "sadasdasf";
	string str = "das";
	string sumstr = accumulate(cbegin(name), cend(name), string(""));
	cout << sumstr << endl;

	// int s = equal(cbegin(name), cend(name), cbegin(name2));
	// cout << s << endl;
	// auto strfind = find(cbegin(Name), cend(Name), str);
	// cout << "The value" << str << (strfind == Name.cend() ? "is not present" : "is present") << endl;;

	cout << name << endl;

	
	return 0;
}



