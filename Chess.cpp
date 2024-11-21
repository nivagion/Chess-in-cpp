//pazi y dolazi prije x, [y][x]
/*TO DO
	gameover

	castling
	en passant
	position to FEN
*/
#include<iostream>
using namespace std;

bool isWithinBoard(int x, int y);
bool isValidPieceMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isValidPawnMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isValidRookMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isValidKnightMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isValidBishopMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isValidQueenMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isValidKingMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]);
bool isKingInCheck(char board[8][8], char king, char currentPlayer);
bool isUnderAttack(int x, int y, char board[8][8], char currentPlayer);

struct extraStates{
    char onMove;
    bool whiteKingCastle;
    bool whiteQueenCastle;
    bool blackKingCastle;
    bool blackQueenCastle;
    string enPassant;
    int fiftyMoveRule; // left number
    int totalMoves; // right, last number
    extraStates(){
        onMove = 'B';
        whiteQueenCastle = true;
        whiteQueenCastle = true;
        blackKingCastle = true;
        blackQueenCastle = true;
        enPassant = '-';
        fiftyMoveRule = 0;
        totalMoves = 0;
    }
};

void FENtoBoard(string FEN, char board[8][8], extraStates& states){
    int i = 0;
    int ii = 0;
    int jj = 0;
    while(i < (int)FEN.length() && ii < 8){
        if(FEN[i]== '/'){
            ii++;
            jj=0;
            i++;
        }else if(FEN[i]== '1' || FEN[i]== '2' || FEN[i]== '3' || FEN[i]== '4' || FEN[i]== '5' || FEN[i]== '6' || FEN[i]== '7' || FEN[i]== '8'){
            int pomak = FEN[i] - '0';
            for(int t=0;t<pomak;t++){
                board[ii][jj] = '.';
                jj++;
            }
            i++;
        }else{
            board[ii][jj] = FEN[i];
            i++;
            jj++;
        }

    }

}

void printBoard(char board[8][8]){
	cout << "   A B C D E F G H\n\n";
    for(int t=0; t<8; t++){
        cout << 8 - t << "  ";
        for(int d=0; d<8; d++){
            cout << board[t][d] << " ";
        }
        cout <<" "<<8 - t<< "\n";
    }
    cout << " \n   A B C D E F G H\n";
}

void printDebugBoard(char board[8][8]){
	cout << "   0 1 2 3 4 5 6 7\n\n";
    for(int t=0; t<8; t++){
        cout <<  t << "  ";
        for(int d=0; d<8; d++){
            cout << board[t][d] << " ";
        }
        cout <<" "<< t<< "\n";
    }
    cout << " \n   0 1 2 3 4 5 6 7\n";
}

void parseMove(string toParse, int* x1, int* y1, int* x2, int* y2, char currentPlayer) {
    *y1 = 8 - (toParse[1] - '0'); 
    *y2 = 8 - (toParse[4] - '0');
    *x1 = toParse[0] - 'a';
    *x2 = toParse[3] - 'a';
}

int playMove(char board[8][8], int x1, int y1, int x2, int y2, char currentPlayer){ // prvo y onda x
    char piece = board[y1][x1];

    //ako je u boardu ili je piece prazno polje
	if (!isWithinBoard(x2, y2) || piece == '.') return false;

	//ako je piece od igrača koji nije na redu
	if(islower(piece) && currentPlayer == 'W') return false;
	if(isupper(piece) && currentPlayer == 'B') return false;

    //ako potez nije legalan vrati false
    if(!isValidPieceMove(piece,x1,y1,x2,y2,board)) return false; // TO TEST

    //simuliramo potez i gledamo dal nam je kralj u checku
    char temp = board[y2][x2];
    board[y2][x2] = board[y1][x1];
    board[y1][x1] = '.';
    //ako je kralj u checku, onda vrati proslu poziciju na polje i returnaj false
    if(isKingInCheck(board, currentPlayer == 'W' ? 'K' : 'k', currentPlayer)){
    	board[y1][x1] = piece;
    	board[y2][x2] = temp;
    	return false;
    }
    return true;
}

bool isValidPieceMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]) {
    switch (tolower(piece)) {
        case 'p': return isValidPawnMove  (piece, x1, y1, x2, y2, board);
        case 'r': return isValidRookMove  (piece, x1, y1, x2, y2, board);
		case 'n': return isValidKnightMove(piece, x1, y1, x2, y2, board);
		case 'b': return isValidBishopMove(piece, x1, y1, x2, y2, board);
		case 'q': return isValidQueenMove (piece, x1, y1, x2, y2, board);
		case 'k': return isValidKingMove  (piece, x1, y1, x2, y2, board);
        default: return false;
    }
}

