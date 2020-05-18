#include "Huffman.h"

void swap(HuffmanNode a, HuffmanNode b)
{
	HuffmanNode temp = a;
	a = b;
	b = temp;
}

void sort(vector<HuffmanNode*> &a)
{
	for (int i = 0; i < a.size() - 1; i++)
		for (int j = i + 1; j < a.size(); j++)
			if (a[i]->freq < a[j]->freq)
				swap(a[i], a[j]);

}

HuffmanNode* createRoot()
{
	return newNode('#', 0, "0");
}

HuffmanNode* newNode(char data, int freq, string p)
{
	HuffmanNode* node = new HuffmanNode;
	node->data = data;
	node->freq = freq;
	node->left = node->right = nullptr;
	node->path = p;
	return node;
}

void UpdatePath(HuffmanNode* root)
{
	if (isLeaf(root))
		return;
	root->right->path = root->path + root->right->path;
	root->left->path = root->path + root->left->path;
	UpdatePath(root->right);
	UpdatePath(root->left);
}

HuffmanNode* createTree(vector<HuffmanNode*> &a)
{
	if (a.size() == 1)
	{
		return a[0];
	}
	
	HuffmanNode* node = newNode('#', 0, "");
	node->right = a.back();
	node->right->path = "1" + node->right->path;
	a.erase(a.end()-1);
	node->left = a.back();
	node->left->path = "0" + node->left->path;
	a.erase(a.end()-1);

	node->freq = node->left->freq + node->right->freq;
	a.push_back(node);
	sort(a);
		
	createTree(a);

	return a[0];
}

void Search(HuffmanNode* root, char c, HuffmanNode* &v)
{
	if (root == NULL)
		return;
	if (root->data == c)
	{
		v = root;
		return;
	}
	Search(root->left, c, v);
	Search(root->right, c, v);
}

bool isLeaf(HuffmanNode* p)
{
	return (p->left == 0 && p->right == 0);
}

vector<HuffmanNode*> countFreq(string arr, int n)
{
	unordered_map<char, int> mp;

	// Traverse through array elements and count frequencies 

	for (int i = 0; i <= n; i++)
		mp[arr[i]]++;

	// Traverse through map and print frequencies 
	vector<HuffmanNode*> a;
	for (auto x : mp)
	{
		HuffmanNode* temp = newNode(x.first, x.second, "");
		a.push_back(temp);
	}
	
	sort(a);
	return a;
}

pair<string, int> encode(vector<HuffmanNode*> a, string buf, HuffmanNode*& root)
{
	root = createRoot();
	root = createTree(a);
	UpdatePath(root);
	string temp;
	for (int i = 0; i < buf.length(); i++)
	{
		HuffmanNode* v;
		Search(root, buf[i], v);
		temp += v->path;
	}
	int count = 0;
	while (temp.length() % 8 != 0)
	{
		temp.push_back('0');
		count++;
	}

	string ans;
	for (int i = 0; i + 8 <= temp.length(); i += 8)
	{
		string t(temp.begin() + i, temp.begin() + i + 8);
		ans.push_back(convertFromByte(t));
	}
	return {ans, count};
}

unsigned char convertFromByte(string s)
{
	int ans = 0;
	int exp = 0;
	for (int i = s.length() - 1; i >= 0; i--)
	{
		if (s[i] == '1')
			ans += pow(2, exp);
		exp++;
	}
	unsigned char c = unsigned char(ans);
	
	return c;
}

string convertFromChar(unsigned char c)
{
	string ans;

	unsigned int s = unsigned int(c);
	
	for (int i = 0; s > 0; i++)
	{
		if (s % 2)
			ans.push_back('1');
		else ans.push_back('0');
		s /= 2;
	}

	while (ans.length() < 8)
	{
		ans.push_back('0');
	}

	reverse(ans.begin(), ans.end());
	return ans;
}

string decode(string s, HuffmanNode* root, int Abits)
{
	HuffmanNode* p = root;

 	string temp;
	for (int i = 0; i < s.length(); i++)
		temp += convertFromChar(s[i]);
	s = temp;

	for (int i = 0; i < Abits; i++)
		s.erase(s.begin() + s.length() - i - 1);

	string ans;
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] == '0')
			p = p->left;
		else p = p->right;

		if (isLeaf(p))
		{
			ans += p->data;
			p = root;
		}
	}
	
	return ans;
}

