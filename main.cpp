#include <bits/stdc++.h>

using namespace std;

mutex mut;

class Field {
public:
    Field() {
        coordinates=make_pair(1, 1);
    }
    Field (int x, int y) {
        coordinates=make_pair(x, y);
    }
    Field (pair<int, int> coors) {
        coordinates=coors;
    }
    Field (string s) {
        coordinates=make_pair((s[0]-'a')+1, (s[1]-'0'));
    }
    bool IsLegal() const {
        if (coordinates.first>8 || coordinates.second>8 || coordinates.first<1 || coordinates.second<1) {
            return false;
        } else return true;
    }
    string GetString() const {
        string s="a1";
        s[0]='a'+coordinates.first-1;
        s[1]='0'+coordinates.second;
        return s;
    }
    pair<int, int> GetCoordinates() const {
        return coordinates;
    }
    bool operator<(Field other) const {
        if (coordinates.first<other.coordinates.first ||
        (coordinates.first==other.coordinates.first && coordinates.second<other.coordinates.second)) {
            return true;
        } else return false;
    }
    bool operator==(Field other) const {
        if (coordinates.first==other.coordinates.first && coordinates.second==other.coordinates.second) {
            return true;
        } else return false;
    }
    Field operator+(pair<int, int> para) const {
        Field f(coordinates.first+para.first, coordinates.second+para.second);
        return f;
    }
    void operator=(Field other) {
        coordinates=other.coordinates;
    }
private:
    pair<int, int> coordinates;
};

class Move {
public:
    Move(Field f1, Field f2, int x=0) : start(f1), finish(f2), transformation(x) {}
    Move(string s) {
        if (s=="0-0") {
            transformation=5;
            start=Field("a1");
            finish=Field("a1");
        } else if (s=="0-0-0") {
            transformation=6;
            start=Field("a1");
            finish=Field("a1");
        } else {
            Field f1(s.substr(0, 2));
            start = f1;
            Field f2(s.substr(3, 2));
            finish = f2;
            transformation = 0;
            if (s.size() == 6) {
                if (s[5] == 'N') transformation = 1;
                if (s[5] == 'B') transformation = 2;
                if (s[5] == 'R') transformation = 3;
                if (s[5] == 'Q') transformation = 4;
            }
        }
    }
    string GetString() const {
        if (transformation==5) {
            return "0-0";
        } else if (transformation==6) {
            return "0-0-0";
        }
        string s="";
        s+=start.GetString();
        s+="-";
        s+=finish.GetString();
        if (transformation==1) s+="N";
        if (transformation==2) s+="B";
        if (transformation==3) s+="R";
        if (transformation==4) s+="Q";
        return s;
    }
    int GetTransformation() const {
        return transformation;
    }
    pair<Field, Field> GetFields() const {
        return make_pair(start, finish);
    }
    void operator=(const Move& other) {
        start=other.start;
        finish=other.finish;
        transformation=other.transformation;
    }
    bool operator<(Move other) const {
        return true;
    }
private:
    Field start;
    Field finish;
    int transformation;
};

class Figure {
public:
    Figure(int c) : color(c) {
    }
    string GetShortName() const {
        return shortname;
    }
    int GetColor() const {
        return color;
    }
    int GetCost() const {
        return cost;
    }
    vector<pair<int, int>> GetDirections() const {
        return directions;
    }
    vector<pair<int, int>> GetMoves() const {
        return moves;
    }
protected:
    int color;
    int cost;
    string shortname;
    vector<pair<int, int>> directions;
    vector<pair<int, int>> moves;
};

class Pawn : public Figure {
public:
    Pawn(int c) : Figure(c) {
        shortname="";
        cost=1;
        if (c==0) {
            moves = {make_pair(1, 1), make_pair(-1, 1), make_pair(0, 1), make_pair(0, 2)};
        } else moves = {make_pair(1, -1), make_pair(-1, -1), make_pair(0, -1), make_pair(0, -2)};
    }
};

class Knight : public Figure {
public:
    Knight(int c) : Figure(c) {
        shortname="N";
        cost=3;
        moves = {make_pair(1, 2), make_pair(-1, 2), make_pair(2, 1),
                 make_pair(-2, 1), make_pair(1, -2), make_pair(-1, -2),
                 make_pair(2, -1), make_pair(-2, -1)};
    }
};

class Bishop : public Figure {
public:
    Bishop(int c) : Figure(c) {
        shortname="B";
        cost=3;
        directions={make_pair(1, 1), make_pair(1, -1), make_pair(-1, 1), make_pair(-1, -1)};
    }
};

class Rook : public Figure {
public:
    Rook(int c) : Figure(c) {
        shortname="R";
        cost=5;
        directions={make_pair(1, 0), make_pair(0, -1), make_pair(-1, 0), make_pair(0, 1)};
    }
};

class Queen : public Figure {
public:
    Queen(int c) : Figure(c) {
        shortname="Q";
        cost=10;
        directions={make_pair(1, 0), make_pair(0, -1), make_pair(-1, 0), make_pair(0, 1),
                    make_pair(1, 1), make_pair(1, -1), make_pair(-1, 1), make_pair(-1, -1)};
    }
};

class King : public Figure {
public:
    King(int c) : Figure(c) {
        shortname="K";
        cost=0;
        moves={make_pair(1, 0), make_pair(0, -1), make_pair(-1, 0), make_pair(0, 1),
                    make_pair(1, 1), make_pair(1, -1), make_pair(-1, 1), make_pair(-1, -1)};
    }
};

