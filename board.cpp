//definition of a board

/* I use the convention that one byte [xxxx-xxxx] stores the data at the even index in high bits,
   the odd in low bits */
   
unsigned LOW_MASK = 15;
unsigned HIGH_MASK = 240;

typedef struct Board {

    uint_fast8_t squares[32];
    uint_fast32_t conf;

    int get(int pos) {
        uint_fast8_t byte = squares[pos/2];
        return (pos % 2 == 0) ? (byte >> 4) : (byte & LOW_MASK);
    }
    
    void set(int pos, uint_fast8_t val) {
        if (pos % 2 == 0) {
            squares[pos/2] = (val << 4) | (squares[pos/2] & LOW_MASK);
        } else {
            squares[pos/2] = (squares[pos/2] & HIGH_MASK) | val;
        }
    }

} Board;

/*int main() {
    Board b;
    b.set(0, 9);
    printf("%d", b.get(0));
    return 0;
}*/
