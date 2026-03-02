#ifndef SHADOWAREA_H
#define SHADOWAREA_H

#include "algoritambaza.h"
#include "shadowarea_datastructures.h"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <fstream>

using namespace std;

class ShadowArea : public AlgoritamBaza
{
public:
    ShadowArea(QWidget *pCrtanje,
             int pauzaKoraka,
             const bool &naivni = false,
             std::string imeDatoteke = "",
             int brojPrepreka = BROJ_SLUCAJNIH_OBJEKATA);

    virtual ~ShadowArea() override;

    void pokreniAlgoritam() final;
    void crtajAlgoritam(QPainter *painter) const final;
    void pokreniNaivniAlgoritam() final;
    void crtajNaivniAlgoritam(QPainter *painter) const final;

    std::vector<std::string> generisiNasumicnuSobu(int brojPrepreka);
    std::vector<std::string> ucitajPodatkeIzDatoteke(string imeDatoteke);
    void parsirajSobu();
    void parsirajZidove();
    double izracunajUgao(const QPointF &tacka);
    void dodajIvicu(const QPointF &tacka1, const QPointF &tacka2);
    static int orijentacija(const QPointF &p, const QPointF &q, const QPointF &r);
    double povrsinaPoligona(const QPolygonF &polygon) const;

private:
    int _n, _m, _brojPrepreka;
    bool _zavrsenAlgoritam;
    QPointF _izvorSvetlosti;
    QLineF _brisucaPrava;
    std::vector<std::string> _soba;
    std::vector<QPointF> _vidljiveTacke;
    std::set<QLineF, QLineFComparator> _ivicePrepreka;
    std::unordered_map<QPointF, std::vector<QLineF>, QPointFHash> _startIvice;
    std::unordered_map<QPointF, std::vector<QLineF>, QPointFHash> _endIvice;
};

struct poredjenjeIvicaStatusaSA {
    const QPointF _izvorSvetlosti;
    const QLineF* _brisucaPrava;

    poredjenjeIvicaStatusaSA(const QPointF& izvorSvetlosti, const QLineF& brisucaPrava)
        : _izvorSvetlosti(izvorSvetlosti), _brisucaPrava(&brisucaPrava) {}

    bool operator()(const QLineF& ivica1, const QLineF& ivica2) const {
        if (ivica1.p1() == ivica2.p1() && ivica1.p2() == ivica2.p2()) {
            return false;
        }

        if (ivica1.p1() == ivica2.p2()) {
            return ShadowArea::orijentacija(ivica1.p2(), ivica1.p1(), ivica2.p1()) == 1;
        }
        if (ivica1.p1() == ivica2.p1()) {
            return ShadowArea::orijentacija(ivica1.p1(), ivica1.p2(), ivica2.p2()) == -1;
        }
        if (ivica1.p2() == ivica2.p1()) {
            return ShadowArea::orijentacija(ivica1.p1(), ivica1.p2(), ivica2.p2()) == 1;
        }
        if (ivica1.p2() == ivica2.p2()) {
            return ShadowArea::orijentacija(ivica1.p1(), ivica1.p2(), ivica2.p1()) == -1;
        }

        QPointF presek1, presek2;
        _brisucaPrava->intersects(ivica1, &presek1);
        _brisucaPrava->intersects(ivica2, &presek2);
        return QLineF(_izvorSvetlosti, presek1).length() < QLineF(_izvorSvetlosti, presek2).length();
    }
};

#endif // SHADOWAREA_H
