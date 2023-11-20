#include<iostream>
#include<fstream>
#include<vector>
#include<time.h>
using namespace std;
class com
{
	private:
		int Inverse = 0;
		int Length;
		int Run;
		string Pattern;
	public:
		com() 
		{ 
			this->Length = 0;
			this->Run = 0;
			this->Pattern = "";
		}
		void update(int length, int run, string pattern)	//更新資料
		{	
			this->Length = length;
			this->Run = run;
			this->Pattern = pattern;
		}
		void setpattern(string pattern)
		{
			this->Pattern = pattern;
		}
		void setinverse(int inverse)
		{
			this->Inverse = inverse;
		}
		bool ok()	//判斷是否可以切割
		{
			vector<string> part(Run);	//將讀入字串進行切割
			for (int i = 0; i < part.size(); i++)
				part[i] = Pattern.substr(i * Length, Length);	//從i*length開始, 取長度為length的string
			string all = part[0];
			int inverse = 0;
			for (int i = 1; i < part.size(); i++)
			{
				int inverse_bit = 0;
				for (int j = 0; j < part[0].size(); j++)
				{
					if (all[j] == 'X')
						all[j] = part[i][j];
					if ((all[j] == '1' && part[i][j] == '0') || (all[j] == '0' && part[i][j] == '1') )
						inverse_bit++;
				}
		/*		if (inverse_bit == part[0].size())
				{
					setinverse(1);
					inverse++;
				}*/
				if (/*inverse != i && */inverse_bit != 0)
				{
					setinverse(0);
					return false;
				}
			}
			setpattern(all);
			return true;
		}
		bool last_again(com last)	//判斷是否為上一個pattern重複 
		{
			if (last.getinversebit() == 0)
			{
				for (int i = 0; i < Run; i++)
				{
					if (Pattern.substr(i * Length, Length) != last.getpattern())
						return false;
				}
				return true;
			}
			else
			{
				if (Pattern.substr(0, Length) != last.getpattern())
					return false;
				last.setpattern(Inverse_Pattern(last.getpattern()));
				for (int i = 1; i < Run; i++)
				{
					if (Pattern.substr(i * Length, Length) != last.getpattern())
						return false;
				}
				return true;
			}
		}
		bool last_inverse(com last)	//判斷是否為上一個pattern的inverse
		{
			if (last.getinversebit() == 0)
			{
				last.setpattern(Inverse_Pattern(last.getpattern()));
				for (int i = 0; i < Run; i++)
				{
					if (Pattern.substr(i * Length, Length) != last.getpattern())
						return false;
				}
				return true;
			}
			else
			{
				for (int i = 1; i < Run; i++)
				{
					if (Pattern.substr(i * Length, Length) != last.getpattern())
						return false;
				}
				if (Pattern.substr(0, Length) != Inverse_Pattern(last.getpattern()))
					return false;
				return true;
			}
		}
		string Inverse_Pattern(string pattern)	//取inverse
		{
			for (int i = 0; i < pattern.length(); i++)
			{
				if (pattern[i] == '0')
					pattern[i] = '1';
				else if (pattern[i] == '1')
					pattern[i] = '0';
			}
			return pattern;
		}
		int getbit()
		{
			return ((this->Run) * (this->Length));
		}
		int getrun()
		{	
			return this->Run;
		}
		int getlength()
		{
			return this->Length;
		}
		int getinversebit()
		{
			return this->Inverse;
		}
		string getpattern()
		{
			return this->Pattern;
		}
};
int compress(string orig,int orig_bit,string &compressed);
double print_compress(string file,string ofile);
void ori_bit_do(string pattern, string& compressed, int ori_bit);
int main()
{
	string file[6] = {"s5378f.vec","s9234f.vec","s13207f.vec","s15850f.vec","s38417f.vec","s38584f.vec"};
	double total = 0;
	for (int i = 0; i < 6; i++)
	{
		cout << file[i] << "：";
		string ofile = file[i];
		ofile+="_compressed";
		total += print_compress(file[i], ofile);
	}
	cout << "總執行時間：" << total << "s\n";
}
double print_compress(string file,string ofile)
{
	int compressed_bit = 0, orig_bit = 0;
	double start, end;
	vector<string> orig;
	vector<string> compressed;
	ifstream open(file);
	ofstream out(ofile);
	if (!open)
	{
		cout << "Can't open!";
		return -1;
	}
	while (open)
	{
		orig.resize(orig.size() + 1);
		compressed.resize(compressed.size() + 1);
		open >> orig[orig.size() - 1];
	}
	start = clock();
	for (int i = 0; i < orig.size(); i++)
	{
		int now_bit = orig[i].size();
		orig_bit += now_bit;
		compressed_bit += compress(orig[i], now_bit, compressed[i]);
		out << compressed[i] << endl;
	}
	end = clock();
	float compre = (orig_bit - compressed_bit);
	cout << "壓縮率：" << compre / orig_bit * 100 << "%\n";
	return  (end - start) / CLOCKS_PER_SEC;
}
int compress(string orig, int orig_bit, string& compressed)	//資料壓縮
{
	vector<com> comp;
	int compressed_bit = 0;
	int ori_bit = 0;
	bool m = false;
	while (orig_bit > 0)
	{
		comp.resize(comp.size() + 1);
		for (int i = 32; i > 0; i--)
		{
			bool modify = false;
			if (i * i > orig_bit)
			{
				orig.append(i * i - orig_bit, 'X');
				ori_bit = orig_bit;
				modify = true;
				m = true;
			}
			comp[comp.size() - 1].update(i, i, orig.substr(0, i * i));
			if (comp.size() > 1 && comp[comp.size() - 1].getlength() == comp[comp.size() - 2].getlength())	//可判斷是否跟上一個儲存pattern相同 or inverse
			{
				if (comp[comp.size() - 1].last_again(comp[comp.size() - 2]))
				{
					orig_bit -= comp[comp.size() - 1].getbit();
					orig.erase(0, comp[comp.size() - 1].getbit());
					comp[comp.size() - 1].update(0, i, "");
					break;
				}
				if (comp[comp.size() - 1].last_inverse(comp[comp.size() - 2]))
				{
					orig_bit -= comp[comp.size() - 1].getbit();
					orig.erase(0, comp[comp.size() - 1].getbit());
					comp[comp.size() - 1].update(0, i, "");
					comp[comp.size() - 1].setinverse(1);
					break;
				}
			}
			if (comp[comp.size() - 1].ok())
			{
				orig_bit -= comp[comp.size() - 1].getbit();
				orig.erase(0, comp[comp.size() - 1].getbit());
				break;
			}
			else
			{
				if (modify)
				{
					orig.erase(orig_bit, i * i - orig_bit);
					if (i * (i - 1) > orig_bit)
					{
						orig.append(i * (i - 1) - orig_bit, 'X');
						m = true;
					}
				}
				comp[comp.size() - 1].update(i, i - 1, orig.substr(0, i * (i - 1)));
				if (comp[comp.size() - 1].ok())
				{
					orig_bit -= comp[comp.size() - 1].getbit();
					orig.erase(0, comp[comp.size() - 1].getbit());
					break;
				}
				else
				{
					comp[comp.size() - 1].update(i - 1, i, orig.substr(0, i * (i - 1)));
					if (comp[comp.size() - 1].ok())
					{
						orig_bit -= comp[comp.size() - 1].getbit();
						orig.erase(0, comp[comp.size() - 1].getbit());
						break;
					}
				}
				if (modify)
					orig.erase(orig_bit, i * (i - 1) - orig_bit);
			}
			m = false;
		}
	}
	for (int i = 0; i < comp.size(); i++)
	{
		if (i != comp.size() - 1 || !m)
		{
			//if (comp[i].getinversebit() == 0)
			{
				if (comp[i].getpattern().length() != 0)	//跟上一個pattern儲存不同
				{
					int bit = comp[i].getpattern().length();
					for (int j = 0; j < comp[i].getrun(); j++)
						compressed += comp[i].getpattern();
					compressed_bit += comp[i].getpattern().length();
				}
				else //跟上一個pattern儲存同
				{
					for (int j = 0; j < comp[i].getrun(); j++)
						compressed += comp[i - 1].getpattern();
				}
			}
			/*else
			{
				if (comp[i].getpattern().length() != 0)	//跟上一個pattern儲存不同
				{
					compressed += comp[i].getpattern();
					for (int j = 1; j < comp[i].getrun(); j++)
						compressed += comp[i].Inverse_Pattern(comp[i].getpattern());
					compressed_bit += comp[i].getpattern().length();
				}
				else //跟上一個pattern儲存同
				{
					compressed += comp[i].Inverse_Pattern(comp[i].getpattern());
					for (int j = 1; j < comp[i].getrun(); j++)
						compressed += comp[i - 1].getpattern();
				}
			}*/
		}
		else //最後有多存
		{
			//if (comp[i].getinversebit() == 0)
			{
				if (comp[i].getpattern().length() != 0)	//跟上一個pattern儲存不同
				{
					ori_bit_do(comp[i].getpattern(), compressed, ori_bit);
					compressed_bit += comp[i].getpattern().length();
				}
				else //跟上一個pattern儲存同
					ori_bit_do(comp[i - 1].getpattern(), compressed, ori_bit);
			}
		/*	else
			{
				if (comp[i].getpattern().length() != 0)	//跟上一個pattern儲存不同
				{
					compressed += (ori_bit >= comp[i].getlength()) ? comp[i].getpattern() : comp[i].getpattern().substr(0,ori_bit);
					ori_bit -= comp[i].getlength();
					ori_bit_do(comp[i].Inverse_Pattern(comp[i].getpattern()), compressed, ori_bit);
					compressed_bit += comp[i].getpattern().length();
				}
				else //跟上一個pattern儲存同
				{
					compressed += (ori_bit >= comp[i - 1].getlength()) ? comp[i - 1].Inverse_Pattern(comp[i - 1].getpattern()) : comp[i - 1].getpattern().substr(0, ori_bit);
					ori_bit -= comp[i].getlength();
					ori_bit_do(comp[i - 1].getpattern(), compressed, ori_bit);
				}
			}*/
		}
		int run = comp[i].getrun() - 1, length = comp[i].getlength() - 1;
		while (run != 0)
		{
			compressed_bit++;
			run /= 2;
		}
		while (length != 0)
		{
			compressed_bit++;
			length /= 2;
		}
		//compressed_bit++;	//加inverse bit
	}
	return compressed_bit;
}
void ori_bit_do(string pattern,string & compressed, int ori_bit)
{
	while (ori_bit > 0)
	{
		if (ori_bit >= pattern.length())
			compressed += pattern;
		else
			compressed += pattern.substr(0, ori_bit);
		ori_bit -= pattern.length();
	}
}