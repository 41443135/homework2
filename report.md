# 41443135

作業二

## 解題說明
想法<br>
Term類別：用來儲存多項式中的單一非零項,包含coef(係數)與exp(指數)
Polynomial類別：使用動態陣列termArray來儲存所有非零項,並維持指數由大到小排序,同指數只會出現一次<br>
newTerm(新增項):如果陣列容量滿了,它就會容量倍,並增配置一個更大的新陣列，將舊資料複製過去，並釋放舊陣列的記憶體<br>
operator>>:讀入後做三件事1.排序2.合併指數,因為每個指數只能有一項,使用者輸入時可能不造規則寫<br>
例如輸入像3X^2+5X^2,就會把相同指數的係數加起來,3.自動清除係數為0的項<br>
Add:用兩個指標分別指著兩個多項式去比對 指數一樣就把係數相加,如果指數不一樣,就把指數比較大的那一項先放進結果裡<br>
Mult:用雙層迴圈,拿A的每一項去乘B的每一項,把乘出來的結果丟進暫存的temp裡面,然後用Add一行一行加到總結果裡面<br>
Eval:把每一個非零項拿出來,用coef*pow(x,exp)算出數值,全部加起來就是答案<br>
##舉例說明
(2x+1)(x+3):
|a的項 |乘所B所有項得到的temp|result(用Add累積)|
|---|---|---|
|2x    | 2x^2,6x          |2x^2+6x             |
|1    | x,3                |2x^2+7x+3          |
### Algorithm Design&Programming
## 輸入格式
項數 係數 指數 係數 指數...,例如 `3 4 2 1 1 -2 0` 代表 4x^2 + x - 2
## 選單功能<br>
1.輸入A<br>
2.輸入B<br>
3.顯示A和B<br>
4.A+B<br>
5.A*B<br>
6.帶入x求A的值<br>
7.帶入x求B的值<br>
0.離開<br>
## 程式實作
```cpp
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
```
## 效能分析
設A有N項,B有M項<br>
|函式      |          時間複雜度        |額外空間    |
|---|---|---|
|newTerm   |均攤O(1)(擴充時為O(terms))|O(1)      |
|Add       |O(N+M)                     |O(N+M)     |
|Eval      |O(n)                       |O(1)       |
|operator<<|O(n)                       |O(1)       |
|operator>>|最佳O(n),最差O(n^2)     |O(1)       |
|MulT      |最差O(n^2*m)   |O(nm)     |
## 說明
newTerm:因為動態陣列採用容量每次翻倍,雖然當陣列滿了需要搬移資料時是O(\text{terms}),但平均分攤下來,每一項其實只要O(1)的時間<br>
Add:兩個多項式都已排序,雙指標雙指標一路走到底,每項只處理一次,時間就是兩個多項式項數相加O(n+m)<br>
operator>>:讀入O(n),插入排序在輸入完全反序時最差O(n^2),已排序時O(n),合併與移除零項各O(n)<br>
Mult:外層迴圈跑n次,每圈建立temp為O(m),再做一次Add與一次operator,此時result最多有i·m項 總成本約m+2m+...+nm,即O(n^2*m) 若指數範圍小、乘積大量重複,結果項數受限,實際會比最差情況快<br>
## 測試與驗證
### 測試案例
|測試目的|A輸入|B輸入|預期結果|
|---|---|---|---|
|基本|2 1 1 1 0|2 1 1 1 0|A+B=2X+2:A*B=X^2+2+1|
|係數相消|2 1 1 1 0|2 -1 1 -1 0|A+B=0:A*B=-x^2-2x+1|
|亂序家重複的指數|5 2 1 3 3 -2 1 0 5 4 0|1 1 0|A=3x^3+4|
|零多項式|0|2 1 1 1 0|A=0:A+B=x+1:A*B=0|
|代入求值|3 2 2 3 1 5 0||x=2時,A(2)=19|
|小數|2 1.5 2 -0.5 0||x=2,A(2)=5.5;顯示1.5x^2-0.5|

### 效能測試
一般輸入在100項以內的日常測資時,因為演算法(雙指標與動態陣列)的效率極高,輸出測量值幾乎都在幾十微秒甚至個位數微秒內完成
## 申論及開發報告
### 心得討論
其實一開始看這份作業的時候,感覺什麼都看不懂,大部分的指標跟類別的寫法都是參照助教給我們的資料寫的,不然就是去問ai出來的,雖然說,
到現在我可能還是沒辦法完全理解,但相較剛開始,我覺得我學到很多關於c++的用法跟程式,這跟大一上課學的c++感覺難度根本就不一樣,但我覺得寫完的成就感,讓我又重回了我對剛開始學習程式的那份熱愛,雖然說真的難了不止一點,但至少我覺得我有在進步
