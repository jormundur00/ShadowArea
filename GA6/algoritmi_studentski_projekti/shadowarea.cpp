#include "shadowarea.h"

ShadowArea::ShadowArea(QWidget *pCrtanje,
                   int pauzaKoraka,
                   const bool &naivni,
                   std::string imeDatoteke,
                   int brojPrepreka)
    :AlgoritamBaza(pCrtanje, pauzaKoraka, naivni) {

    _brojPrepreka = 0;
    _zavrsenAlgoritam = false;
    if (imeDatoteke == ""){
        _soba = generisiNasumicnuSobu(brojPrepreka);
    }  else {
        _soba = ucitajPodatkeIzDatoteke(imeDatoteke);
    }
    parsirajSobu();
}

ShadowArea::~ShadowArea() {
}

void ShadowArea::pokreniAlgoritam() {
    _brisucaPrava = QLineF(_izvorSvetlosti, QPointF(_izvorSvetlosti.x() - 1000, _izvorSvetlosti.y()));
    std::set<tackaDogadjajaSA> dogadjaji;
    std::set<QLineF, poredjenjeIvicaStatusaSA> status{{_izvorSvetlosti, _brisucaPrava}};

    for (auto &ivica : _ivicePrepreka) {
        QPointF p1 = ivica.p1();
        QPointF p2 = ivica.p2();

        double ugao1 = izracunajUgao(p1);
        double ugao2 = izracunajUgao(p2);

        double udaljenostOdSvetla1 = QLineF(_izvorSvetlosti, p1).length();
        double udaljenostOdSvetla2 = QLineF(_izvorSvetlosti, p2).length();

        dogadjaji.insert({p1, ugao1, udaljenostOdSvetla1});
        dogadjaji.insert({p2, ugao2, udaljenostOdSvetla2});

        QPointF presek;
        if (_brisucaPrava.intersects(ivica, &presek) == QLineF::BoundedIntersection) {
            status.insert(ivica);
        }
    }

    QLineF prethodnaNajbliza = *status.begin();
    for (auto &dogadjaj : dogadjaji) {
        _brisucaPrava = QLineF(_izvorSvetlosti, dogadjaj.tacka + 10 * (dogadjaj.tacka - _izvorSvetlosti));
        for (auto &ivica : _endIvice[dogadjaj.tacka]) {
            status.erase(ivica);
        }
        for (auto &ivica : _startIvice[dogadjaj.tacka]) {
            status.insert(ivica);
        }
        if (!status.empty()) {
            QLineF najblizaIvica = *status.begin();
            if (najblizaIvica != prethodnaNajbliza) {
                QPointF presek;
                if (_brisucaPrava.intersects(prethodnaNajbliza, &presek) == QLineF::BoundedIntersection) {
                    _vidljiveTacke.push_back(presek);
                }
            }
            QPointF presek;
            if (_brisucaPrava.intersects(najblizaIvica, &presek) == QLineF::BoundedIntersection) {
                _vidljiveTacke.push_back(presek);
            }
            prethodnaNajbliza = najblizaIvica;
            AlgoritamBaza_updateCanvasAndBlock();
        }
    }
    _zavrsenAlgoritam = true;
    AlgoritamBaza_updateCanvasAndBlock();
}

void ShadowArea::crtajAlgoritam(QPainter *painter) const {
    if (!painter) {
        return;
    }
    int cellSize = 90, svetloX = -1, svetloY = -1;

    painter->setPen(Qt::lightGray);
    for (int x = 0; x <= _m; x++) {
        painter->drawLine(x * cellSize, 0, x * cellSize, _n * cellSize);
    }
    for (int y = 0; y <= _n; y++) {
        painter->drawLine(0, y * cellSize, _m * cellSize, y * cellSize);
    }

    for (int y = 0; y < _n; y++) {
        for (int x = 0; x < _m; x++) {
            if (_soba[y][x] == '#') {
                painter->setBrush(Qt::darkGray);
                painter->drawRect(x * cellSize, y * cellSize, cellSize, cellSize);
            }
            else if (_soba[y][x] == '*') {
                painter->setBrush(Qt::yellow);
                painter->drawRect(x * cellSize, y * cellSize, cellSize, cellSize);
                svetloX = x;
                svetloY = y;
            }
        }
    }
    int brojac = 0;
    auto pen = painter->pen();
    pen.setColor(Qt::green);
    pen.setWidth(10);
    painter->setPen(pen);
    for (auto &tacka : _vidljiveTacke) {
        painter->drawPoint(tacka.x() * cellSize, tacka.y() * cellSize);
        painter->drawText(tacka.x() * cellSize + 5, tacka.y() * cellSize + 5, QString::number(brojac));
        brojac++;
    }
    if (_vidljiveTacke.size() > 0 && !_zavrsenAlgoritam) {
        painter->setPen(Qt::green);
        painter->drawLine(_brisucaPrava.x1() * cellSize, _brisucaPrava.y1() * cellSize, _brisucaPrava.x2() * cellSize, _brisucaPrava.y2() * cellSize);
    }
    if (_zavrsenAlgoritam) {
        QPolygonF poligon, vPoligon;
        for (const auto &tacka : _vidljiveTacke) {
            poligon << QPointF(tacka.x() * cellSize, tacka.y() * cellSize);
            vPoligon << tacka;
        }
        painter->setPen(Qt::yellow);
        painter->setBrush(Qt::yellow);
        painter->drawPolygon(poligon);
        painter->setBrush(Qt::darkYellow);
        painter->drawRect(svetloX * cellSize, svetloY * cellSize, cellSize, cellSize);
        double povrsinaSenke = (_n * _m) - povrsinaPoligona(vPoligon) - _brojPrepreka;
        qDebug() << "Povrsina senke: " << povrsinaSenke;
    }
}

