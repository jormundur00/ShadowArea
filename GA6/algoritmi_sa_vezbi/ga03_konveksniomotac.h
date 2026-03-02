#ifndef GA03_KONVEKSNIOMOTAC_H
#define GA03_KONVEKSNIOMOTAC_H

#include "algoritambaza.h"

class KonveksniOmotac : public AlgoritamBaza
{
public:
    KonveksniOmotac(QWidget *pCrtanje,
                    int pauzaKoraka,
                    const bool &naivni = false,
                    std::string imeDatoteke = "",
                    int brojTacaka = BROJ_SLUCAJNIH_OBJEKATA);

    void pokreniAlgoritam() final;
    void crtajAlgoritam(QPainter *painter) const final;
    void pokreniNaivniAlgoritam() final;
    void crtajNaivniAlgoritam(QPainter *painter) const final;

    const std::vector<QPoint> &getKonveksniOmotac() const;
    const std::vector<std::pair<QPoint, QPoint> > &getNaivniOmotac() const;

private:
    //void naglasiTrenutno(QPainter *painter, unsigned long i, const char *s) const;

    std::vector<QPoint> _tacke;
    std::vector<QPoint> _konveksniOmotac;
    std::vector<std::pair<QPoint,QPoint>> _naivniOmotac;
    QPoint _maxTacka;
    int _k;
    bool kraj;
};

#endif // GA03_KONVEKSNIOMOTAC_H
