//definition of a board
#include <bitset>
using std::bitset;

// The four sets of 
typedef struct Board {
    bitset<64> pieces[4];
    bitset<32> flags;

    int get(int pos) {
        return (pieces[0][pos] << 3) | (pieces[1][pos] << 2) | (pieces[2][pos] << 1) | (pieces[3][pos]);
    }

    void set(int pos, int val) {
        pieces[0][pos] = (val >> 3) & 1;
        pieces[1][pos] = (val >> 2) & 1;
        pieces[2][pos] = (val >> 1) & 1;
        pieces[3][pos] = val & 1;
    }
} Board;

/*int main() {
    Board b;
    b.set(0, 9);
    printf("%d", b.get(0));
    return 0;
}*/
