#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <tr1/unordered_map>

using namespace std;

// BULANELI ***************************************

#define bias 10                                 /**/
#define culoarePartiala(x) (x * x * 2)          /**/
#define chintaPartiala(x) (x * 4)               /**/

//*************************************************

int MAX_DEPTH = 4;
#define INF (1 << 30) - 1

string nToS[20];
string sToS[5];

unsigned long long p[4] = {65537, 666013, 1e9 + 7, 3241618767LL};

tr1::unordered_map<unsigned long long, double> H;

struct __attribute__((__packed__)) card {
    unsigned int number: 4;
    unsigned int suit: 2;

    card():
        number(0),
        suit(0){}

    card(int n, int s):
        number(n),
        suit(s){}

    card(int code):
        number(code / 10 + 9),
        suit  (code % 10){}

    const int toInt() const {
        return (number - 9) * 10 + suit;
    }

    const string toString() const {
        return nToS[number] + sToS[suit];
    }
};


struct __attribute__((__packed__)) gameState {
    card col[4][5];
    int at[4];

    void print() {
        for(int i = 0 ; i < 4 ; ++i) {
            cerr << "col " << (i + 1) << ": ";

            for(int j = 0 ; j < at[i] ; ++j) {

                cerr << col[i][j].toString() << " ";

            }

            cerr << "\n";
        }
    }

    unsigned long long colToInt(const int i) const {
        unsigned long long ret = 0, P = p[i];
        for(int j = 0 ; j < at[i] ; ++j) {
            ret += col[i][j].toInt() * P;
            P *= p[i];
        }

        return ret;
    }

    unsigned long long toInt() const {
        unsigned long long ret = 0;

        for(int i = 0 ; i < 4 ; ++i)
            ret += colToInt(i);

        return ret;
    }

    const double value (const int i) const {
        double ret = 0;
        int ap[16];
        int color[4];
        memset (ap, 0, sizeof (ap));
        memset (color, 0, sizeof (color));

        for (int j = 0; j < at[i]; j ++){
            ap[col[i][j].number] ++;
            color[col[i][j].suit] ++;
        }

        int nrPairs = 0;
        int three = 0;
        int careu = 0;
        int maxColor = 0;
        bool chinta = 0;
        int nrCulori = 0;
        int nrCartiDif = 0;

        for (int j = 9; j <= 15; j ++){
            if (ap[j] == 2)
                nrPairs ++;
            else
                if (ap[j] == 3)
                    three ++;
                else
                    if (ap[j] == 4)
                        careu ++;

            if (ap[j])
                nrCartiDif ++;
        }

        for (int j = 0; j < 4; j ++){
            if (color[i] > maxColor)
                maxColor = color[i];

            if (color[i])
                nrCulori ++;
        }

        if (nrPairs == 1 && three == 1){
            ret = 20;
        }
        else{
            if (careu == 1){
                ret = 30;
            }
            else{
                if (three == 1){
                    ret = 5;
                }
                else{
                    if (nrPairs == 2){
                        ret = 2;
                    }
                    else
                        if (nrPairs == 1){
                            ret = 1;
                        }
                }
            }
        }
        if (maxColor == 5){
            ret = 500;
        }

        chinta |= (ap[9] & ap[10] & ap[12] & ap[13] & ap[14]);
        chinta |= (ap[10] & ap[12] & ap[13] & ap[14] & ap[15]);
        if (chinta && maxColor == 5){
            ret = 100;
        }
        else{
            if (chinta){
                ret = 10;
            }
        }

        return ret;
    }

    const double potential (const int i) const {

        double ret = 0;

        int ap[16];
        int color[4];
        memset (ap, 0, sizeof (ap));
        memset (color, 0, sizeof (color));

        for (int j = 0; j < at[i]; j ++){
            ap[col[i][j].number] ++;
            color[col[i][j].suit] ++;
        }

        int nrCartiDif = 0;
        int nrCulori = 0;

        for (int j = 9; j <= 15; j ++){
            if (ap[j])
                nrCartiDif ++;
        }

        for (int j = 0; j < 4; j ++){
            if (color[i])
                nrCulori ++;
        }

        if (nrCulori == 1){
            ret += culoarePartiala(at[i]);
        }
        if (nrCartiDif == at[i]){
            ret += chintaPartiala(at[i]);
        }

        return ret;
    }

    const double potential() const {
        double ret = 0;

        for(int i = 0 ; i < 4 ; ++i)
            ret += potential(i);

        return ret;
    }

    const double value() const {
        double ret = 0;

        for(int i = 0 ; i < 4 ; ++i)
            ret += value(i);

        return ret;
    }

    const double score() const {

        return value() * bias + potential();
    }

    gameState() {
        memset(&col[0][0], 0, sizeof(col));
        memset(&at[0], 0, sizeof(at));
    }

    gameState(const gameState &X) {
        for(int i = 0 ; i < 4 ; ++i) {
            at[i] = X.at[i];
            for(int j = 0 ; j < 5 ; ++j)
                col[i][j] = X.col[i][j];
        }
    }