class Position {
public:
    Position() {
        Rook* r1 = new Rook(0);
        figures.emplace_back(make_pair(Field("a1"), 0), r1);
        figures.emplace_back(make_pair(Field("h1"), 0), r1);
        Rook* r3 = new Rook(1);
        figures.emplace_back(make_pair(Field("a8"), 0), r3);
        figures.emplace_back(make_pair(Field("h8"), 0), r3);
        Knight* n1=new Knight(0);
        figures.emplace_back(make_pair(Field("b1"), 0), n1);
        figures.emplace_back(make_pair(Field("g1"), 0), n1);
        Knight* n3= new Knight(1);
        figures.emplace_back(make_pair(Field("b8"), 0), n3);
        figures.emplace_back(make_pair(Field("g8"), 0), n3);
        Bishop* b1 = new Bishop(0);
        figures.emplace_back(make_pair(Field("c1"), 0), b1);
        figures.emplace_back(make_pair(Field("f1"), 0), b1);
        Bishop* b3 = new Bishop(1);
        figures.emplace_back(make_pair(Field("c8"), 0), b3);
        figures.emplace_back(make_pair(Field("f8"), 0), b3);
        King* k1 = new King(0);
        figures.emplace_back(make_pair(Field("e1"), 0), k1);
        Queen* q1 = new Queen(0);
        figures.emplace_back(make_pair(Field("d1"), 0), q1);
        King* k2 = new King(1);
        figures.emplace_back(make_pair(Field("e8"), 0), k2);
        Queen* q2 = new Queen(1);
        figures.emplace_back(make_pair(Field("d8"), 0), q2);
        Pawn* p1 = new Pawn(0);
        figures.emplace_back(make_pair(Field("a2"), 0), p1);
        figures.emplace_back(make_pair(Field("b2"), 0), p1);
        figures.emplace_back(make_pair(Field("c2"), 0), p1);
        figures.emplace_back(make_pair(Field("d2"), 0), p1);
        figures.emplace_back(make_pair(Field("e2"), 0), p1);
        figures.emplace_back(make_pair(Field("f2"), 0), p1);
        figures.emplace_back(make_pair(Field("g2"), 0), p1);
        figures.emplace_back(make_pair(Field("h2"), 0), p1);
        Pawn* p2 = new Pawn(1);
        figures.emplace_back(make_pair(Field("a7"), 0), p2);
        figures.emplace_back(make_pair(Field("b7"), 0), p2);
        figures.emplace_back(make_pair(Field("c7"), 0), p2);
        figures.emplace_back(make_pair(Field("d7"), 0), p2);
        figures.emplace_back(make_pair(Field("e7"), 0), p2);
        figures.emplace_back(make_pair(Field("f7"), 0), p2);
        figures.emplace_back(make_pair(Field("g7"), 0), p2);
        figures.emplace_back(make_pair(Field("h7"), 0), p2);
        sort(figures.begin(), figures.end());
    }
    Position(const Position& other) {
        for (auto it: other.figures) {
            figures.emplace_back(it);
        }
        lastmove=other.lastmove;
    }
    Figure* FigureOnField(Field f) const {
        int n=figures.size();
        int left=0;
        int right=n;
        while (right>left+1) {
            int mid=(left+right)/2;
            if (f==figures[mid].first.first) {
                return figures[mid].second;
            } else if (f<figures[mid].first.first) {
                right=mid;
            } else left = mid+1;
        }
        if (left==right) return nullptr;
        if (figures[left].first.first==f) {
            return figures[left].second;
        } else return nullptr;
    }
    int IndexOfField(Field f) const {
        int n=figures.size();
        int left=0;
        int right=n;
        while (right>left+1) {
            int mid=(left+right)/2;
            if (f==figures[mid].first.first) {
                return mid;
            } else if (f<figures[mid].first.first) {
                right=mid;
            } else left = mid+1;
        }
        if (left==right) return -1;
        if (figures[left].first.first==f) {
            return left;
        } else return -1;
    }
    bool IsCheck(int c) const {
        Field kingfield(10, 10);
        for (auto it: figures) {
            if (it.second->GetShortName()=="K" && it.second->GetColor()==c) {
                kingfield=it.first.first;
                break;
            }
        }
        if (!kingfield.IsLegal()) return false;
        for (auto it: figures) {
            if (it.second->GetColor()!=c) {
                if (it.second->GetShortName()=="") {
                    for (auto move : it.second->GetMoves()) {
                        if (move.first*move.second!=0) {
                            Field fieldtobeat=it.first.first+move;
                            if (fieldtobeat==kingfield){
                                return true;
                            }
                        }
                    }
                } else {
                    for (auto move : it.second->GetMoves()) {
                        Field fieldtobeat=it.first.first+move;
                        if (fieldtobeat==kingfield){
                            return true;
                        }
                    }
                    for (auto direction : it.second->GetDirections()) {
                        Field curfield=it.first.first;
                        while (true) {
                            curfield = curfield + direction;
                            if (!curfield.IsLegal()) {
                                break;
                            }
                            if (FigureOnField(curfield)== nullptr) continue;
                            if (curfield==kingfield) return true;
                            break;
                        }
                    }
                }
            }
        }
        return false;
    }
    void Visualize() const {
        vector<vector<char>> v(8, vector<char> (8, '#'));
        for (auto it: figures) {
            pair<int, int> coor=it.first.first.GetCoordinates();
            char c='0';
            if (it.second->GetShortName()=="") {
                c='p';
            } else {
                string s=it.second->GetShortName();
                c=s[0];
            }
            v[coor.first-1][coor.second-1]=c;
        }
        for (int i=7; i>=0; i--) {
            for (int j=0; j<8; j++) cout<<v[j][i];
            cout<<endl;
        }
    }
    void MakeMove(Move m, int color) {
        if (m.GetTransformation()==5 && color==0) {
            MakeMove(Move("e1-g1"), color);
            MakeMove(Move("h1-f1"), color);
            return;
        }
        if (m.GetTransformation()==6 && color==0) {
            MakeMove(Move("e1-c1"), color);
            MakeMove(Move("a1-d1"), color);
            return;
        }
        if (m.GetTransformation()==5 && color==1) {
            MakeMove(Move("e8-g8"), color);
            MakeMove(Move("h8-f8"), color);
            return;
        }
        if (m.GetTransformation()==6 && color==1) {
            MakeMove(Move("e8-c8"), color);
            MakeMove(Move("a8-d8"), color);
            return;
        }
        Field f1=m.GetFields().first;
        Field f2=m.GetFields().second;
        int i1=IndexOfField(f1);
        int i2=IndexOfField(f2);
        figures[i1].first.first=f2;
        figures[i1].first.second=1;
        if (m.GetTransformation()==1) {
            Knight* k = new Knight(figures[i1].second->GetColor());
            figures[i1].second=k;
        }
        if (m.GetTransformation()==2) {
            Bishop* k = new Bishop(figures[i1].second->GetColor());
            figures[i1].second=k;
        }
        if (m.GetTransformation()==3) {
            Rook* k = new Rook(figures[i1].second->GetColor());
            figures[i1].second=k;
        }
        if (m.GetTransformation()==4) {
            Queen* k = new Queen(figures[i1].second->GetColor());
            figures[i1].second=k;
        }
        int waserase=0;
        if (i2!=-1) {
            figures.erase(figures.begin()+i2);
            waserase=1;
        }
        if (figures[i1].second->GetShortName()=="") {
            if (f1.GetCoordinates().first!=f2.GetCoordinates().first &&
            f1.GetCoordinates().second!=f2.GetCoordinates().second && i2==-1) {
                Field del(f2.GetCoordinates().first, f1.GetCoordinates().second);
                figures[i1].first.first=f1;
                int index = IndexOfField(del);
                figures[i1].first.first=f2;
                figures.erase(figures.begin()+index);
                waserase=1;
            }
        }
        if (f1 < f2) {
            for (int i=i1; i<figures.size()-1; i++) {
                if (figures[i+1].first.first<figures[i].first.first) {
                    swap(figures[i], figures[i+1]);
                } else break;
            }
        } else {
            if (waserase) i1--;
            for (int i=i1; i>0; i--) {
                if (figures[i].first.first<figures[i-1].first.first) {
                    swap(figures[i], figures[i-1]);
                } else break;
            }
        }
        lastmove=m;
    }
    bool IsPossible(const Move& move, int color) const {
        if (color==0 && move.GetString()=="0-0-0") {
            if (FigureOnField(Field("a1"))== nullptr || FigureOnField(Field("e1"))== nullptr) return false;
            if (FigureOnField(Field("a1"))->GetShortName()!="R" || FigureOnField(Field("e1"))->GetShortName()!="K") return false;
            if (figures[IndexOfField(Field("a1"))].first.second==1 ||
            figures[IndexOfField(Field("e1"))].first.second==1) return false;
            if (FigureOnField(Field("b1"))!= nullptr || FigureOnField(Field("c1"))!= nullptr ||
            FigureOnField(Field("d1"))!= nullptr) return false;
            if (IsCheck(color)) return false;
            Field f1("b1");
            Field f2("c1");
            Field f3("d1");
            int mozno=1;
            for (auto it: figures) {
                if (it.second->GetColor()!=color) {
                    if (it.second->GetShortName()=="") {
                        if (it.first.first.GetCoordinates().second==2 &&
                        it.first.first.GetCoordinates().first<6 && it.first.first.GetCoordinates().first>1){
                            mozno=0;
                            break;
                        }
                    } else {
                        for (auto move : it.second->GetMoves()) {
                            Field fieldtobeat=it.first.first+move;
                            if (fieldtobeat==f1 || fieldtobeat==f2 || fieldtobeat==f3){
                                mozno=0;
                                break;
                            }
                        }
                        for (auto direction : it.second->GetDirections()) {
                            Field curfield=it.first.first;
                            while (true) {
                                curfield = curfield + direction;
                                if (!curfield.IsLegal()) {
                                    break;
                                }
                                if (FigureOnField(curfield)!= nullptr) break;
                                if (curfield==f1 || curfield==f2 || curfield==f3) {
                                    mozno=0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (mozno) {
                return true;
            } else return false;
        }
        if (color==0 && move.GetString()=="0-0") {
            if (FigureOnField(Field("h1"))== nullptr || FigureOnField(Field("e1"))== nullptr) return false;
            if (FigureOnField(Field("h1"))->GetShortName()!="R" || FigureOnField(Field("e1"))->GetShortName()!="K") return false;
            if (figures[IndexOfField(Field("h1"))].first.second==1 ||
                figures[IndexOfField(Field("e1"))].first.second==1) return false;
            if (FigureOnField(Field("f1"))!= nullptr || FigureOnField(Field("g1"))!= nullptr) return false;
            if (IsCheck(color)) return false;
            Field f1("f1");
            Field f2("g1");
            int mozno=1;
            for (auto it: figures) {
                if (it.second->GetColor()!=color) {
                    if (it.second->GetShortName()=="") {
                        if (it.first.first.GetCoordinates().second==2 && it.first.first.GetCoordinates().first>4){
                            mozno=0;
                            break;
                        }
                    } else {
                        for (auto move : it.second->GetMoves()) {
                            Field fieldtobeat=it.first.first+move;
                            if (fieldtobeat==f1 || fieldtobeat==f2){
                                mozno=0;
                                break;
                            }
                        }
                        for (auto direction : it.second->GetDirections()) {
                            Field curfield=it.first.first;
                            while (true) {
                                curfield = curfield + direction;
                                if (!curfield.IsLegal()) {
                                    break;
                                }
                                if (FigureOnField(curfield)!= nullptr) break;
                                if (curfield==f1 || curfield==f2) {
                                    mozno=0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (mozno) {
                return true;
            } else return false;
        }
        if (color==0 && move.GetString()=="0-0-0") {
            if (FigureOnField(Field("a8"))== nullptr || FigureOnField(Field("e8"))== nullptr) return false;
            if (FigureOnField(Field("a8"))->GetShortName()!="R" || FigureOnField(Field("e8"))->GetShortName()!="K") return false;
            if (figures[IndexOfField(Field("a8"))].first.second==1 ||
                figures[IndexOfField(Field("e8"))].first.second==1) return false;
            if (FigureOnField(Field("b8"))!= nullptr || FigureOnField(Field("c8"))!= nullptr ||
                FigureOnField(Field("d8"))!= nullptr) return false;
            if (IsCheck(color)) return false;
            Field f1("b8");
            Field f2("c8");
            Field f3("d8");
            int mozno=1;
            for (auto it: figures) {
                if (it.second->GetColor()!=color) {
                    if (it.second->GetShortName()=="") {
                        if (it.first.first.GetCoordinates().second==7 &&
                        it.first.first.GetCoordinates().first<6 && it.first.first.GetCoordinates().first>1){
                            mozno=0;
                            break;
                        }
                    } else {
                        for (auto move : it.second->GetMoves()) {
                            Field fieldtobeat=it.first.first+move;
                            if (fieldtobeat==f1 || fieldtobeat==f2 || fieldtobeat==f3){
                                mozno=0;
                                break;
                            }
                        }
                        for (auto direction : it.second->GetDirections()) {
                            Field curfield=it.first.first;
                            while (true) {
                                curfield = curfield + direction;
                                if (!curfield.IsLegal()) {
                                    break;
                                }
                                if (FigureOnField(curfield)!= nullptr) break;
                                if (curfield==f1 || curfield==f2 || curfield==f3) {
                                    mozno=0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (mozno) {
                return true;
            } else return false;
        }
        if (color==0 && move.GetString()=="0-0") {
            if (FigureOnField(Field("h8"))== nullptr || FigureOnField(Field("e8"))== nullptr) return false;
            if (FigureOnField(Field("h8"))->GetShortName()!="R" || FigureOnField(Field("e8"))->GetShortName()!="K") return false;
            if (figures[IndexOfField(Field("h8"))].first.second==1 ||
                figures[IndexOfField(Field("e8"))].first.second==1) return false;
            if (FigureOnField(Field("f8"))!= nullptr || FigureOnField(Field("g8"))!= nullptr) return false;
            if (IsCheck(color)) return false;
            Field f1("f8");
            Field f2("g8");
            int mozno=1;
            for (auto it: figures) {
                if (it.second->GetColor()!=color) {
                    if (it.second->GetShortName()=="") {
                        if (it.first.first.GetCoordinates().second==7 && it.first.first.GetCoordinates().first>4){
                            mozno=0;
                            break;
                        }
                    } else {
                        for (auto move : it.second->GetMoves()) {
                            Field fieldtobeat=it.first.first+move;
                            if (fieldtobeat==f1 || fieldtobeat==f2){
                                mozno=0;
                                break;
                            }
                        }
                        for (auto direction : it.second->GetDirections()) {
                            Field curfield=it.first.first;
                            while (true) {
                                curfield = curfield + direction;
                                if (!curfield.IsLegal()) {
                                    break;
                                }
                                if (FigureOnField(curfield)!= nullptr) break;
                                if (curfield==f1 || curfield==f2) {
                                    mozno=0;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            if (mozno) {
                return true;
            } else return false;
        }
        Field start = move.GetFields().first;
        Field finish = move.GetFields().second;
        int index = IndexOfField(start);
        if (index==-1) return false;
        Figure* fig =figures[index].second;
        if (fig->GetColor()!=color) return false;
        pair<int, int> vect=make_pair(finish.GetCoordinates().first-start.GetCoordinates().first,
                                      finish.GetCoordinates().second-start.GetCoordinates().second);

        if (fig->GetShortName()=="") {
            for (auto m : fig->GetMoves()) {
                if (m.first*m.second!=0 && m==vect) {
                    Figure* beat=FigureOnField(finish);
                    if (beat!=nullptr){
                        if (beat->GetColor()!=color) {
                            Position p(*this);
                            p.MakeMove(move, color);
                            if (p.IsCheck(color)) {
                                return false;
                            } else return true;
                        } else return false;
                    } else {
                        if (color+start.GetCoordinates().second==5) {
                            Field f1(finish.GetCoordinates().first,
                                     start.GetCoordinates().second+2*m.second);
                            Field f2(finish.GetCoordinates().first,
                                     start.GetCoordinates().second);
                            Move last(f1, f2);
                            if (last.GetString()==lastmove.GetString()) {
                                if (FigureOnField(f2)->GetShortName()=="") {
                                    Position p(*this);
                                    p.MakeMove(move, color);
                                    if (p.IsCheck(color)) {
                                        return false;
                                    } else return true;
                                }
                            }
                        }
                        return false;
                    }
                } else if ((m.second==1 || m.second==-1) && m==vect){
                    if (FigureOnField(finish)== nullptr) {
                        Position p(*this);
                        p.MakeMove(move, color);
                        if (p.IsCheck(color)) {
                            return false;
                        } else return true;
                    } else return false;
                } else if (m==vect){
                    if (figures[index].first.second==0) {
                        if (FigureOnField(finish)== nullptr &&
                        FigureOnField(Field(start.GetCoordinates().first,start.GetCoordinates().second+vect.second/2))== nullptr) {
                            Position p(*this);
                            p.MakeMove(move, color);
                            if (p.IsCheck(color)) {
                                return false;
                            } else return true;
                        }
                    }
                    return false;
                }
            }
            return false;
        } else {
            for (auto m : fig->GetMoves()) {
                if (m==vect) {
                    if (FigureOnField(finish) == nullptr) {
                        Position p(*this);
                        p.MakeMove(move, color);
                        if (p.IsCheck(color)) {
                            return false;
                        } else return true;
                    } else if (FigureOnField(finish)->GetColor() != color) {
                        Position p(*this);
                        p.MakeMove(move, color);
                        if (p.IsCheck(color)) {
                            return false;
                        } else return true;
                    } else return false;
                }
            }
            for (auto direction : fig->GetDirections()) {
                Field curfield=start;
                while (true) {
                    curfield = curfield + direction;
                    if (!curfield.IsLegal()) break;
                    if (FigureOnField(curfield)== nullptr) {
                        if (curfield==finish) {
                            Position p(*this);
                            p.MakeMove(move, color);
                            if (p.IsCheck(color)) {
                                return false;
                            } else return true;
                        }
                    } else {
                        if (FigureOnField(curfield)->GetColor()!=color) {
                            if (curfield==finish) {
                                Position p(*this);
                                p.MakeMove(move, color);
                                if (p.IsCheck(color)) {
                                    return false;
                                } else return true;
                            }
                        }
                        break;
                    }
                }
            }
            return false;
        }
    }
    /*bool IsActive(Move move, int color) const {
        Position p(*this);
        int threat=0;
        for (auto it: figures) {
            if (it.second->GetColor()!=color) continue;
            for (auto it2: figures) {
                if (it2.second->GetColor()!=color) {
                    Move m(it.first.first, it2.first.first);
                    if (IsPossible(m, color)) {
                        if (IsSafeToBeat(m, color)) threat++;
                    }
                }
            }
        }
        p.MakeMove(move, color);
        if (p.IsCheck(1-color)) return true;
        if (move.GetTransformation()>4) return false;
        if (move.GetTransformation()>0) return true;
        if (FigureOnField(move.GetFields().second)!= nullptr) return true;
        if (FigureOnField(move.GetFields().first)->GetShortName()=="") {
            if (move.GetFields().first.GetCoordinates().first != move.GetFields().second.GetCoordinates().first &&
                    move.GetFields().first.GetCoordinates().second != move.GetFields().second.GetCoordinates().second) {
                return true;
            }
        }
        int newthreat=0;
        for (auto it: p.figures) {
            if (it.second->GetColor()!=color) continue;
            for (auto it2: p.figures) {
                if (it2.second->GetColor()!=color) {
                    Move m(it.first.first, it2.first.first);
                    if (p.IsPossible(m, color)) {
                        if (p.IsSafeToBeat(m, color)) newthreat++;
                    }
                }
            }
        }
        if (newthreat>threat) return true;
        return false;
    }*/
    vector<Move> GetCapturesTransFromVector (const vector<Move>& moves) const {
        vector<pair<int, Move>> answer;
        for (auto move: moves) {
            if (move.GetTransformation()==5 || move.GetTransformation()==6) continue;
            if (move.GetTransformation()>0) {
                int x=-1;
                if (move.GetTransformation()==1) x+=3;
                if (move.GetTransformation()==2) x+=3;
                if (move.GetTransformation()==3) x+=5;
                if (move.GetTransformation()==4) x+=10;
                if (FigureOnField(move.GetFields().second)!= nullptr) x+=FigureOnField(move.GetFields().second)->GetCost();
                answer.emplace_back(make_pair(x,move));
            } else if (FigureOnField(move.GetFields().first)->GetShortName()=="") {
                if (move.GetFields().first.GetCoordinates().first!=move.GetFields().second.GetCoordinates().first &&
                        move.GetFields().first.GetCoordinates().second!=move.GetFields().second.GetCoordinates().second) {
                    if (FigureOnField(move.GetFields().second)==nullptr) {
                        answer.emplace_back(make_pair(1,move));
                    } else answer.emplace_back(make_pair(FigureOnField(move.GetFields().second)->GetCost(),move));
                } else continue;
            } else if (FigureOnField(move.GetFields().second)!=nullptr) {
                answer.emplace_back(make_pair(FigureOnField(move.GetFields().second)->GetCost(),move));
            }
        }
        sort(answer.begin(), answer.end());
        int n=answer.size();
        vector<Move> result;
        for (int i=n-1; i>=0; i--) result.emplace_back(answer[i].second);
        return result;
    }
    vector<Move> GetCapturesTrans (int color) const {
        vector<Move> v=PossibleMoves(color);
        vector<Move> answer=GetCapturesTransFromVector(v);
        return answer;
    }
    int SafeToBeat(Move move, int colortomove, int primarycolor, int potential) const {
        Field f=move.GetFields().second;
        int change=0;
        if (move.GetTransformation()>0) {
            change=-1;
            if (move.GetTransformation()==1) change+=3;
            if (move.GetTransformation()==2) change+=3;
            if (move.GetTransformation()==3) change+=5;
            if (move.GetTransformation()==4) change+=10;
        } else if (FigureOnField(f) == nullptr) {
            change=1;
        }
        if (FigureOnField(f) != nullptr) change+=FigureOnField(f)->GetCost();
        int newpotential=potential;
        if (colortomove==primarycolor) {
            newpotential+=change;
        } else newpotential-=change;
        Position p(*this);
        p.MakeMove(move, colortomove);
        int  bestmark=newpotential;
        int mate=p.MateMove(2, 1-colortomove);
        if (mate!=-1) {
            if (colortomove==primarycolor) {
                return -100;
            } else return 100;
        }
        vector<Move> v=p.GetCapturesTrans(1-colortomove);
        for (const Move& m: v) {
            Field f2=m.GetFields().second;
            int change2=0;
            if (p.FigureOnField(f2) != nullptr) {
                change2 = p.FigureOnField(f2)->GetCost();
            } else if (m.GetTransformation()==0) {
                change2=1;
            }
            if (m.GetTransformation()==1) change2+=2;
            if (m.GetTransformation()==2) change2+=2;
            if (m.GetTransformation()==3) change2+=4;
            if (m.GetTransformation()==4) change2+=9;
            if (colortomove==primarycolor) {
                if (newpotential-change2>0) continue;
            } else {
                if (newpotential+change2<0) continue;
            }
            int x=p.SafeToBeat(m, 1-colortomove, primarycolor, newpotential);
            if (colortomove==primarycolor) {
                bestmark=min(bestmark, x);
                if (bestmark<0) break;
            } else {
                bestmark=max(bestmark, x);
                if (bestmark>0) break;
            }
        }
        if (bestmark==100 || bestmark==-100) return newpotential;
        return bestmark;
    }
    int GetNumberOfDangerousCaptures(int color) const {
        vector<Move> v=GetCapturesTrans(color);
        int answer=0;
        for (Move m: v) {
            if (SafeToBeat(m, color, color, 0)>0) answer++;
        }
        return answer;
    }
    int GetMaxDangerousCapture(int color) const {
        vector<Move> v=GetCapturesTrans(color);
        int answer=0;
        for (Move m: v) {
            int s=SafeToBeat(m, color, color, 0);
            if (s>answer) answer=s;
        }
        return answer;
    }
    vector<Move> PossibleMoves(int color) const {
        vector<Move> result;
        for (auto it: figures) {
            if (it.second->GetColor()==color) {
                if (it.second->GetShortName()=="") {
                    for (auto move : it.second->GetMoves()) {
                        if (move.first*move.second!=0) {
                            Field fieldtobeat=it.first.first+move;
                            Figure* beat=FigureOnField(fieldtobeat);
                            if (beat!=nullptr){
                                if (beat->GetColor()!=color) {
                                    if (fieldtobeat.GetCoordinates().second==8 || fieldtobeat.GetCoordinates().second==1) {
                                        result.emplace_back(Move(it.first.first, fieldtobeat, 1));
                                        result.emplace_back(Move(it.first.first, fieldtobeat, 2));
                                        result.emplace_back(Move(it.first.first, fieldtobeat, 3));
                                        result.emplace_back(Move(it.first.first, fieldtobeat, 4));
                                    } else result.emplace_back(Move(it.first.first, fieldtobeat, 0));
                                }
                            } else {
                                if (color+it.first.first.GetCoordinates().second==5) {
                                    Field f1(fieldtobeat.GetCoordinates().first,
                                             it.first.first.GetCoordinates().second+2*move.second);
                                    Field f2(fieldtobeat.GetCoordinates().first,
                                             it.first.first.GetCoordinates().second);
                                    Move last(f1, f2);
                                    if (last.GetString()==lastmove.GetString()) {
                                        if (FigureOnField(f2)->GetShortName()=="") {
                                            result.emplace_back(Move(it.first.first, fieldtobeat, 0));
                                        }
                                    }
                                }
                            }
                        } else if (move.second==1 || move.second==-1){
                            Field fieldtogo=it.first.first+move;
                            if (FigureOnField(fieldtogo)== nullptr) {
                                if (fieldtogo.GetCoordinates().second==8 || fieldtogo.GetCoordinates().second==1) {
                                    result.emplace_back(Move(it.first.first, fieldtogo, 1));
                                    result.emplace_back(Move(it.first.first, fieldtogo, 2));
                                    result.emplace_back(Move(it.first.first, fieldtogo, 3));
                                    result.emplace_back(Move(it.first.first, fieldtogo, 4));
                                } else result.emplace_back(Move(it.first.first, fieldtogo, 0));
                            } else break;
                        } else {
                            if (it.first.second==0) {
                                Field fieldtogo=it.first.first+move;
                                if (FigureOnField(fieldtogo)== nullptr) {
                                    result.emplace_back(Move(it.first.first, fieldtogo, 0));
                                }
                            }
                        }
                    }
                } else {
                    for (auto move : it.second->GetMoves()) {
                        Field fieldtobeat=it.first.first+move;
                        if (!fieldtobeat.IsLegal()) continue;
                        if (FigureOnField(fieldtobeat)== nullptr) {
                            result.emplace_back(Move(it.first.first, fieldtobeat, 0));
                        } else if (FigureOnField(fieldtobeat)->GetColor()!=color) {
                            result.emplace_back(Move(it.first.first, fieldtobeat, 0));
                        }
                    }
                    for (auto direction : it.second->GetDirections()) {
                        Field curfield=it.first.first;
                        while (true) {
                            curfield = curfield + direction;
                            if (!curfield.IsLegal()) break;
                            if (FigureOnField(curfield)== nullptr) {
                                result.emplace_back(Move(it.first.first, curfield, 0));
                            } else {
                                if (FigureOnField(curfield)->GetColor()!=color) {
                                    result.emplace_back(Move(it.first.first, curfield, 0));
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (color==0) {
            if (FigureOnField(Field("a1"))!= nullptr && FigureOnField(Field("e1"))!= nullptr) {
                if (FigureOnField(Field("a1"))->GetShortName()=="R" && FigureOnField(Field("e1"))->GetShortName()=="K") {
                    if (figures[IndexOfField(Field("a1"))].first.second==0 && figures[IndexOfField(Field("e1"))].first.second==0) {
                        if (FigureOnField(Field("b1"))== nullptr && FigureOnField(Field("c1"))== nullptr &&
                        FigureOnField(Field("d1"))== nullptr) {
                            if (!IsCheck(color)) {
                                //------------------------------------------------------------------------------
                                Field f1("b1");
                                Field f2("c1");
                                Field f3("d1");
                                int mozno=1;
                                for (auto it: figures) {
                                    if (it.second->GetColor()!=color) {
                                        if (it.second->GetShortName()=="") {
                                            for (auto move : it.second->GetMoves()) {
                                                if (move.first*move.second!=0) {
                                                    Field fieldtobeat=it.first.first+move;
                                                    if (fieldtobeat==f1 || fieldtobeat==f2 || fieldtobeat==f3){
                                                        mozno=0;
                                                    }
                                                }
                                            }
                                        } else {
                                            for (auto move : it.second->GetMoves()) {
                                                Field fieldtobeat=it.first.first+move;
                                                if (fieldtobeat==f1 || fieldtobeat==f2 || fieldtobeat==f3){
                                                    mozno=0;
                                                }
                                            }
                                            for (auto direction : it.second->GetDirections()) {
                                                Field curfield=it.first.first;
                                                while (true) {
                                                    curfield = curfield + direction;
                                                    if (!curfield.IsLegal()) {
                                                        break;
                                                    }
                                                    if (FigureOnField(curfield)!= nullptr) break;
                                                    if (curfield==f1 || curfield==f2 || curfield==f3) {
                                                        mozno=0;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (mozno) {
                                    result.emplace_back(Move("0-0-0"));
                                }
                            }
                        }
                    }
                }
            }
            if (FigureOnField(Field("h1"))!= nullptr && FigureOnField(Field("e1"))!= nullptr) {
                if (FigureOnField(Field("h1"))->GetShortName()=="R" && FigureOnField(Field("e1"))->GetShortName()=="K") {
                    if (figures[IndexOfField(Field("h1"))].first.second==0 && figures[IndexOfField(Field("e1"))].first.second==0) {
                        if (FigureOnField(Field("f1"))== nullptr && FigureOnField(Field("g1"))== nullptr) {
                            if (!IsCheck(color)) {
                                Field f1("f1");
                                Field f2("g1");
                                int mozno=1;
                                for (auto it: figures) {
                                    if (it.second->GetColor()!=color) {
                                        if (it.second->GetShortName()=="") {
                                            for (auto move : it.second->GetMoves()) {
                                                if (move.first*move.second!=0) {
                                                    Field fieldtobeat=it.first.first+move;
                                                    if (fieldtobeat==f1 || fieldtobeat==f2){
                                                        mozno=0;
                                                    }
                                                }
                                            }
                                        } else {
                                            for (auto move : it.second->GetMoves()) {
                                                Field fieldtobeat=it.first.first+move;
                                                if (fieldtobeat==f1 || fieldtobeat==f2){
                                                    mozno=0;
                                                }
                                            }
                                            for (auto direction : it.second->GetDirections()) {
                                                Field curfield=it.first.first;
                                                while (true) {
                                                    curfield = curfield + direction;
                                                    if (!curfield.IsLegal()) {
                                                        break;
                                                    }
                                                    if (FigureOnField(curfield)!= nullptr) break;
                                                    if (curfield==f1 || curfield==f2) {
                                                        mozno=0;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (mozno) {
                                    result.emplace_back(Move("0-0"));
                                }
                            }
                        }
                    }
                }
            }
        }
        if (color==1) {
            if (FigureOnField(Field("a8"))!= nullptr && FigureOnField(Field("e8"))!= nullptr) {
                if (FigureOnField(Field("a8"))->GetShortName()=="R" && FigureOnField(Field("e8"))->GetShortName()=="K") {
                    if (figures[IndexOfField(Field("a8"))].first.second==0 && figures[IndexOfField(Field("e8"))].first.second==0) {
                        if (FigureOnField(Field("b8"))== nullptr && FigureOnField(Field("c8"))== nullptr &&
                            FigureOnField(Field("d8"))== nullptr) {
                            if (!IsCheck(color)) {
                                Field f1("b8");
                                Field f2("c8");
                                Field f3("d8");
                                int mozno=1;
                                for (auto it: figures) {
                                    if (it.second->GetColor()!=color) {
                                        if (it.second->GetShortName()=="") {
                                            for (auto move : it.second->GetMoves()) {
                                                if (move.first*move.second!=0) {
                                                    Field fieldtobeat=it.first.first+move;
                                                    if (fieldtobeat==f1 || fieldtobeat==f2 || fieldtobeat==f3){
                                                        mozno=0;
                                                    }
                                                }
                                            }
                                        } else {
                                            for (auto move : it.second->GetMoves()) {
                                                Field fieldtobeat=it.first.first+move;
                                                if (fieldtobeat==f1 || fieldtobeat==f2 || fieldtobeat==f3){
                                                    mozno=0;
                                                }
                                            }
                                            for (auto direction : it.second->GetDirections()) {
                                                Field curfield=it.first.first;
                                                while (true) {
                                                    curfield = curfield + direction;
                                                    if (!curfield.IsLegal()) {
                                                        break;
                                                    }
                                                    if (FigureOnField(curfield)!= nullptr) break;
                                                    if (curfield==f1 || curfield==f2 || curfield==f3) {
                                                        mozno=0;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (mozno) {
                                    result.emplace_back(Move("0-0-0"));
                                }
                            }
                        }
                    }
                }
            }
            if (FigureOnField(Field("h8"))!= nullptr && FigureOnField(Field("e8"))!= nullptr) {
                if (FigureOnField(Field("h8"))->GetShortName()=="R" && FigureOnField(Field("e8"))->GetShortName()=="K") {
                    if (figures[IndexOfField(Field("h8"))].first.second==0 && figures[IndexOfField(Field("e8"))].first.second==0) {
                        if (FigureOnField(Field("f8"))== nullptr && FigureOnField(Field("g8"))== nullptr) {
                            if (!IsCheck(color)) {
                                Field f1("f8");
                                Field f2("g8");
                                int mozno=1;
                                for (auto it: figures) {
                                    if (it.second->GetColor()!=color) {
                                        if (it.second->GetShortName()=="") {
                                            for (auto move : it.second->GetMoves()) {
                                                if (move.first*move.second!=0) {
                                                    Field fieldtobeat=it.first.first+move;
                                                    if (fieldtobeat==f1 || fieldtobeat==f2){
                                                        mozno=0;
                                                    }
                                                }
                                            }
                                        } else {
                                            for (auto move : it.second->GetMoves()) {
                                                Field fieldtobeat=it.first.first+move;
                                                if (fieldtobeat==f1 || fieldtobeat==f2){
                                                    mozno=0;
                                                }
                                            }
                                            for (auto direction : it.second->GetDirections()) {
                                                Field curfield=it.first.first;
                                                while (true) {
                                                    curfield = curfield + direction;
                                                    if (!curfield.IsLegal()) {
                                                        break;
                                                    }
                                                    if (FigureOnField(curfield)!= nullptr) break;
                                                    if (curfield==f1 || curfield==f2) {
                                                        mozno=0;
                                                        break;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (mozno) {
                                    result.emplace_back(Move("0-0"));
                                }
                            }
                        }
                    }
                }
            }
        }
        vector<Move> answer;
        for (Move it : result) {
            Position p(*this);
            p.MakeMove(it, color);
            if (!p.IsCheck(color)) answer.emplace_back(it);
        }
       /* for (Move it : answer) {
            cout<<it.GetString()<<endl;
        }*/
        return answer;
    }
    vector<pair<pair<Field, int>, Figure*>> GetFigures() const {
        return figures;
    }
    int GetActivity (int color) const {
        return PossibleMoves(color).size()-PossibleMoves(1-color).size();
    }
    int Mark(int color) const {
        int yourpoints = 0;
        int opponentpoints = 0;
        for (auto it: figures) {
            if (it.second->GetColor() == color) {
                yourpoints += it.second->GetCost();
            } else opponentpoints += it.second->GetCost();
        }
        return yourpoints - opponentpoints;
    }
    /*vector<int> BestQualityMoves (const vector<Move>& v, int deep, int color) const {
        int maximum=-100;
        vector<int> bestmoves;
        for (int i=0; i<v.size(); i++) {
            Move move=v[i];
            Position p(*this);
            p.MakeMove(move, color);
            int mark=p.BestQuality(p.PossibleMoves(1-color), deep-1, 1-color);
            if (-mark > maximum) {
                bestmoves.clear();
                bestmoves.push_back(i);
                maximum=-mark;
            } else if (-mark==maximum) {
                bestmoves.push_back(i);
            }
        }
        cout<<maximum<<endl;
        return bestmoves;
    }
    int BestQuality (const vector<Move>& v, int deep, int color) const {
        if (deep==0) {
            return Mark(color);
        } else {
            int maximum=-100;
            for (int i=0; i<v.size(); i++) {
                Move move=v[i];
                if (!IsActive(move, color)) {
                    Position p(*this);
                    p.MakeMove(move, color);
                    int mark=p.Mark(1-color);
                    maximum=max(maximum, -mark);
                } else {
                    Position p(*this);
                    int wasfig=figures.size();
                    p.MakeMove(move, color);
                    int newfig=p.figures.size();
                    int mark;
                    if (deep==1 && newfig<wasfig && (!IsSafeToBeat(move, color))) {
                        mark=p.BestQuality(p.PossibleMoves(1 - color), deep - 1, 1 - color)+
                                FigureOnField(move.GetFields().first)->GetCost();
                    } else mark = p.BestQuality(p.PossibleMoves(1 - color), deep - 1, 1 - color);
                    if (-mark > maximum) {
                        maximum = -mark;
                    }
                }
            }
            return maximum;
        }
    }*/
    int MateMove(int n, int color) const {
        vector<Move> v=PossibleMoves(color);
        if (n==1) {
            for (int i=0; i<v.size(); i++) {
                Position p(*this);
                p.MakeMove(v[i], color);
                vector<Move> v2=p.PossibleMoves(1-color);
                if (v2.empty() && p.IsCheck(1-color)) return i;
            }
            return -1;
        } else {
            for (int i=0; i<v.size(); i++) {
                Position p(*this);
                p.MakeMove(v[i], color);
                vector<Move> v2=p.PossibleMoves(1-color);
                if (v2.empty() && p.IsCheck(1-color)) return i;
                if (v2.size()>7) continue;
                int mat=1;
                for (Move move : v2) {
                    Position p2(p);
                    p2.MakeMove(move, 1-color);
                    if (p2.MateMove(n-1, color)==-1) {
                        mat=0;
                        break;
                    }
                }
                if (mat==1) return i;
            }
            return -1;
        }
    }
    void OneThreadFunction (const vector<Move>& v, int i, int threadnumber, vector<Move>& answers, int& winpoints, int color) {
        int index=i;
        while (index<v.size()) {
            int was=GetNumberOfDangerousCaptures(color);
            Move it=v[index];
            Position p(*this);
            p.MakeMove(it, color);
            int a=0;
            if (p.IsCheck(1-color)) a=1;
            int stalo=p.GetNumberOfDangerousCaptures(color);
            if (stalo>was) a=1;
            if (a==0) {
                index+=threadnumber;
                continue;
            }
            vector<Move> pos=p.PossibleMoves(1-color);
            int profit=100;
            for (auto it2: pos) {
                Position p2(p);
                p2.MakeMove(it2, 1-color);
                int win=p2.GetMaxDangerousCapture(color);
                if (win<profit) profit=win;
            }
            mut.lock();
            if (profit>winpoints) {
                winpoints=profit;
                answers.clear();
                answers.emplace_back(it);
            } else if (profit==winpoints) {
                answers.emplace_back(it);
            }
            mut.unlock();
            index+=threadnumber;
        }
    }
    Move ChoseMove(int color) {
        vector<Move> moves = PossibleMoves(color);
        vector<Move> safemoves;
        //int active=0;
        for (Move x: moves) {
            Position p(*this);
            p.MakeMove(x, color);
            int index = p.MateMove(3, 1-color);
            if (index==-1) {
                safemoves.emplace_back(x);
                /*if (IsActive(x, color)) {
                    active++;
                    cout<<x.GetString()<<endl;
                }*/
            }
        }
        //cout<<"safe: "<<safemoves.size()<<" active: "<<active<<endl;
        if (safemoves.size()==0) {
            int n = moves.size();
            srand(time(0));
            int i = rand() % n;
            return moves[i];
        } else {
            /*vector<int> p = BestQualityMoves(safemoves, 4, color);
            int n=p.size();
            srand(time(0));
            int i = rand() % n;
            for (int j=0; j<n; j++) {
                if (safemoves[p[j]].GetTransformation()==5 || safemoves[p[j]].GetTransformation()==6) i=j;
            }
            return safemoves[p[i]];*/
            vector<Move> v=GetCapturesTransFromVector(safemoves);
            vector<Move> noncaptures;
            int a=safemoves.size();
            int b=v.size();
            int bi=0;
            if (b==0) {
                noncaptures=safemoves;
            } else {
                for (int j = 0; j < a; j++) {
                    if (bi>v.size()) {
                        noncaptures.emplace_back(safemoves[j]);
                    } else if (safemoves[j].GetString()==v[bi].GetString()) {
                        bi++;
                    } else {
                        noncaptures.emplace_back(safemoves[j]);
                    }
                }
            }
            int maxi=-100;
            vector<Move> best;
            for (auto it: v) {
                int safe=SafeToBeat(it, color, color, 0);
                if (safe>maxi) {
                    maxi=safe;
                    best.clear();
                    best.emplace_back(it);
                } else if (safe==maxi) {
                    best.emplace_back(it);
                }
            }
            if (maxi>0) {
                int n = best.size();
                srand(time(0));
                int i = rand() % n;
                return best[i];
            }
            vector<Move> best2;
            int castle=0;
            Move rok("a1-b1");
            for (auto it: noncaptures) {
                Position p(*this);
                p.MakeMove(it, color);
                int q=p.GetNumberOfDangerousCaptures(1-color);
                if (q==0) {
                    best2.emplace_back(it);
                    if (it.GetTransformation()>4) {
                        castle=1;
                        rok=it;
                    }
                }
            }
            if ((best2.size()==0 && best.size()!=0 && maxi==0) || (noncaptures.empty())) {
                int n = best.size();
                srand(time(0));
                int i = rand() % n;
                return best[i];
            }
            if (best2.size()==0 && (best.size()==0 || maxi<0)) {
                int mini=100;
                vector<Move> result;
                for (auto it: noncaptures) {
                    Position p(*this);
                    p.MakeMove(it, color);
                    int q=p.GetMaxDangerousCapture(1-color);
                    if (q<mini) {
                        result.clear();
                        mini=q;
                        result.emplace_back(it);
                    } else if (q==mini) {
                        result.emplace_back(it);
                    }
                }
                if (best.empty()) {
                    int n = result.size();
                    srand(time(0));
                    int i = rand() % n;
                    return result[i];
                } else {
                    if (maxi>-mini) {
                        int n = best.size();
                        srand(time(0));
                        int i = rand() % n;
                        return best[i];
                    } else if (-mini>maxi) {
                        int n = result.size();
                        srand(time(0));
                        int i = rand() % n;
                        return result[i];
                    } else {
                        int n = best.size();
                        srand(time(0));
                        int i = rand() % n;
                        int nn = result.size();
                        int ii = rand() % nn;
                        int pp=rand()%2;
                        if (pp==0) return best[i];
                        return result[ii];
                    }
                }
            }
            vector<Move> best3;
            int winpoints=-100;
            int n=best2.size();
            int threadsnumber=8;
            vector<thread> engines;
            for (int i=0; i<threadsnumber; i++) {
                engines.emplace_back(&Position::OneThreadFunction, this, best2, i, threadsnumber, ref(best3),
                                     ref(winpoints), color);
            }
            for (int i=0; i<threadsnumber; i++) engines[i].join();
            if (maxi<0) {
                if (winpoints>0) {
                    int n = best3.size();
                    srand(time(0));
                    int i = rand() % n;
                    return best3[i];
                } else {
                    if (castle) return rok;
                    vector<Move> active;
                    int maxactivity=-100;
                    if (best3.size()==0) best3=best2;
                    for (auto mov: best3) {
                        Position p(*this);
                        p.MakeMove(mov, color);
                        int activity=p.GetActivity(color);
                        if (activity>maxactivity) {
                            maxactivity=activity;
                            active.clear();
                            active.emplace_back(mov);
                        } else if (activity==maxactivity) active.emplace_back(mov);
                    }
                    int n = active.size();
                    srand(time(0));
                    int i = rand() % n;
                    return active[i];
                }
            } else {
                if (winpoints>0) {
                    int n = best3.size();
                    srand(time(0));
                    int i = rand() % n;
                    return best3[i];
                } else {
                    if (castle) return rok;
                    int n = best.size();
                    srand(time(0));
                    int i = rand() % n;
                    vector<Move> active;
                    int maxactivity=-100;
                    if (best3.size()==0) best3=best2;
                    for (auto mov: best3) {
                        Position p(*this);
                        p.MakeMove(mov, color);
                        int activity=p.GetActivity(color);
                        if (activity>maxactivity) {
                            maxactivity=activity;
                            active.clear();
                            active.emplace_back(mov);
                        } else if (activity==maxactivity) active.emplace_back(mov);
                    }
                    int nn = active.size();
                    int ii = rand() % n;
                    int pp=rand()%2;
                    if (pp==0) return best[i];
                    return active[ii];
                }
            }
        }
    }
    void LoadPosition(const vector<string>& v) {
        int color=0;
        figures.clear();
        for (string s: v) {
            if (s=="White") {
                color=0;
                continue;
            }
            if (s=="Black") {
                color = 1;
                continue;
            }
            if (s.size()==2) {
                Pawn* p = new Pawn(color);
                figures.emplace_back(make_pair(Field(s), 0), p);
                continue;
            }
            if (s[0]=='R') {
                Rook* p = new Rook(color);
                figures.emplace_back(make_pair(Field(s.substr(1, 2)), 0), p);
                continue;
            }
            if (s[0]=='N') {
                Knight* p = new Knight(color);
                figures.emplace_back(make_pair(Field(s.substr(1, 2)), 0), p);
                continue;
            }
            if (s[0]=='B') {
                Bishop* p = new Bishop(color);
                figures.emplace_back(make_pair(Field(s.substr(1, 2)), 0), p);
                continue;
            }
            if (s[0]=='K') {
                King* p = new King(color);
                figures.emplace_back(make_pair(Field(s.substr(1, 2)), 0), p);
                continue;
            }
            if (s[0]=='Q') {
                Queen* p = new Queen(color);
                figures.emplace_back(make_pair(Field(s.substr(1, 2)), 0), p);
                continue;
            }
        }
        sort(figures.begin(), figures.end());
    }
    void List() const {
        for (auto it: figures) {
            cout<<it.second->GetShortName()<<it.first.first.GetString()<<endl;
        }
    }
private:
    vector<pair<pair<Field, int>, Figure*>> figures;
    Move lastmove=Move("c6-b8");
};

int main() {
    Position p;
    int playercolor, computercolor;
    cout<<"Chose the colour you will play: White or Black"<<endl;
    while (true) {
        string s;
        cin >> s;
        if (s == "White") {
            playercolor = 0;
            computercolor = 1;
            break;
        } else if (s == "Black") {
            playercolor = 1;
            computercolor = 0;
            break;
        } else {
            cout<<"Incorrect color. Write one word: White or Black"<<endl;
        }
    }
    int colortomove=0;
    cout<<"Game begins"<<endl;
    if (playercolor==0) cout<<"It is your move"<<endl;
    while (true) {
        if (colortomove==playercolor) {
            vector<Move> v = p.PossibleMoves(playercolor);
            if (v.size()==0) {
                if (p.IsCheck(playercolor)) {
                    cout<<"Checkmate! You lost!"<<endl;
                    break;
                } else {
                    cout<<"Stalemate! It is draw"<<endl;
                    break;
                }
            } else {
                while (true) {
                    string move;
                    cin >> move;
                    if (move != "0-0" && move != "0-0-0") {
                        if (move.size() == 5) {
                            if (move[2] != '-') {
                                cout << "Incorrect writing" << endl;
                                continue;
                            } else {
                                Field f1 = move.substr(0, 2);
                                Field f2 = move.substr(3, 2);
                                if (!f1.IsLegal() || !f2.IsLegal()) {
                                    cout << "Incorrect writing" << endl;
                                    continue;
                                }
                            }
                        } else if (move.size() == 6) {
                            if (move[2] != '-') {
                                cout << "Incorrect writing" << endl;
                                continue;
                            } else if (move[5] != 'N' && move[5] != 'B' && move[5] != 'R' && move[5] != 'Q') {
                                cout << "Incorrect writing" << endl;
                                continue;
                            } else {
                                Field f1 = move.substr(0, 2);
                                Field f2 = move.substr(3, 2);
                                if (!f1.IsLegal() || !f2.IsLegal()) {
                                    cout << "Incorrect writing" << endl;
                                    continue;
                                }
                            }
                        } else {
                            cout << "Incorrect writing" << endl;
                            continue;
                        }
                    }
                    Move m(move);
                    int possible = 0;
                    for (Move it: v) {
                        if (it.GetString() == move) {
                            possible = 1;
                        }
                    }
                    if (!possible) {
                        cout << "Impossible move" << endl;
                        continue;
                    }
                    p.MakeMove(m, playercolor);
                    break;
                }
            }
        } else {
            vector<Move> v = p.PossibleMoves(computercolor);
            if (v.size()==0) {
                if (p.IsCheck(computercolor)) {
                    cout<<"Checkmate! You win!"<<endl;
                    break;
                } else {
                    cout<<"Stalemate! It is draw"<<endl;
                    break;
                }
            } else {
                int i = p.MateMove(3, computercolor);
                if (i!=-1) {
                    Move move = v[i];
                    p.MakeMove(move, computercolor);
                    cout<<move.GetString()<<endl;
                } else {
                    Move move = p.ChoseMove(computercolor);
                    p.MakeMove(move, computercolor);
                    cout << move.GetString() << endl;
                }
            }
        }
        colortomove=1-colortomove;
    }
    /*Position p;
   vector<string> v;
    v.push_back("White");
    v.push_back("a2");
    v.push_back("b2");
    v.push_back("c3");
    v.push_back("d3");
    v.push_back("e2");
    v.push_back("f2");
    v.push_back("g3");
    v.push_back("h4");
    v.push_back("Na3");
    v.push_back("Nh7");
    v.push_back("Bc1");
    v.push_back("Bf1");
    v.push_back("Ra1");
    v.push_back("Rh1");
    v.push_back("Ke1");
    v.push_back("Qc2");
    v.push_back("Black");
    v.push_back("a7");
    v.push_back("b6");
    v.push_back("c6");
    v.push_back("d5");
    v.push_back("e5");
    v.push_back("f7");
    v.push_back("g7");
    v.push_back("Nb8");
    v.push_back("Nf6");
    v.push_back("Bc8");
    v.push_back("Bf8");
    v.push_back("Ra8");
    v.push_back("Rh8");
    v.push_back("Kc5");
    v.push_back("Qd6");
    p.LoadPosition(v);
    p.Visualize();
    cout<<endl;
    vector<Move> m=p.PossibleMoves(0);*/
    /*if (p.IsCheck(0)) cout<<"Belym shah"<<endl;
    if (p.IsCheck(1)) cout<<"Chernym shah"<<endl;*/
    /*Position p;
    p.MakeMove(Move("d2-d3"), 0);
    p.MakeMove(Move("e7-e6"), 1);
    p.MakeMove(Move("e2-e4"), 0);
    p.MakeMove(Move("f8-b4"), 1);
    p.MakeMove(Move("c2-c3"), 0);
    p.MakeMove(Move("b4-c5"), 1);
    p.MakeMove(Move("g1-f3"), 0);
    p.MakeMove(Move("d7-d5"), 1);
    p.MakeMove(Move("c1-g5"), 0);
    //p.MakeMove(Move("d2-d4"), 0);
    Move x=p.ChoseMove(1);
    cout<<x.GetString();
    /*p.MakeMove(Move("g8-f6"), 1);
    p.MakeMove(Move("b1-c3"), 0);
    p.MakeMove(Move("c7-c6"), 1);
    p.MakeMove(Move("e4-e5"), 0);
    p.MakeMove(Move("f6-h7"), 1);
    p.MakeMove(Move("g1-f3"), 0);
    p.MakeMove(Move("d7-d5"), 1);
    p.MakeMove(Move("f1-d3"), 0);
    p.MakeMove(Move("b8-d7"), 1);
    p.MakeMove(Move("0-0"), 0);
    p.MakeMove(Move("d8-a5"), 1);
    p.MakeMove(Move("c1-d2"), 0);
    p.MakeMove(Move("e8-d8"), 1);
    p.MakeMove(Move("a2-a3"), 0);
    p.MakeMove(Move("f7-f6"), 1);
    p.MakeMove(Move("c3-a4"), 0);
    Move x=p.ChoseMove(1);
    cout<<x.GetString()<<endl;
    /*p.MakeMove(Move("d5-d4"), 1);
    p.MakeMove(Move("e3-f3"), 0);
    p.MakeMove(Move("c7-c5"), 1);
    p.MakeMove(Move("b4-c5"), 0);
    p.MakeMove(Move("b6-c5"), 1);
    p.MakeMove(Move("c2-c3"), 0);
    p.MakeMove(Move("d4-c3"), 1);
    p.MakeMove(Move("e2-e3"), 0);
    p.MakeMove(Move("c3-c2"), 1);
    p.MakeMove(Move("f1-b5"), 0);
    p.MakeMove(Move("e8-e7"), 1);
    p.MakeMove(Move("b1-c3"), 0);
    p.MakeMove(Move("d8-a5"), 1);
    p.MakeMove(Move("g1-e2"), 0);
    p.MakeMove(Move("a8-b8"), 1);
    cout<<p.ChoseMove(0).GetString()<<endl;*/
    //p.MakeMove(Move("f3-e5"), 0);
    //p.MakeMove(Move("g6-h6"), 1);
    //p.MakeMove(Move("e5-g4"), 0);
    //p.MakeMove(Move("f5-g4"), 1);
    //p.MakeMove(Move("f2-f4"), 0);
    /*p.MakeMove(Move("e5-f6"), 0);
    p.Visualize();
    cout<<endl;
    p.MakeMove(Move("c8-f5"), 0);
    p.MakeMove(Move("f6-g7"), 0);
    p.MakeMove(Move("d8-d7"), 0);
    p.MakeMove(Move("g7-h8B"), 0);
    p.Visualize();
    cout<<endl;
    p.MakeMove(Move("0-0-0"), 0);
    p.Visualize();
    cout<<endl;
    p.MakeMove(Move("b1-c3"), 0);
    p.MakeMove(Move("c1-e3"), 0);
    p.MakeMove(Move("0-0-0"), 0);
    p.Visualize();
    cout<<endl;
    vector<Move> vm = p.PossibleMoves(0);
    int i=p.MateMove(3, 0);
    cout<<i<<endl;*/
    return 0;
}