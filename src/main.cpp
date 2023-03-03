#include <SFML/Graphics.hpp>
#include "Board.hpp"
#define TS 100
#define PS 280
sf::Sprite& pieceToSprite(ce::Piece piece, sf::Sprite sprites[]){
    switch (piece)
    {
    case ce::Piece::WKING:
        return sprites[0];
        break;
    case ce::Piece::WQUEEN:
        return sprites[1];
        break;
    case ce::Piece::WBISHOP:
        return sprites[2];
        break;
    case ce::Piece::WKNIGHT:
        return sprites[3];
        break;
    case ce::Piece::WROOK:
        return sprites[4];
        break;
    case ce::Piece::WPAWN:
        return sprites[5];
        break;
    case ce::Piece::BKING:
        return sprites[6];
        break;
    case ce::Piece::BQUEEN:
        return sprites[7];
        break;
    case ce::Piece::BBISHOP:
        return sprites[8];
        break;
    case ce::Piece::BKNIGHT:
        return sprites[9];
        break;
    case ce::Piece::BROOK:
        return sprites[10];
        break;
    case ce::Piece::BPAWN:
        return sprites[11];
        break;
    default:
        return sprites[0];
        break;
    }
}
ce::Piece nextPromotingPiece(ce::Piece cur){
    switch (cur)
    {
    case ce::Piece::WQUEEN:
        return ce::Piece::WROOK;
        break;
    case ce::Piece::WROOK:
        return ce::Piece::WKNIGHT;
        break;
    case ce::Piece::WKNIGHT:
        return ce::Piece::WBISHOP;
        break;
    default:
        return ce::Piece::WQUEEN;
        break;
    }
}
std::string pieceName(ce::Piece cur) {
    switch (cur)
    {
    case ce::Piece::WQUEEN:
        return "queen";
        break;
    case ce::Piece::WROOK:
        return "rook";
        break;
    case ce::Piece::WKNIGHT:
        return "knight";
        break;
    default:
        return "bishop";
        break;
    }
}
int main(){
    sf::RenderWindow win;
    win.create(sf::VideoMode(TS*8, TS*8), "chess", sf::Style::Close | sf::Style::Titlebar);
    sf::Event e;
    sf::RectangleShape tile;
    tile.setSize(sf::Vector2f(TS, TS));
    ce::Board board;
    sf::Texture pieceTextures[12];
    sf::Sprite pieceSprites[12];
    for(int i = 0;i < 2;i++){
        for(int j = 0;j < 6;j++){
            pieceTextures[i*6 + j].loadFromFile("res/sprites.png", sf::IntRect(j*(PS+50), i*(PS+50), PS, PS));
            pieceSprites[i*6+j].setTexture(pieceTextures[i*6+j]);
            pieceSprites[i*6+j].setScale(TS*1./PS, TS*1./PS);
        }
    }
    ce::Piece promoting = ce::Piece::WQUEEN;
    ce::uchar selected = 255;
    ce::Move accessible[64];
    while(win.isOpen()){
        while(win.pollEvent(e)){
            if(e.type == sf::Event::Closed){
                win.close();
            } else if(e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left){
                ce::uchar row = e.mouseButton.y / TS;
                ce::uchar col = e.mouseButton.x / TS;
                ce::uchar pos = row<<3|col;
                if((board.getTurn() && board.getPiece(row, col) < 0) || (!board.getTurn() && board.getPiece(row, col) > 0)){
                    selected = row<<3|col;
                    std::vector<ce::Move> moves;
                    board.getMoves(selected, moves);
                    for(ce::uchar i = 0;i < 64;i++) accessible[i] = ce::Move();
                    for(ce::Move move : moves) {
                        if(move.target > 63) accessible[(board.getTurn()*7<<3)+(move.target&7)] = move;
                        else accessible[move.target] = move;
                    }
                } else if(accessible[pos].source == 255) {
                    selected = 255;
                    for(ce::uchar i = 0;i < 64;i++) accessible[i] = ce::Move();
                    continue;
                } else {
                    if(accessible[pos].target > 63){
                        accessible[pos].target = 64+(accessible[pos].target&7)+(board.getTurn()*7<<3)+promoting*8;
                    }
                    board.makeMove(accessible[pos]);
                    selected = 255;
                    for(ce::uchar i = 0;i < 64;i++) accessible[i] = ce::Move();
                }
            } else if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space) promoting = nextPromotingPiece(promoting);
        }
        win.clear();
        for(ce::uchar i = 0;i < 8;i++) for(ce::uchar j = 0;j < 8;j++){
            tile.setFillColor((i+j)%2 ? sf::Color(85, 35, 27) : sf::Color(206, 202, 202));
            tile.setPosition(j*TS, i*TS);
            win.draw(tile);
            if(accessible[i<<3|j].source != 255) {
                tile.setFillColor(sf::Color(0, 255, 0, 30));
                win.draw(tile);
            }
            if(board.getPiece(i, j) == ce::Piece::NONE) continue;
            sf::Sprite& sp = pieceToSprite(board.getPiece(i, j), pieceSprites);
            sp.setPosition(j*TS, i*TS);
            win.draw(sp);
        }
        sf::Sprite& sp = pieceToSprite(promoting, pieceSprites);
        sp.setScale(0.125, 0.125);
        sp.setPosition(0, TS*4);
        win.draw(sp);
        sp.setScale(TS*1./PS, TS*1./PS);
        win.display();
    }
}