#include "csvreader.h"

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


/* *************** CSVRow ****************************************************/
std::string const& cCSVRow::operator[](std::size_t index) const
{
    return m_data[index];
}


std::size_t cCSVRow::size() const
{
    return m_data.size();
}


void cCSVRow::readNextRow(std::istream& str)
{
    std::string line;
    std::getline(str, line);

    std::stringstream lineStream(line);
    std::string cell;

    m_data.clear();
    while (std::getline(lineStream, cell, '\t'))
    {
        m_data.push_back(cell);
    }
}


std::istream& operator>>(std::istream& str, cCSVRow& data)
{
    data.readNextRow(str);
    return str;
}

/* *************** CSVRow ****************************************************/
cCSVIterator::cCSVIterator(std::istream& str) : m_str(str.good() ? &str : NULL)
{
    ++(*this);
}


cCSVIterator::cCSVIterator() : m_str(NULL) { }


// Pre Increment
cCSVIterator& cCSVIterator::operator++()
{
    if (m_str)
    {
        if (!((*m_str) >> m_row))
        {
            m_str = NULL;
        }
    }
    return *this;
}


// Post increment
cCSVIterator cCSVIterator::operator++(int)
{
    cCSVIterator tmp(*this);
    ++(*this);
    return tmp;
}


cCSVRow const& cCSVIterator::operator*() const
{
    return m_row;
}


cCSVRow const* cCSVIterator::operator->() const
{
    return &m_row;
}


bool cCSVIterator::operator==(cCSVIterator const& rhs)
{
    return ((this == &rhs) || ((this->m_str == NULL) && (rhs.m_str == NULL)));
}


bool cCSVIterator::operator!=(cCSVIterator const &rhs)
{
    return !((*this) == rhs);
}