    const int getLevel() const {
        int mx = 0;
        for(int j = 0 ; j < 4 ; ++j)
                if(at[j] > mx)
                    mx = at[j];
        return mx;
    }

    void place(const int where, const card & c) {
        col[where][at[where]] = c;
        ++at[where];
    }

    int remove(int where) {
        --at[where];
        col[where][at[where]] = 0;
    }

    const int getMoves() const {
        int mx = getLevel();
        int ret = 0;

        for(int j = 0 ; j < 4 ; ++j) {
            if(at[j] < mx)
                ret = ret ^ (1 << j);
        }

        if(ret == 0)
            ret = (1 << 4) - 1;

        return ret;
    }
};

double expectimax(const gameState &now, const int &depth);
double consider(const gameState &now, const card &c, const int &depth);
void populate(const gameState &now);


card full[24];
card rms[24];
bool viz[10000];
int at;

void populate(const gameState &now) {

    for(int i = 0 ; i < 24 ; ++i) {
        viz[full[i].toInt()] = 0;
    }


    for(int i = 0 ; i < 4 ; ++i) {
        for(int j = 0 ; j < now.at[i] ; ++j) {
            viz[now.col[i][j].toInt()] = 1;
        }
    }

    at = 0;

    for(int i = 0 ; i < 24 ; ++i) {
        if(viz[full[i].toInt()])
            continue;
        rms[at] = full[i];
        ++at;
    }
}

double consider(const gameState &now, const card &c, const int &depth) {
    int nxt = now.getMoves();
    double ret = -INF;

    gameState next = now;

    for(int j = 0 ; j < 4 ; ++j) {
        if((1 << j) & nxt) {

            next.place(j, c);
            double n = expectimax(next, depth + 1);
            if(n > ret)
                ret = n;

            next.remove(j);
        }
    }

    return ret;
}

double expectimax(const gameState &now, const int &depth) {
    if(depth >= MAX_DEPTH)
        return now.score();

    if(H.count(now.toInt())) {
        return H[now.toInt()];
    }

    populate(now);

    if(at <= 5)
        return now.score();

    card * test = new card[at];
    for(int i = 0 ; i < at ; ++i)
        test[i] = rms[i];

    double ret = 0;

    for(int i = 0 ; i < at ; ++i)
        ret += consider(now, test[i], depth);

    ret /= (double)at;
    delete[] test;

    return (H[now.toInt()] = ret);
}

card readCard() {
    string s;
    cin >> s;
    char suit = s[1];
    int number;
    if(s[0] == 'K')
        number = 14;
    if(s[0] == 'Q')
        number = 13;
    if(s[0] == 'J')
        number = 12;
    if(s[0] == 'A')
        number = 15;
    if(s[0] == '9')
        number = 9;
    if(s[0] == '1') {
        number = 10;
        suit = s[2];
    }

    int st;

    if(suit == 'D')
        st = 0;
    if(suit == 'C')
        st = 1;
    if(suit == 'H')
        st = 2;
    if(suit == 'S')
        st = 3;

    return card(number, st);
}

void init() {

    nToS[9] = "9";
    nToS[10] = "10";
    nToS[12] = "J";
    nToS[13] = "Q";
    nToS[14] = "K";
    nToS[15] = "A";

    sToS[0] = "D";
    sToS[1] = "C";
    sToS[2] = "H";
    sToS[3] = "S";

    int l = 0;
    for(int i = 9 ; i <= 15 ; ++i) {
        if(i == 11)
            continue;

        for(int j = 0 ; j < 4 ; ++j) {
            full[l] = card(i, j);
            ++l;
        }
    }
}

inline int coef(int x) {
    int ret = x % 4;
    if(ret == 0)
        ret = 4;
    return ret;
}

void calculateDepth(const gameState &now) {
    populate(now);

    //at = cate carti mai sunt de pus

    unsigned long long tot = 1;
    int i;

    for(i = 0 ; tot <= 1020LL * 1000 * 1000 && i <= 10 ; ++i) {
        tot *= (at - i) * coef(at - i);
    }
    --i;

    if(at >= 21)
        MAX_DEPTH = 1;
    else
        MAX_DEPTH = max(4, i);
}

int main() {
    init();

    freopen ("data.in", "r", stdin);

    gameState now;

    for(int i = 0 ; i < 20 ; ++i) {

        H.clear();

        calculateDepth(now);

        card c = readCard();

        int nxt = now.getMoves();
        int best = -INF;
        int mv = 0;
        for(int j = 0 ; j < 4 ; ++j) {
            if((1 << j) & nxt) {

                now.place(j, c);
                int n = expectimax(now, 1);

                if(n > best) {
                    best = n;
                    mv = j;
                }

                now.remove(j);
            }
        }

        now.place(mv, c);

        cout << (mv + 1) << "\n";
    }

    cerr << now.value() << "\n";

    return 0;
}