bool isValidPawnMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]){
	int direction = piece == 'P' ? -1 : 1; // bijeli +1, crni -1
	if(x1 == x2){ // isti red
		if(y2 == y1 + direction && board[y2][x2] == '.'){
			return true;
		}// ako je pawn tu, i ako je finalno polje tocno, i ako je put do tamo prazan
		if(y1 == 6 && piece == 'P' && y2 == y1 + 2 * direction){
			if(board[y1 + direction][x2] == '.' && board[y2][x2] == '.') return true;
		}

		if(y1 == 1 && piece == 'p' && y2 == y1 + 2 * direction){
			if(board[y1 + direction][x2] == '.' && board[y2][x2] == '.') return true;
		}
	}else if(abs(x2-x1) == 1 && y2 == y1 + direction){//capture
		if(board[y2][x2] != '.'){
			if((piece == 'P' && islower(board[y2][x2])) || (piece == 'p' && isupper(board[y2][x2])))
				return true;
		}
	}
	return false;
}	
bool isValidRookMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]){
	if(x1 != x2 && y1 != y2) return false; // dijagonalan move
	// ako je isti red, onda je put po stupcu, zato je dx onda 0
	// inace gledamo dal idemo lijevo ili desno, zato 1 : -1
	int dx = (x2 - x1) == 0 ? 0 : (x2 > x1 ? 1 : -1);
	int dy = (y2 - y1) == 0 ? 0 : (y2 > y1 ? 1 : -1);
	int x = x1 + dx;
	int y = y1 + dy;

	while(x != x2 || y != y2){//checkaj put
		if(board[y][x] != '.') return false;
		x += dx;
		y += dy;
	}
	//ako polje nije prazno ili nije opp na polju
	if(board[y2][x2] != '.' && ((piece == 'R' && isupper(board[y2][x2])) || (piece == 'r' && islower(board[y2][x2])))){
		return false;
	}

	return true;
}
bool isValidKnightMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]){
	int dx = abs(x2 - x1);
	int dy = abs(y2 - y1);

	if((dx == 2 && dy == 1) || (dx == 1 && dy == 2)){
		if(board[y2][x2] == '.') return true;
		//ako je destination lowercase, onda piece mora bit upper da bi bilo true
		if(islower(board[y2][x2] != islower(piece))) return true;
	}
	return false;
}
bool isValidBishopMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]){
	if(abs(x2 - x1) != abs(y2 - y1)){ // ako nije dijagonalan
		return false; 
	}//u kojem smjeru idemo
	int dx = (x2 > x1) ? 1 : -1;
    int dy = (y2 > y1) ? 1 : -1;
    int x = x1 + dx;
    int y = y1 + dy;
    //while slican kao i za rooka
    while (x != x2 && y != y2) {
        if (board[y][x] != '.')return false;
        x += dx;
        y += dy;
    }
    //target square
    if(board[y2][x2] == '.') return true;
    //ako je destination lowercase, onda piece mora bit upper da bi bilo true
	if(islower(board[y2][x2] != islower(piece))) return true;
}
bool isValidQueenMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]){
	if (x1 != x2 && y1 != y2 && abs(x2 - x1) != abs(y2 - y1)) {
        return false;
    }
    // ako je x2 veci onda idemo u desno, inace ulijevo, inace se micemo ravno i ostajemo na tom ranku
    int dx = (x2 > x1) ? 1 : (x2 < x1) ? -1 : 0;
    int dy = (y2 > y1) ? 1 : (y2 < y1) ? -1 : 0;
    int x = x1 + dx;
    int y = y1 + dy;

    while (x != x2 || y != y2) {
        if (board[y][x] != '.')return false;
        x += dx;
        y += dy;
    }
    if(board[y2][x2] == '.') return true;
	if(islower(board[y2][x2] != islower(piece))) return true;
}
bool isValidKingMove(char piece, int x1, int y1, int x2, int y2, char board[8][8]){
	int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    //pomak samo za 1 polje
    if ((dx <= 1 && dy <= 1) && (dx + dy > 0)){
    	if(board[y2][x2] == '.')return true;
    	if(islower(board[y2][x2]) != islower(piece))return true;
    }
    //ako nije ovo gore onda je false
    return false;
}

bool isKingInCheck(char board[8][8], char king, char currentPlayer) {
    int kingX, kingY;
    //nadi kinga
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board[i][j] == king) {
                kingY = i;
                kingX = j;
                break;
            }
        }
    }

    //provjeravamo ako je napadnut
    return isUnderAttack(kingX, kingY, board, currentPlayer);
}

