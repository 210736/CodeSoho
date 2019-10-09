#include <iostream>
#include <algorithm>
#include <numeric>
#include <string>
#include <iterator>
#include <vector>
using namespace std;

int main()
{
//----------------------�����㷨----------------------------------------
//-----------------------INT---------------------------------------------
	int ia[] = { 27,210,12,47,109,83 };
	int ia2[] = { 0,0,0,0,0,0,0,0,0,0,0,0 };
	int val = 83;

	//��ѯ�Ƿ�ia�к���val
	auto *result = find(begin(ia), end(ia), val);
	cout << "The value " << val << (result == end(ia) ? "is not present" : "is present") << endl;
	cout << "The value address is " << result << endl;

	//��ia�������ַ�����
	int sum = accumulate(begin(ia), end(ia), 0);
	cout << "sum = " << sum << endl;

	// //��ia2��ia��Ա�
	// auto a=equal(begin(ia), end(ia), begin(ia2));
	// cout << a << endl;

	//��number������Ԫ����Ϊ0
	int number[] = { 4,1,5,54,54,3,135,1,3 };
	fill(begin(number), end(number), 0);
	// //��ia�д�ǰ����6��Ԫ����Ϊ0
	// fill_n(begin(ia), 6, 0);

	//��һЩ�ַ���������
	vector<int> vec;
	auto it = back_inserter(vec);//back_inserter����һ��ָ�����������ã�����һ����������󶨵ĵĲ��������
	*it = 42;//vec�к���һ��42��
	fill_n(back_inserter(vec), 10, 0); // ��ĩβ����10��0

	//��ia���Ƶ�ia3
	int ia3[sizeof(ia) / sizeof(*ia)];//ia3��iaһ����С
	auto ret = copy(begin(ia), end(ia), ia3);//retΪia3ĩ�˵�ַ
	cout << "ret= "<<ret << endl;

	// //�滻�㷨
	// int REP = { 1,2,3,4,5,6,7,8,9 };
	// replace(begin(), end(REP),2,3);//��2�滻��3
	// int ivec = {};
	// replace_copy(cbegin(REP), cend(REP), back_inserter((ivec), 0, 42));//��0�滻��42���浽ivec��

	//ɾ���ض�Ԫ��
	string words;
	void elimDups(vector<string> & words);
	{
		//���ֵ�����word���Ա�����ظ�����
		sort(words.begin(), words.end());
		//unique�������뷶Χ��ʹ��ÿ������ֻ����һ��
		//�����ڷ�Χ��ǰ��������ָ���ظ�����֮��һ��λ�õĵ�����
		auto end_unique = unique(words.begin(), words.end());
		//ʹ����������eraseɾ���ظ�����
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



