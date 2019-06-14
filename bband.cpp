#include <iostream>
#include <fstream>
#include <cmath>
#include<algorithm>
#include<time.h>
#include<string>
using namespace std;
struct bband {
	float sd;
	float * ma;
	float * up;
	float * down;
};
struct test {
	int count_move;
	int success;
	double profit;
};
struct check {
	int countlow;
	int counthigh;
};
struct raw {
	string open;
	string close;
	string high;
	string low;
};
struct data_b {
	float * high;
	float * low;
};
void clear(float *&a, int n) {
	for (int i = 0; i < n; i++)
		a[i] = 0;
}
void read_data(string data, float * &store, int &n, data_b &d) {//read data from txt(checked)
	ifstream infile(data);
	infile >> n;
	raw a;
	store = new float[n];
	d.high = new float[n];
	d.low = new float[n];
	for (int i = 0; i < n; i++) {

		infile >> a.open >> a.close >> a.high >> a.low;

		a.open.erase(remove(a.open.begin(), a.open.end(), ','), a.open.end());
		a.close.erase(remove(a.close.begin(), a.close.end(), ','), a.close.end());
		a.high.erase(remove(a.high.begin(), a.high.end(), ','), a.high.end());
		a.low.erase(remove(a.low.begin(), a.low.end(), ','), a.low.end());
		store[n - i - 1] = (stof(a.close) + stof(a.high) + stof(a.open)) / 3;

		d.low[n - i - 1] = stof(a.low);
		d.high[n - i - 1] = stof(a.high);
	}

};
long long power(long long a, int  b) {
	if (b == 0)
		return 1;
	else if (b % 2 == 0)
		return power(a, b / 2)*power(a, b / 2);
	else
		return a * power(a, b / 2)*power(a, b / 2);
}
void cal_sd(bband& a, float * store, float m, int n,int j) {//calculate sd(checked)
	/*
	double mean;
	for(int i=0;i<n;i++){
		mean+=store[i];

	}

	mean/=n;
	*/
	a.sd = 0;
	double temp = 0;
	long long temp_l = 0;
	if (store[0] > 1000) {
		for (int i = j-19; i < j+1; i++) {
			long long a = store[i] - m;
			temp_l += power(a, 2);
		}
		//cout << temp_l<<endl;
		temp_l /= (n - 1);
		a.sd = sqrt(temp_l);
	}
	else {
		for (int i = 0; i < n; i++)
			temp += (double)pow(store[i] - m, 2);
		temp /= (n - 1);
		a.sd = sqrt(temp);
	}



	//cout << a.sd << endl;

}

void cal_ma(bband &a, float* store, int n) {// calculate ma,ma+2sd,ma-2sd(checked)
	float temp = 0;
	for (int i = 19; i < n; i++) {
		//if(i==19) {
		for (int j = i - 19; j < i+1; j++) {
			a.ma[i - 19] += store[j];
		}
		temp = a.ma[i - 19];
		// }
	   /*else if(i+1<n){
			 temp -= store[i-20];
			 temp += store[i+1];
			a.ma[i - 19]=temp;
		 }*/
		a.ma[i - 19] /= 20;

		cal_sd(a, store, a.ma[i - 19], 20,i);

		a.up[i - 19] = a.ma[i - 19] + 2 * a.sd;
		a.down[i - 19] = a.ma[i - 19] - 2 * a.sd;
	}
}

//backtesting
void back_test(bband a, data_b s, int n, test& t, double h) {
	bool buy = false;
	int count = 0;
	double amount = 0;
	for (int i = 19; i < n; i++) {
		if (s.low[i] <= a.down[i - 19]) {
			if (!buy)
				buy = true;
			count++;
			amount += (s.low[i] * h);
		}
		else if (s.high[i] >= a.up[i - 19]) {
			if (buy) {
				//cout << i << endl;
				t.profit += (s.high[i] * h * count - amount);
				buy = false;
				if ((s.high[i] * h * count - amount) > 0)
					t.success++;
				count = amount = 0;
				t.count_move++;
			}
		}


	}

}
//prepare 5-year large data
check testing(bband a, float* s, int n, data_b d) {
	check c = { 0,0 };
	for (int i = 19; i < n; i++) {
		if (d.low[i] <= a.down[i - 19]) {
			c.countlow++;
			
		}
		else if (d.high[i] >= a.up[i - 19]) {
			c.counthigh++; //cout << i << endl;
		}
	}
	return c;

}
int main() {
	clock_t t1, t2;
	t1 = clock();
	string file = "mega.txt";
	int n = 0;
	float * store;
	long long a;
	data_b d;

	read_data(file, store, n, d);


	//needed!!!

	bband temp;
	temp.ma = new float[n];
	temp.up = new float[n];
	temp.down = new float[n];
	clear(temp.ma, n);
	clear(temp.up, n);
	clear(temp.down, n);
	//cal_sd(temp,store,n);
	cal_ma(temp, store, n);

	//test
	cout << "The input data : n = " << n << endl;
	
	test t = { 0,0,0 };
	double hand = 10;
	cout << "How many dollar you buy each time?" << endl;
	back_test(temp, d, n, t, hand);
	a = t.profit;
	cout << "The number of trading in profit: " << t.success << endl << "The total number of trading : " << t.count_move << "  " << endl << "The profit : ";
		if (t.profit > 10000)
			cout << a << endl;
	    else
		    cout<<t.profit << endl;
	check c = testing(temp, store, n, d);
	cout << c.countlow << "  " << c.counthigh << endl;

	t2 = clock();  cout << "The usage of time to run the program : "; cout.precision(5); cout << (t2 - t1) / (double)(CLOCKS_PER_SEC) << " secs"<<endl;

	delete[](store);
	system("pause");
	return 0;
}