bool isUnderAttack(int x, int y, char board[8][8], char currentPlayer) {
    int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1},  // Rook directions
                            {1, 1}, {-1, -1}, {1, -1}, {-1, 1}}; // Bishop directions

    // rook and queen check vertical+horizontal
    for (int i = 0; i < 4; i++) {
        int dx = directions[i][0], dy = directions[i][1];
        int nx = x + dx, ny = y + dy;
        while (isWithinBoard(nx, ny)) {
            char piece = board[ny][nx];
            if (piece != '.') {
                if ((currentPlayer == 'W' && islower(piece)) || (currentPlayer == 'B' && isupper(piece))) {
                    if (tolower(piece) == 'r' || tolower(piece) == 'q') return true;
                }
                break;
            }
            nx += dx;
            ny += dy;
        }
    }
    // bishop and queen check diagonally
    for (int i = 4; i < 8; i++) {
        int dx = directions[i][0], dy = directions[i][1];
        int nx = x + dx, ny = y + dy;
        while (isWithinBoard(nx, ny)) {
            char piece = board[ny][nx];
            if (piece != '.') {
                if ((currentPlayer == 'W' && islower(piece)) || (currentPlayer == 'B' && isupper(piece))) {
                    if (tolower(piece) == 'b' || tolower(piece) == 'q') return true;
                }
                break;
            }
            nx += dx;
            ny += dy;
        }
    }
    // knighs
    int knightMoves[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
    for (int i = 0; i < 8; i++) {
        int nx = x + knightMoves[i][0], ny = y + knightMoves[i][1];
        if (isWithinBoard(nx, ny) && ((currentPlayer == 'W' && islower(board[ny][nx])) || (currentPlayer == 'B' && isupper(board[ny][nx])))) {
            if (tolower(board[ny][nx]) == 'n') return true;
        }
    }
    // pawn attacks
    int pawnDir = (currentPlayer == 'W') ? -1 : 1;
    int pawnAttackX[2] = {x + 1, x - 1};
    int pawnAttackY = y + pawnDir;
    for (int i = 0; i < 2; i++) {
        if (isWithinBoard(pawnAttackX[i], pawnAttackY) &&
            ((currentPlayer == 'W' && islower(board[pawnAttackY][pawnAttackX[i]])) ||
             (currentPlayer == 'B' && isupper(board[pawnAttackY][pawnAttackX[i]]))) &&
            tolower(board[pawnAttackY][pawnAttackX[i]]) == 'p') return true;
    }
    /*// king, nikad nije napadnut od kralja tho
    int kingMoves[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int i = 0; i < 8; i++) {
        int nx = x + kingMoves[i][0], ny = y + kingMoves[i][1];
        if (isWithinBoard(nx, ny) && ((currentPlayer == 'W' && islower(board[ny][nx])) || (currentPlayer == 'B' && isupper(board[ny][nx])))) {
            if (tolower(board[ny][nx]) == 'k') return true;
        }
    }*/
    return false;  //not under attack
}


bool isWithinBoard(int x, int y) {
    return x>=0 && x<8 && y>=0 && y<8;
}

bool gameOver(){
	// dobijemo board, tko je na potezu
	// gledamo dal je king koji je na potezu napadnut
	// gledamo dal možemo pomaknut kralja na neko drugo mjesto di nije napadnut
	// simuliramo poteze i gledamo dal možemo maknut kralja iz checka
	// ako ne game over je 
}
    



int main(){
      ios_base::sync_with_stdio(true);
      cin.tie(NULL);

      string startingFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
      string inputFEN;inputFEN = startingFEN;

      char board[8][8] = {};

      extraStates states;

      FENtoBoard(inputFEN, board, states);

      string parseThis; // parsing moves

    for(int i=0; ;i++){

        states.onMove == 'B' ? states.onMove = 'W' : states.onMove = 'B';
        if_move_isnt_found:
        printBoard(board);
        states.onMove == 'B' ? cout<<"Black to move\n" : cout<<"White to move\n";
        cout<<"enter move in this format:e2,e4\n";
        cin>>parseThis;
        int x1,y1,x2,y2;
        parseMove(parseThis, &x1, &y1, &x2, &y2, states.onMove);
        if(!playMove(board, x1, y1, x2, y2, states.onMove)){
        	cout<<"illegal move, enter new one;\n";
        	goto if_move_isnt_found;
        }// ako je potez nelegalan onda ide opet do inputa
      }

    return 0;
}