void ShadowArea::pokreniNaivniAlgoritam() {
    _brisucaPrava = QLineF(_izvorSvetlosti, QPointF(_izvorSvetlosti.x() - 1000, _izvorSvetlosti.y()));
    std::set<tackaDogadjajaSA> dogadjaji;

    for (auto &ivica : _ivicePrepreka) {
        QPointF p1 = ivica.p1();
        QPointF p2 = ivica.p2();

        double ugao1 = izracunajUgao(p1);
        double ugao2 = izracunajUgao(p2);

        double udaljenostOdSvetla1 = QLineF(_izvorSvetlosti, p1).length();
        double udaljenostOdSvetla2 = QLineF(_izvorSvetlosti, p2).length();

        dogadjaji.insert({p1, ugao1, udaljenostOdSvetla1});
        dogadjaji.insert({p2, ugao2, udaljenostOdSvetla2});
    }

    for (auto &dogadjaj : dogadjaji) {
        qDebug() << "Obradjujemo: " << dogadjaj.tacka;
        QPointF prviPresek, drugiPresek;
        QLineF prethodnaNajbliza;
        double udaljenostPrvog = std::numeric_limits<double>::max();
        double udaljenostDrugog = std::numeric_limits<double>::max();
        _brisucaPrava = QLineF(_izvorSvetlosti, dogadjaj.tacka + 10 * (dogadjaj.tacka - _izvorSvetlosti));
        QLineF najblizaIvica, drugaNajblizaIvica;

        for (auto &ivica : _ivicePrepreka) {
            QPointF presek;
            if (_brisucaPrava.intersects(ivica, &presek) == QLineF::BoundedIntersection) {
                double udaljenostPreseka = QLineF(_izvorSvetlosti, presek).length();

                if (udaljenostPreseka < udaljenostPrvog) {
                    udaljenostDrugog = udaljenostPrvog;
                    drugiPresek = prviPresek;
                    drugaNajblizaIvica = najblizaIvica;

                    udaljenostPrvog = udaljenostPreseka;
                    prviPresek = presek;
                    najblizaIvica = ivica;
                }
                else if (udaljenostPreseka < udaljenostDrugog) {
                    if (prviPresek != presek) {
                        udaljenostDrugog = udaljenostPreseka;
                        drugiPresek = presek;
                        drugaNajblizaIvica = ivica;
                    }
                }
            }
        }

        if (_startIvice.find(prviPresek) != _startIvice.end() && _endIvice.find(prviPresek) == _endIvice.end()) {
            _vidljiveTacke.push_back(drugiPresek);
            _vidljiveTacke.push_back(prviPresek);
        } else if (_endIvice.find(prviPresek) != _endIvice.end() && _startIvice.find(prviPresek) == _startIvice.end()) {
            _vidljiveTacke.push_back(prviPresek);
            _vidljiveTacke.push_back(drugiPresek);
        } else {
            _vidljiveTacke.push_back(prviPresek);
        }

        prethodnaNajbliza = najblizaIvica;
        AlgoritamBaza_updateCanvasAndBlock();
    }

    _zavrsenAlgoritam = true;
    AlgoritamBaza_updateCanvasAndBlock();
}



void ShadowArea::crtajNaivniAlgoritam(QPainter *painter) const {
    crtajAlgoritam(painter);
}

std::vector<std::string> ShadowArea::generisiNasumicnuSobu(int brojPrepreka) {
    srand(static_cast<unsigned>(time(nullptr)));

    do {
        _n = 2 + rand() % 9;
        _m = 2 + rand() % 9;
    } while (_n * _m <= brojPrepreka);

    std::vector<std::string> soba(_n, std::string(_m, '.'));

    int izvorSvetlostiY = rand() % _n;
    int izvorSvetlostiX = rand() % _m;
    soba[izvorSvetlostiY][izvorSvetlostiX] = '*';

    int postavljenePrepreke = 0;
    while (postavljenePrepreke < brojPrepreka) {
        int x = rand() % _n;
        int y = rand() % _m;

        if (soba[x][y] == '.') {
            soba[x][y] = '#';
            postavljenePrepreke++;
        }
    }

    return soba;
}

