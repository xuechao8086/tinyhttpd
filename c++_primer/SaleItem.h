#ifndef SALESITEM_H
#define SALESITEM_H
#include "head.h"

class Sales_item {
//    friend bool operator==(const Sales_item &, const Sales_item &);
    friend Sales_item & operator+(Sales_item &, Sales_item &); 
    public:
        Sales_item():units_sold(0), revenue(0.0){};
        Sales_item(const std::string &book):
            isbn(book), units_sold(0), revenue(0.0) {};
        Sales_item(std::istream &is) { is>>*this; };
        Sales_item(Sales_item &s):
            isbn(s.isbn), units_sold(s.units_sold), revenue(s.revenue) {};
        
        friend std::istream & operator>>(std::istream&, Sales_item &);
        friend std::ostream & operator<<(std::ostream&, const Sales_item &);

    public:
        Sales_item & operator+=(const Sales_item &s);

    public:
        double avg_price();
        bool same_isbn(const Sales_item &rhs) const;

    private:
        std::string isbn;
        unsigned units_sold;
        double revenue;
};


// inline bool operator==(const Sales_item &a, const Sales_item &b) {
//     if (a.isbn == b.isbn && a.units_sold == b.units_sold && a.revenue == b.revenue) {
//         return true;
//     } else {
//         return false;
//     }
// }

inline Sales_item & operator +(Sales_item &lhs, Sales_item &rhs) {
    lhs += rhs; 
    return lhs;
}


inline std::istream & operator >> (std::istream &in, Sales_item &si) {
    double price;
    in>>si.isbn>>si.units_sold>>price;
    if(in) {
        si.revenue = si.units_sold*price;
    }
    else {
        si = Sales_item();
    }
    return in;
}

inline std::ostream & operator<<(std::ostream &out, const Sales_item &s) {
    out<<"isbn:"<<s.isbn<<std::endl;
    out<<"unit:"<<s.units_sold<<std::endl;
    out<<"revenue:"<<s.revenue<<std::endl;

    return out;
}

inline Sales_item & Sales_item::operator+=(const Sales_item &s){
    units_sold += s.units_sold;
    revenue += s.revenue;

    return *this;
}

double Sales_item::avg_price() {
    return revenue/units_sold;
}

bool Sales_item::same_isbn(const Sales_item &rhs) const {
    return isbn==rhs.isbn; 
}


#endif
