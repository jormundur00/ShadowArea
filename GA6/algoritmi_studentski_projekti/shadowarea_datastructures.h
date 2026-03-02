#ifndef SHADOWAREA_DATASTRUCTURES_H
#define SHADOWAREA_DATASTRUCTURES_H

struct tackaDogadjajaSA {
    tackaDogadjajaSA(const QPointF &t,
                     double ugao,
                     double udaljenostOdSvetla)
        : tacka(t), ugao(ugao), udaljenostOdSvetla(udaljenostOdSvetla) {}

    QPointF tacka;
    double ugao;
    double udaljenostOdSvetla;

    bool operator<(const tackaDogadjajaSA& other) const {
        if (ugao != other.ugao) {
            return ugao < other.ugao;
        }
        return udaljenostOdSvetla < other.udaljenostOdSvetla;
    }
};

struct QPointFHash {
    std::size_t operator()(const QPointF& p) const {
        std::size_t h1 = std::hash<double>{}(p.x());
        std::size_t h2 = std::hash<double>{}(p.y());
        return h1 ^ (h2 << 1);
    }
};

struct QPointFComparator {
    bool operator()(const QPointF& p1, const QPointF& p2) const {
        if (p1.x() != p2.x()) {
            return p1.x() < p2.x();
        }
        return p1.y() < p2.y();
    }
};

struct QLineFComparator {
    bool operator()(const QLineF& line1, const QLineF& line2) const {
        bool sameDirection = (line1.p1() == line2.p1() && line1.p2() == line2.p2());
        bool oppositeDirection = (line1.p1() == line2.p2() && line1.p2() == line2.p1());

        if (sameDirection || oppositeDirection) return false;

        QPointFComparator comparePoints;
        if (line1.p1() != line2.p1()) {
            return comparePoints(line1.p1(), line2.p1());
        }
        return comparePoints(line1.p2(), line2.p2());
    }
};

#endif // SHADOWAREA_DATASTRUCTURES_H
