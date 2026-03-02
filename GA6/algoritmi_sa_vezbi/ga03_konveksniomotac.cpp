#include "ga03_konveksniomotac.h"
#include "pomocnefunkcije.h"

#include <algorithm>
#include <QPainterPath>

KonveksniOmotac::KonveksniOmotac(QWidget *pCrtanje,
                                 int pauzaKoraka,
                                 const bool &naivni,
                                 std::string imeDatoteke,
                                 int brojTacaka)
    : AlgoritamBaza(pCrtanje, pauzaKoraka, naivni)
{
      if (imeDatoteke == ""){
          _tacke = generisiNasumicneTacke(brojTacaka);
      }  else {
          _tacke = ucitajPodatkeIzDatoteke(imeDatoteke);
      }
      _k = _tacke.size();
      kraj = false;
}

void KonveksniOmotac::pokreniAlgoritam() {
    /* Slozenost ovakvog (Gremovog) algoritma: O(nlogn).
     * Dominira sortiranje, dok su ostali koraci linearni. */

    _maxTacka = _tacke[0];
    for (int i = 1; i<_k; i++){
    if ((_tacke[i].x() > _maxTacka.x()) || (abs(_tacke[i].x() -_maxTacka.x())<0.0001 &&
                                          _tacke[i].y() < _maxTacka.y())){
                _maxTacka = _tacke[i];
        }
    }

    AlgoritamBaza_updateCanvasAndBlock();

    std::sort(_tacke.begin(), _tacke.end(), [&](const auto& lhs, const auto& rhs){
            double p = pomocneFunkcije::povrsinaTrougla(_maxTacka, lhs, rhs);
            return ((p<0) || (abs(p)<0.0001 && pomocneFunkcije::distanceKvadratF(_maxTacka,lhs)
                              < pomocneFunkcije::distanceKvadrat(_maxTacka, rhs)));
    });

     AlgoritamBaza_updateCanvasAndBlock();

     _konveksniOmotac.push_back(_maxTacka);
     _konveksniOmotac.push_back(_tacke[1]);

     int pom = 2; // koliko trenutno imamo tacaka
     int j =2; //koju tacku trenutno razmatramo

     while (j < _k){
            if (pomocneFunkcije::povrsinaTrougla(_konveksniOmotac[pom-2],
                                                 _konveksniOmotac[pom-1],
                                                 _tacke[j]) < 0){
                    _konveksniOmotac.push_back(_tacke[j]);
                    pom++;
                    j++;
            } else {
                _konveksniOmotac.pop_back();
                pom--;
            }
            AlgoritamBaza_updateCanvasAndBlock();
     }


    AlgoritamBaza_updateCanvasAndBlock();
    kraj = true;
    emit animacijaZavrsila();

}

void KonveksniOmotac::crtajAlgoritam(QPainter *painter) const {
     if (!painter) return;
     auto pen = painter->pen();
     pen.setColor(Qt::red);
     painter->setPen(pen);
     for(auto &tacka: _tacke){
         painter->drawPoint(tacka);
     }
     pen = painter->pen();
     pen.setColor(Qt::blue);
     pen.setWidth(5);
     painter->setPen(pen);
     painter->drawPoint(_maxTacka);

     for(int i=0;i<_k;i++){
         painter->drawText(_tacke[i], QString::number(i));
     }

        if (!_konveksniOmotac.empty()){
          pen = painter->pen();
          pen.setColor(Qt::green);
          pen.setWidth(10);
          painter->setPen(pen);
          for (unsigned i=0; i<_konveksniOmotac.size()-1; i++){
              painter->drawLine(_konveksniOmotac[i],_konveksniOmotac[i+1]);
          }
          painter->drawLine(_konveksniOmotac[_konveksniOmotac.size()-1], _konveksniOmotac[0]);
    }

}

void KonveksniOmotac::pokreniNaivniAlgoritam() {
   for (int i=0; i<_k;i++){
       for (int j=i+1; j<_k;j++){

           int orientation = 0;
           bool stranica = true;
           for (int k=0; k<_k;k++){
               if (k==i || k==j) continue;
               if (orientation != 0){
                    if (pomocneFunkcije::povrsinaTrougla(_tacke[i],_tacke[j],_tacke[k])
                            * orientation < 0){
                        stranica = false;
                        break;
                    }
               } else {
                   if (pomocneFunkcije::povrsinaTrougla(_tacke[i],_tacke[j], _tacke[k]) <0)
                        orientation = -1;
                   else
                        orientation = 1;
               }
           }
           if (stranica){
               _naivniOmotac.push_back(std::make_pair(_tacke[i],_tacke[j]));
           }
       }
   }
   AlgoritamBaza_updateCanvasAndBlock();
   kraj = true;
   emit animacijaZavrsila();

}

/*
void KonveksniOmotac::naglasiTrenutno(QPainter *painter, unsigned long i, const char *s) const
{

}*/

void KonveksniOmotac::crtajNaivniAlgoritam(QPainter *painter) const
{
    if (!painter) return;
    auto pen = painter->pen();
    pen.setColor(Qt::red);
    painter->setPen(pen);
    for(auto &tacka: _tacke){
        painter->drawPoint(tacka);
    }
       pen = painter->pen();
      pen.setColor(Qt::green);
      pen.setWidth(10);
      painter->setPen(pen);

      for (auto & par : _naivniOmotac)
          painter->drawLine(par.first, par.second);
}

const std::vector<std::pair<QPoint,QPoint>> &KonveksniOmotac::getNaivniOmotac() const
{
    return _naivniOmotac;
}

const std::vector<QPoint> &KonveksniOmotac::getKonveksniOmotac() const
{
    return _konveksniOmotac;
}