std::vector<std::string> ShadowArea::ucitajPodatkeIzDatoteke(std::string imeDatoteke) {
    std::ifstream fajl(imeDatoteke);
    std::vector<std::string> soba;

    if (fajl.is_open()) {
        std::string linija;
        while (std::getline(fajl, linija)) {
            soba.push_back(linija);
        }
        fajl.close();
    } else {
        std::cerr << "Greska u otvaranju fajla: " << imeDatoteke << std::endl;
    }

    return soba;
}

void ShadowArea::parsirajSobu() {
    _n = _soba.size();
    _m = _soba[0].size();

    for (int i = 0; i < _n; i++) {
        for (int j = 0; j < _m; j++) {
            char cell = _soba[i][j];
            if (cell == '*') {
                _izvorSvetlosti = QPointF(j + 0.5, i + 0.5);
            }
        }
    }

    for (int i = 0; i < _n; ++i) {
        for (int j = 0; j < _m; ++j) {
            char cell = _soba[i][j];
            QPointF doleLevo(j, i);
            QPointF doleDesno(j + 1, i);
            QPointF goreLevo(j, i + 1);
            QPointF goreDesno(j + 1, i + 1);
            if (cell == '#') {
                dodajIvicu(doleLevo, doleDesno);
                dodajIvicu(doleDesno, goreDesno);
                dodajIvicu(goreDesno, goreLevo);
                dodajIvicu(goreLevo, doleLevo);

                _brojPrepreka++;
            }
        }
    }
    parsirajZidove();
}

void ShadowArea::parsirajZidove() {
    int pocetak = -1;
    for (int j = 0; j < _m; j++) {
        if (_soba[0][j] == '.' || _soba[0][j] == '*') {
            if (pocetak == -1) {
                pocetak = j;
            }
        } else {
            if (pocetak != -1) {
                dodajIvicu(QPointF(pocetak, 0), QPointF(j, 0));
                pocetak = -1;
            }
        }
    }
    if (pocetak != -1) {
        dodajIvicu(QPointF(pocetak, 0), QPointF(_m, 0));
    }

    pocetak = -1;
    for (int j = 0; j < _m; j++) {
        if (_soba[_n - 1][j] == '.' || _soba[_n - 1][j] == '*') {
            if (pocetak == -1) {
                pocetak = j;
            }
        } else {
            if (pocetak != -1) {
                dodajIvicu(QPointF(j, _n), QPointF(pocetak, _n));
                pocetak = -1;
            }
        }
    }
    if (pocetak != -1) {
        dodajIvicu(QPointF(_m, _n), QPointF(pocetak, _n));
    }

    pocetak = -1;
    for (int i = 0; i < _n; i++) {
        if (_soba[i][0] == '.' || _soba[i][0] == '*') {
            if (pocetak == -1) {
                pocetak = i;
            }
        } else {
            if (pocetak != -1) {
                dodajIvicu(QPointF(0, i), QPointF(0, pocetak));
                pocetak = -1;
            }
        }
    }
    if (pocetak != -1) {
        dodajIvicu(QPointF(0, _n), QPointF(0, pocetak));
    }

    pocetak = -1;
    for (int i = 0; i < _n; i++) {
        if (_soba[i][_m - 1] == '.' || _soba[i][_m - 1] == '*') {
            if (pocetak == -1) {
                pocetak = i;
            }
        } else {
            if (pocetak != -1) {
                dodajIvicu(QPointF(_m, pocetak), QPointF(_m, i));
                pocetak = -1;
            }
        }
    }
    if (pocetak != -1) {
        dodajIvicu(QPointF(_m, pocetak), QPointF(_m, _n));
    }
}

double ShadowArea::izracunajUgao(const QPointF &tacka) {
    return atan2(tacka.y() - _izvorSvetlosti.y(), tacka.x() - _izvorSvetlosti.x());
}

void ShadowArea::dodajIvicu(const QPointF &tacka1, const QPointF &tacka2) {
    int o = orijentacija(_izvorSvetlosti, tacka1, tacka2);

    if (o == 1) {
        QLineF ivica(tacka1, tacka2);
        _ivicePrepreka.insert(ivica);
        _startIvice[tacka1].push_back(ivica);
        _endIvice[tacka2].push_back(ivica);
    } else {
        QLineF ivica(tacka2, tacka1);
        _ivicePrepreka.insert(ivica);
        _startIvice[tacka2].push_back(ivica);
        _endIvice[tacka1].push_back(ivica);
    }
}

int ShadowArea::orijentacija(const QPointF &p, const QPointF &q, const QPointF &r) {
    double o = (q.x() - p.x()) * (r.y() - p.y()) - (q.y() - p.y()) * (r.x() - p.x());

    if (fabs(o) < std::numeric_limits<double>::epsilon()) {
        return 0;
    }
    return (o > 0) ? 1 : -1;
}

double ShadowArea::povrsinaPoligona(const QPolygonF &p) const {
    int n = p.size();

    double povrsina = 0.0;
    for (int i = 0; i < n; ++i) {
        const QPointF &p1 = p[i];
        const QPointF &p2 = p[(i + 1) % n];
        povrsina += (p1.x() * p2.y()) - (p2.x() * p1.y());
    }
    return qAbs(povrsina) / 2.0;
}
