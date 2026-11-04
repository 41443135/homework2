#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
using namespace std;
class Polynomial;
class Term {
    friend Polynomial; //讓polynomial可以直接存取term的private成員
    friend ostream& operator<<(ostream& out, const Polynomial& p); //讓輸出運算子也能存取term
    friend istream& operator>>(istream& in, Polynomial& p);         //讓輸入運算子也能存取term
private:
    float coef;// 係數項
    int exp;// 指數項
};
class Polynomial {
    friend ostream& operator<<(ostream& out, const Polynomial& p);
    friend istream& operator>>(istream& in, Polynomial& p);
private:
    Term* termArray;
    int capacity;//容量大小
    int terms;//非零項數量
public:
    Polynomial() : capacity(2), terms(0) {
        termArray = new Term[capacity]; // 初始動態配置兩個空間
    }
    ~Polynomial() {
        delete[] termArray;
    }
    //複製建構子(copy constructor)處理動態記憶體深拷貝
    Polynomial(const Polynomial& other) : capacity(other.capacity), terms(other.terms) {
        termArray = new Term[capacity];
        copy(other.termArray, other.termArray + terms, termArray);
    }
    //指定運算子多載(assignment operator)防止記憶體重複釋放
    Polynomial& operator=(const Polynomial& other) {
        if (this != &other) {
            Term* temp = new Term[other.capacity];
            copy(other.termArray, other.termArray + other.terms, temp);
            delete[] termArray;
            termArray = temp;
            capacity = other.capacity;
            terms = other.terms;
        }
        return *this;
    }
    void newTerm(const float theCoef, const int theExp);
    Polynomial Add(const Polynomial& b);
    Polynomial Mult(const Polynomial& b);
    float Eval(float x);
};
void Polynomial::newTerm(const float theCoef, const int theExp) {
    if (theCoef == 0) return; // 係數是 0 的話這個項就不用存
    if (terms == capacity) {  // 判斷空間滿了沒
        capacity *= 2;        // 容量加倍
        Term* temp = new Term[capacity];
        copy(termArray, termArray + terms, temp);
        delete[] termArray;
        termArray = temp;
    }
    termArray[terms].coef = theCoef;
    termArray[terms++].exp = theExp;
}
Polynomial Polynomial::Add(const Polynomial& b) {
    Polynomial c;
    int aPos = 0, bPos = 0;
    while ((aPos < terms) && (bPos < b.terms))
        if (termArray[aPos].exp == b.termArray[bPos].exp) {
            float t = termArray[aPos].coef + b.termArray[bPos].coef;
            if (t != 0) c.newTerm(t, termArray[aPos].exp);
            aPos++; bPos++;
        }
        else if (termArray[aPos].exp < b.termArray[bPos].exp) {
            c.newTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
            bPos++;
        }
        else {
            c.newTerm(termArray[aPos].coef, termArray[aPos].exp);
            aPos++;
        }
    for (; aPos < terms; aPos++)
        c.newTerm(termArray[aPos].coef, termArray[aPos].exp);
    for (; bPos < b.terms; bPos++)
        c.newTerm(b.termArray[bPos].coef, b.termArray[bPos].exp);
    return c;
}
Polynomial Polynomial::Mult(const Polynomial& b) {
    Polynomial result; // 一開始是 0
    for (int i = 0; i < terms; i++) {
        Polynomial temp;
        for (int j = 0; j < b.terms; j++) {
            temp.newTerm(termArray[i].coef * b.termArray[j].coef, termArray[i].exp + b.termArray[j].exp);
        }
        result = result.Add(temp); // 乘起來之後一列一列丟去加
    }
    return result;
}
float Polynomial::Eval(float x) {
    float result = 0;
    for (int i = 0; i < terms; i++) {
        result = result + termArray[i].coef * pow(x, termArray[i].exp);
    }
    return result;
}
istream& operator>>(istream& in, Polynomial& p) {
    int n; // 有幾組係數指數
    in >> n;
    p.terms = 0;
    for (int i = 0; i < n; i++) {
        float c; int e;
        in >> c >> e;
        p.newTerm(c, e);
    }
    for (int i = 1; i < p.terms; i++) {
        Term key = p.termArray[i];
        int j = i - 1;
        while (j >= 0 && p.termArray[j].exp < key.exp) {
            p.termArray[j + 1] = p.termArray[j];
            j--;
        }
        p.termArray[j + 1] = key;
    }
    int k = 0;
    for (int i = 0; i < p.terms; i++) {//合併相同指數
        if (k > 0 && p.termArray[k - 1].exp == p.termArray[i].exp)
            p.termArray[k - 1].coef += p.termArray[i].coef;
        else
            p.termArray[k++] = p.termArray[i];
    }
    int m = 0;
    for (int i = 0; i < k; i++)//移除係數變 0 的項
        if (p.termArray[i].coef != 0) p.termArray[m++] = p.termArray[i];
    p.terms = m;
    return in;
}
ostream& operator<<(ostream& out, const Polynomial& p) {
    if (p.terms == 0) { out << "0"; return out; }
    for (int i = 0; i < p.terms; i++) {
        float c = p.termArray[i].coef;
        int e = p.termArray[i].exp;
        if (i == 0) { if (c < 0) out << "-"; }
        else out << (c < 0 ? " - " : " + ");
        float a = fabs(c); //取絕對值
        if (e == 0) out << a;
        else {
            if (a != 1) out << a;
            out << "x";
            if (e != 1) out << "^" << e;
        }
    }
    return out;
}
int main() {
    Polynomial p1, p2, r;
    int choice;
    float x;
    do {
        cout << "\n===== 多項式計算機 =====\n";
        cout << "1. 輸入 A\n";
        cout << "2. 輸入 B\n";
        cout << "3. 顯示 A 和 B\n";
        cout << "4. A + B\n";
        cout << "5. A * B\n";
        cout << "6. 帶入 x 求 A 的值\n";
        cout << "7. 帶入 x 求 B 的值\n";
        cout << "0. 離開\n";
        cout << "請選擇: ";
        cin >> choice;
        switch (choice) {
        case 1:
            cout <<"請輸入A:";
            cin >> p1;
            break;
        case 2:
            cout <<"請輸入B:";
            cin >> p2;
            break;
        case 3:
            cout <<"A ="<<p1<<endl;
            cout <<"B ="<<p2<<endl;
            break;
        case 4: {
            clock_t start = clock();
            r = p1.Add(p2);
            clock_t end = clock();
            cout << "A + B = " << r << endl;
            cout << "Add花了:" << (double)(end - start) * 1000000.0 / CLOCKS_PER_SEC << " us" << endl;//因為執行速度太快它讀不到
            break;
        }
        case 5: {
            clock_t start1 = clock();
            r = p1.Mult(p2);
            clock_t end1 = clock();
            cout << "A * B = " << r << endl;
            cout << "Mult花了:" << (double)(end1 - start1) * 1000000.0 / CLOCKS_PER_SEC << " us" << endl;//因為執行速度太快它讀不到
            break;
        }
        case 6: {
            cout << "x = ";
            cin >> x;
            clock_t start2 = clock();
            float ans = p1.Eval(x);
            clock_t end2 = clock();
            cout << "A("<< x <<") = " << p1.Eval(x) << endl;
            cout << "Eval花了:" << (double)(end2 - start2) * 1000000.0 / CLOCKS_PER_SEC << " us" << endl;//因為執行速度太快它讀不到
            break;
        }
        case 7: {
            cout << "x = ";
            cin >> x;
            clock_t start3 = clock();
            float ans = p2.Eval(x);
            clock_t end3 = clock();
            cout << "B(" << x << ") = " << p2.Eval(x) << endl;
            cout << "Eval花了:" << (double)(end3 - start3) * 1000000.0 / CLOCKS_PER_SEC << " us" << endl;//因為執行速度太快它讀不到
            break;
        }
        case 0:
            cout << "離開程式\n";
            break;
        default:
            cout << "無效選項\n";
        }
    } while (choice != 0);
    return 0;
}