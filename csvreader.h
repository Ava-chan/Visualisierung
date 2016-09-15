#ifndef CSVREADER_H
#define CSVREADER_H

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


class cCSVRow
{
    public:
        std::string const& operator[](std::size_t index) const;
        std::size_t size() const;
        void readNextRow(std::istream& str);

    private:
        std::vector<std::string> m_data;
};


class cCSVIterator
{
    public:
        typedef std::input_iterator_tag iterator_category;
        typedef cCSVRow                  value_type;
        typedef std::size_t             difference_type;
        typedef cCSVRow*                 pointer;
        typedef cCSVRow&                 reference;

        cCSVIterator(std::istream& str);
        cCSVIterator();

        // Pre Increment
        cCSVIterator& operator++();

        // Post increment
        cCSVIterator operator++(int);
        cCSVRow const& operator*() const;
        cCSVRow const* operator->() const;

        bool operator==(cCSVIterator const& rhs);
        bool operator!=(cCSVIterator const &rhs);

    private:
        std::istream* m_str;
        cCSVRow m_row;
};

#endif // CSVREADER_H
